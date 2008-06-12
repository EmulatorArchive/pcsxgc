/*  Pcsx - Pc Psx Emulator
 *  Copyright (C) 1999-2003  Pcsx Team
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "PsxCommon.h"

//#define SAME_CYCLE_MODE

//#define INT_LOG(STR, ...) {fprintf(logfile, STR, ## __VA_ARGS__); fflush(logfile);}

#ifdef INT_LOG
static FILE *logfile;
static int branchLog = 0;

static void _branch_log_handler() {
}
#endif

static void _exc_int_handler();

// sorted list of timer events
timer_s timed_interrupts[MAX_TIMERS];
static int num_timers;
static u32 new_ints;

void TimedInterruptsReset(u32 time)
{
	timed_interrupts[0].when = time;
	timed_interrupts[0].handler = NULL;
	psxRegs.cycle = 0;
	num_timers = 0;
	new_ints = 0;
	
	psxIntAdd(psxRegs.interrupt);
	psxExceptionTest();
	psxRcntSet();

#ifdef INT_LOG
	if (logfile) fclose(logfile);
	logfile = fopen("int.log", "w");
#endif
}

// called when psxReg.cycles <= 0
void TimedInterruptHandle()
{
	int cycle = psxCurrentCycle;
	
#ifdef INT_LOG
	INT_LOG("INT: PC=0x%08x, v0=0x%08x, CNT=%i\n", psxRegs.pc, psxRegs.GPR.n.v0, psxCurrentCycle);
#if 1
	if (psxCurrentCycle == 73763) {
		branchLog = 1;
	} else if (psxCurrentCycle >= 98340+100) {
		branchLog = 0;
	}
#endif
#endif
	
	if (num_timers == 0) {
		RemoveTimedInterrupt(NULL);
		return;
	}
	
	// TODO: optimize this
	while (num_timers && ((s32)(timed_interrupts[0].when - cycle) <= 0)) {
		int_handler handler = timed_interrupts[0].handler;
		
		RemoveTimedInterrupt(handler);
		
		(handler)();
	}
	
	// check if any of the int's has made an exception
	if (psxHu32ref(0x1070) & psxHu32ref(0x1074)) {
		if ((psxRegs.CP0.n.Status & 0x401) == 0x401) {
			_exc_int_handler();
		}
	}

#ifdef INT_LOG
	if (branchLog)
		AddTimedInterrupt(_branch_log_handler, psxCurrentCycle);
#endif
}

void AddTimedInterrupt(int_handler handler, u32 when)
{
	int cycle = psxCurrentCycle;
	int i;
	
	RemoveTimedInterrupt(handler);
	
	if (num_timers >= MAX_TIMERS) {
		// FATAL ERROR!
		SysPrintf("Error! Trying to add another timer when queue is full");
		return;
	}
	
	// TODO: use better insertion algorithm
	for (i=0; i<num_timers; i++) {
		if ((s32)((u32)timed_interrupts[i].when - (u32)when) >= 0) {
			/*if (timed_interrupts[i].when == when) {
				// times are equal - we can't have that.
				when += 1; // add 1 unit to the newcomer
				continue;
			}*/
			
			memmove(&timed_interrupts[i+1], &timed_interrupts[i], (num_timers-i)*sizeof(timer_s));
			break;
		}
	}
	
	// fill in the information
	timed_interrupts[i].handler = handler;
	timed_interrupts[i].when = when;
	
	if (i == 0) {
		// update next event timer
		psxRegs.cycle = timed_interrupts[0].when - cycle;
	}
	
	num_timers++;
}

// removes first instance of handler
int RemoveTimedInterrupt(int_handler handler)
{
	int cycle = psxCurrentCycle;
	int was_timers = num_timers;
	int i;
	
	for (i=0; i<num_timers; i++) {
		if (timed_interrupts[i].handler == handler) {
			num_timers--;
			memmove(&timed_interrupts[i], &timed_interrupts[i+1], (num_timers-i)*sizeof(timer_s));
			break;
		}
	}
	
	if (i == 0) {
		if (num_timers == 0) {
			timed_interrupts[0].when = cycle + 0x7fffffff;
			timed_interrupts[0].handler = NULL;
		}
		psxRegs.cycle = timed_interrupts[0].when - cycle;
	}
	
	return num_timers - was_timers;
}


static void _sio_int_handler() {
//	PSXCPU_LOG("Timed Interrupt: %x\n", psxRegs.interrupt);
	if (!Config.Sio) {
		psxRegs.interrupt&=~0x80;
		sioInterrupt();
	}
}
static void _cdr_int_handler() {
//	PSXCPU_LOG("Timed Interrupt: %x\n", psxRegs.interrupt);
	psxRegs.interrupt&=~0x04;
	cdrInterrupt();
}
static void _cdrread_int_handler() {
//	PSXCPU_LOG("Timed Interrupt: %x\n", psxRegs.interrupt);
	psxRegs.interrupt&=~0x040000;
	cdrReadInterrupt();
}
static void _gpudma_int_handler() {
//	PSXCPU_LOG("Timed Interrupt: %x\n", psxRegs.interrupt);
	psxRegs.interrupt&=~0x01000000;
	gpuInterrupt();
}
static void _mdecdma_int_handler() {
//	PSXCPU_LOG("Timed Interrupt: %x\n", psxRegs.interrupt);
	psxRegs.interrupt&=~0x02000000;
	mdec1Interrupt();
}

static int_handler _psx_int_handler(unsigned long which)
{
	switch (which) {
		case 0x04:			return _cdr_int_handler;
		case 0x80:			return _sio_int_handler;
		case 0x40000:		return _cdrread_int_handler;
		case 0x01000000:	return _gpudma_int_handler;
		case 0x02000000:	return _mdecdma_int_handler;
	}
	
	return NULL;
}

static void _install_psx_int_handler()
{
	int i;
	
	for (i=0; i<32; i++) {
		if (new_ints & (1 << i)) {
			int_handler handler = _psx_int_handler(1 << i);
			
			if (handler) {
				unsigned long when = 0;
				switch (1 << i) {
					case 0x04:
						psxRegs.intCycle[2] = psxCurrentCycle;
						when = psxRegs.intCycle[2] + psxRegs.intCycle[2+1];
						break;
					case 0x80:
						psxRegs.intCycle[7] = psxCurrentCycle;
						when = psxRegs.intCycle[7] + psxRegs.intCycle[7+1];
						break;
					case 0x40000:
						psxRegs.intCycle[2+16] = psxCurrentCycle;
						when = psxRegs.intCycle[2+16] + psxRegs.intCycle[2+16+1];
						break;
					case 0x01000000:
						psxRegs.intCycle[3+24] = psxCurrentCycle;
						when = psxRegs.intCycle[3+24] + psxRegs.intCycle[3+24+1];
						break;
					case 0x02000000:
						psxRegs.intCycle[5+24] = psxCurrentCycle;
						when = psxRegs.intCycle[5+24] + psxRegs.intCycle[5+24+1];
						break;
				}
				//RemoveTimedInterrupt(handler);
				AddTimedInterrupt(handler, when);
			} else {
				new_ints &= ~(1 << i);
			}
		}
	}

	psxRegs.interrupt |= new_ints;
	new_ints = 0;
}

// the correct psxRegs.intCycle[x]'s must have been set before calling this
void psxIntAdd(unsigned long which)
{
#ifdef SAME_CYCLE_MODE
	new_ints |= which;
	
	AddTimedInterrupt(_install_psx_int_handler, psxCurrentCycle);
#else
	int i;
	
	for (i=0; i<32; i++) {
		if (which & (1 << i)) {
			int_handler handler = _psx_int_handler(1 << i);
			
			if (handler) {
				unsigned long when = 0;
				switch (1 << i) {
					case 0x04:
						when = psxRegs.intCycle[2] + psxRegs.intCycle[2+1];
						break;
					case 0x80:
						when = psxRegs.intCycle[7] + psxRegs.intCycle[7+1];
						break;
					case 0x40000:
						when = psxRegs.intCycle[2+16] + psxRegs.intCycle[2+16+1];
						break;
					case 0x01000000:
						when = psxRegs.intCycle[3+24] + psxRegs.intCycle[3+24+1];
						break;
					case 0x02000000:
						when = psxRegs.intCycle[5+24] + psxRegs.intCycle[5+24+1];
						break;
				}
				//RemoveTimedInterrupt(handler);
				AddTimedInterrupt(handler, when);
			} else {
				which &= ~(1 << i);
			}
		}
	}

	psxRegs.interrupt |= which;
#endif
}

void psxIntRemove(unsigned long which)
{
	int i;
	
	for (i=0; i<32; i++) {
		if (which & (1 << i)) {
			int_handler handler = _psx_int_handler(1 << i);
			
			if (handler) {
				RemoveTimedInterrupt(handler);
			}
		}
	}

	psxRegs.interrupt &= ~which;
	new_ints &= ~which;
}

static void _exc_int_handler()
{
#ifdef PSXCPU_LOG
	//PSXCPU_LOG("Interrupt: %x %x\n", psxHu32(0x1070), psxHu32(0x1074));
#endif
//	SysPrintf("Interrupt (%x): %x %x\n", psxCurrentCycle, psxHu32(0x1070), psxHu32(0x1074));
	psxException(0x400, 0);
}

void psxExceptionTest()
{
	if (psxHu32ref(0x1070) & psxHu32ref(0x1074)) {
		if ((psxRegs.CP0.n.Status & 0x401) == 0x401) {
			AddTimedInterrupt(_exc_int_handler, psxCurrentCycle);
		}
	} else {
		RemoveTimedInterrupt(_exc_int_handler);
	}
}