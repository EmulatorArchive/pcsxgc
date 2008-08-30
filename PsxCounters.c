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

#include <string.h>

#include "PsxCommon.h"

psxCounter psxCounters[5];
//unsigned long psxNextCounter;

static int cnts = 4;

static void psxRcntUpd(unsigned long index) {
	psxCounters[index].sCycle = psxCurrentCycle;
	if (((!(psxCounters[index].mode & 1)) || (index!=2)) &&
		psxCounters[index].mode & 0x30) {
		if (psxCounters[index].mode & 0x10) { // Interrupt on target
			psxCounters[index].Cycle = ((psxCounters[index].target - psxCounters[index].count) * psxCounters[index].rate) >> BIAS;
		} else { // Interrupt on 0xffff
			psxCounters[index].Cycle = ((0xffff - psxCounters[index].count) * psxCounters[index].rate) >> BIAS;
		}
	} else psxCounters[index].Cycle = 0xffffffff;
//	if (index == 2) SysPrintf("Cycle %x\n", psxCounters[index].Cycle);
}

static void psxRcntReset(unsigned long index) {
//	SysPrintf("psxRcntReset %x (mode=%x)\n", index, psxCounters[index].mode);
	psxCounters[index].count = 0;
	psxRcntUpd(index);

//	if (index == 2) SysPrintf("rcnt2 %x\n", psxCounters[index].mode);
	psxHu32ref(0x1070)|= SWAPu32(psxCounters[index].interrupt);
	if (!(psxCounters[index].mode & 0x40)) { // Only 1 interrupt
		psxCounters[index].Cycle = 0xffffffff;
	} // else Continuos interrupt mode
	psxExceptionTest();
}

void psxRcntSet() {
	u32 cycle = psxCurrentCycle;
	s32 next = 0x7fffffff;
	int i;
	
	for (i=0; i<cnts; i++) {
		s32 count;

		if (psxCounters[i].Cycle == 0xffffffff) continue;

		count = (u32)psxCounters[i].Cycle - (cycle - (u32)psxCounters[i].sCycle);

/*		if (count < 0) {
			psxRegs.cycle = count; break;
		}*/

		if (count < next) {
			next = count;
		}
	}
	
	AddTimedInterrupt(psxRcntUpdate, cycle + next);
}

void psxRcntInit() {

	memset(psxCounters, 0, sizeof(psxCounters));

	psxCounters[0].rate = 1; psxCounters[0].interrupt = 0x10;
	psxCounters[1].rate = 1; psxCounters[1].interrupt = 0x20;
	psxCounters[2].rate = 1; psxCounters[2].interrupt = 0x40;

	psxCounters[3].interrupt = 1;
	psxCounters[3].mode = 0x58; // The VSync counter mode
	psxCounters[3].target = 1;
	psxUpdateVSyncRate();

	if (SPU_async != NULL) {
		cnts = 5;

		psxCounters[4].rate = 768 * 64;
		psxCounters[4].target = 1;
		psxCounters[4].mode = 0x58;
	} else cnts = 4;
        
	psxRcntUpd(0); psxRcntUpd(1); psxRcntUpd(2); psxRcntUpd(3);
	psxRcntSet();
}

void psxUpdateVSyncRate() {
	if (Config.PsxType) // ntsc - 0 | pal - 1
	     psxCounters[3].rate = (PSXCLK / 50);// >> BIAS;
	else psxCounters[3].rate = (PSXCLK / 60);// >> BIAS;
	psxCounters[3].rate-= (psxCounters[3].rate / 262) * 22;
	if (Config.VSyncWA) psxCounters[3].rate/= 2;
}

void psxUpdateVSyncRateEnd() {
	if (Config.PsxType) // ntsc - 0 | pal - 1
	     psxCounters[3].rate = (PSXCLK / 50);// >> BIAS;
	else psxCounters[3].rate = (PSXCLK / 60);// >> BIAS;
	psxCounters[3].rate = (psxCounters[3].rate / 262) * 22;
	if (Config.VSyncWA) psxCounters[3].rate/= 2;
}

void psxRcntUpdate() {
	int cycle = psxCurrentCycle;
	if ((cycle - psxCounters[3].sCycle) >= psxCounters[3].Cycle) {
		if (psxCounters[3].mode & 0x10000) { // VSync End (22 hsyncs)
			psxCounters[3].mode&=~0x10000;
			psxUpdateVSyncRate();
			psxRcntUpd(3);
			GPU_updateLace(); // updateGPU
			SysUpdate();
#ifdef GTE_LOG
			GTE_LOG("VSync\n");
#endif
		} else { // VSync Start (240 hsyncs) 
			psxCounters[3].mode|= 0x10000;
			psxUpdateVSyncRateEnd();
			psxRcntUpd(3);
			psxHu32ref(0x1070)|= SWAPu32(1);
			psxExceptionTest();
		}
	}

	if ((cycle - psxCounters[0].sCycle) >= psxCounters[0].Cycle) {
		psxRcntReset(0);
	}

	if ((cycle - psxCounters[1].sCycle) >= psxCounters[1].Cycle) {
		psxRcntReset(1);
	}

	if ((cycle - psxCounters[2].sCycle) >= psxCounters[2].Cycle) {
		psxRcntReset(2);
	}

	if (cnts >= 5) {
		if ((cycle - psxCounters[4].sCycle) >= psxCounters[4].Cycle) {
			SPU_async((cycle - psxCounters[4].sCycle) << BIAS);
			psxRcntReset(4);
		}
	}

	psxRcntSet();
}

void psxRcntWcount(unsigned long index, unsigned long value) {
//	SysPrintf("writeCcount[%d] = %x\n", index, value);
//	PSXCPU_LOG("writeCcount[%d] = %x\n", index, value);
	psxCounters[index].count = value;
	psxRcntUpd(index);
	psxRcntSet();
}

void psxRcntWmode(unsigned long index, unsigned long value)  {
//	SysPrintf("writeCmode[%ld] = %lx\n", index, value);
	psxCounters[index].mode = value;
	psxCounters[index].count = 0;
	if(index == 0) {
		switch (value & 0x300) {
			case 0x100:
				psxCounters[index].rate = ((psxCounters[3].rate /*<< BIAS*/) / 386) / 262; // seems ok
				break;
			default:
				psxCounters[index].rate = 1;
		}
	}
	else if(index == 1) {
		switch (value & 0x300) {
			case 0x100:
				psxCounters[index].rate = (psxCounters[3].rate /*<< BIAS*/) / 262; // seems ok
				break;
			default:
				psxCounters[index].rate = 1;
		}
	}
	else if(index == 2) {
		switch (value & 0x300) {
			case 0x200:
				psxCounters[index].rate = 8; // 1/8 speed
				break;
			default:
				psxCounters[index].rate = 1; // normal speed
		}
	}

	// Need to set a rate and target
	psxRcntUpd(index);
	psxRcntSet();
}

void psxRcntWtarget(unsigned long index, unsigned long value) {
//	SysPrintf("writeCtarget[%ld] = %lx\n", index, value);
	psxCounters[index].target = value;
	psxRcntUpd(index);
	psxRcntSet();
}

unsigned long psxRcntRcount(unsigned long index) {
	u32 ret;
        
        int cycle = psxCurrentCycle;
//	if ((!(psxCounters[index].mode & 1)) || (index!=2)) {
		if (psxCounters[index].mode & 0x08) { // Wrap at target
			if (Config.RCntFix) { // Parasite Eve 2
				ret = (psxCounters[index].count + ((cycle - psxCounters[index].sCycle) / psxCounters[index].rate)) /*<< BIAS*/ & 0xffff;
			} else {
				ret = (psxCounters[index].count + ((cycle - psxCounters[index].sCycle) / psxCounters[index].rate)) << BIAS & 0xffff;
			}
		} else { // Wrap at 0xffff
			ret = (psxCounters[index].count + (cycle / psxCounters[index].rate)) << BIAS & 0xffff;
			if (Config.RCntFix) { // Vandal Hearts 1/2
				ret/= 16;
			}
		}
//		return (psxCounters[index].count + ((cycle - psxCounters[index].sCycle) / psxCounters[index].rate)) << BIAS & 0xffff;
//	} else return 0;

//	SysPrintf("readCcount[%ld] = %lx (mode %lx, target %lx, cycle %lx)\n", index, ret, psxCounters[index].mode, psxCounters[index].target, cycle);

	return ret;
}

static void rcntSwap()
{
	int i;
	
	for (i=0; i<5; i++) {
		psxCounters[i].count = SWAP32p((void *)&psxCounters[i].count);
		psxCounters[i].mode = SWAP32p((void *)&psxCounters[i].mode);
		psxCounters[i].target = SWAP32p((void *)&psxCounters[i].target);
		psxCounters[i].sCycle = SWAP32p((void *)&psxCounters[i].sCycle);
		psxCounters[i].Cycle = SWAP32p((void *)&psxCounters[i].Cycle);
		psxCounters[i].rate = SWAP32p((void *)&psxCounters[i].rate);
		psxCounters[i].interrupt = SWAP32p((void *)&psxCounters[i].interrupt);
	}
}

int psxRcntFreeze(gzFile f, int Mode) {
	char Unused[4096 - sizeof(psxCounter)];
	
	rcntSwap();
	
	gzfreezel(psxCounters);
	gzfreezel(Unused);

	rcntSwap();
	
	return 0;
}
