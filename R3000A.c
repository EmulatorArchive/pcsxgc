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

R3000Acpu *psxCpu;
psxRegisters psxRegs;

int psxInit() {

	psxCpu = &psxInt;
#if defined(__i386__) || defined(__sh__) || defined(__ppc__)
	if (!Config.Cpu) psxCpu = &psxRec;
#endif
	Log=0; // GP

	if (psxMemInit() == -1) {
		SysMessage("Failed to allocate memory for emulation\n Please restart the emulator and try again.");
		return -1;
	}
	
	return psxCpu->Init();
}

void psxReset() {
	psxCpu->Reset();

	psxMemReset();

	memset(&psxRegs, 0, sizeof(psxRegs));

	psxRegs.pc = 0xbfc00000; // Start in bootstrap

	psxRegs.CP0.r[12] = 0x10900000; // COP0 enabled | BEV = 1 | TS = 1
	psxRegs.CP0.r[15] = 0x00000002; // PRevID = Revision ID, same as R3000A
	
	psxHwReset();
	psxBiosInit();
	
	TimedInterruptsReset(0);
	
	if (!Config.HLE) psxExecuteBios();

#ifdef EMU_LOG
	EMU_LOG("*BIOS END*\n");
#endif
	Log=0; // GP
}

void psxShutdown() {
	psxMemShutdown();
	psxBiosShutdown();

	psxCpu->Shutdown();
}

void psxException(u32 code, u32 bd) {
	// Set the Cause
	psxRegs.CP0.n.Cause = code;

	// Set the EPC & PC
	if (bd) {
#ifdef PSXCPU_LOG
		PSXCPU_LOG("bd set!!!\n");
#endif
		SysPrintf("bd set!!!\n");
		psxRegs.CP0.n.Cause|= 0x80000000;
		psxRegs.CP0.n.EPC = (psxRegs.pc - 4);
	} else
		psxRegs.CP0.n.EPC = (psxRegs.pc);

	if (psxRegs.CP0.n.Status & 0x400000)
		psxRegs.pc = 0xbfc00180;
	else
		psxRegs.pc = 0x80000080;

	// Set the Status
	psxRegs.CP0.n.Status = (psxRegs.CP0.n.Status &~0x3f) |
						  ((psxRegs.CP0.n.Status & 0xf) << 2);
	psxExceptionTest();
	
	if (!Config.HLE && (((PSXMu32(psxRegs.CP0.n.EPC) >> 24) & 0xfe) == 0x4a)) {
		// "hokuto no ken" / "Crash Bandicot 2" ... fix
		PSXMu32ref(psxRegs.CP0.n.EPC)&= SWAPu32(~0x02000000);
	}

	if (Config.HLE) psxBiosException();
}

void psxBranchTest() {
#if 1
	//psxExceptionTest();
	
	if (psxRegs.cycle <= 0) {
		TimedInterruptHandle();
	}

	/*if (psxHu32ref(0x1070) & psxHu32ref(0x1074)) {
		if ((psxRegs.CP0.n.Status & 0x401) == 0x401) {
#ifdef PSXCPU_LOG
			//PSXCPU_LOG("Interrupt: %x %x\n", psxHu32(0x1070), psxHu32(0x1074));
#endif
//			SysPrintf("Interrupt (%x): %x %x\n", psxCurrentCycle, psxHu32(0x1070), psxHu32(0x1074));
			psxException(0x400, 0);
		}
	}*/
#else
	// specifically tell the compiler that cycle and interrupt doesn't change in subcalls
	int cycle = psxRegs.cycle;
	int interrupt = psxRegs.interrupt;
	
	if (cycle <= 0) {
		psxRcntUpdate();
		cycle = psxRegs.cycle;
	}
	
	if (interrupt) {
#ifdef PSXCPU_LOG
//                PSXCPU_LOG("Timed Interrupt: %x\n", psxRegs.interrupt);
#endif
		cycle = psxNextCounter - cycle;
		
		if (interrupt & 0x80) { // sio
			if (!Config.Sio) {
				if ((cycle - psxRegs.intCycle[7]) >= psxRegs.intCycle[7+1]) {
					psxRegs.interrupt&=~0x80;
					sioInterrupt();
				}
			}
		}
		if (interrupt & 0x04) { // cdr
			if ((cycle - psxRegs.intCycle[2]) >= psxRegs.intCycle[2+1]) {
				psxRegs.interrupt&=~0x04;
				cdrInterrupt();
			}
		}
		if (interrupt & 0x03040000) {
			if (psxRegs.interrupt & 0x040000) { // cdr read
				if ((cycle - psxRegs.intCycle[2+16]) >= psxRegs.intCycle[2+16+1]) {
					psxRegs.interrupt&=~0x040000;
					cdrReadInterrupt();
				}
			}
			if (interrupt & 0x01000000) { // gpu dma
				if ((cycle - psxRegs.intCycle[3+24]) >= psxRegs.intCycle[3+24+1]) {
					psxRegs.interrupt&=~0x01000000;
					gpuInterrupt();
				}
			}
			if (interrupt & 0x02000000) { // mdec out dma
				if ((cycle - psxRegs.intCycle[5+24]) >= psxRegs.intCycle[5+24+1]) {
					psxRegs.interrupt&=~0x02000000;
					mdec1Interrupt();
				}
			}
		}
	}

	if (psxHu32ref(0x1070) & psxHu32ref(0x1074)) {
		if ((psxRegs.CP0.n.Status & 0x401) == 0x401) {
#ifdef PSXCPU_LOG
			//PSXCPU_LOG("Interrupt: %x %x\n", psxHu32(0x1070), psxHu32(0x1074));
#endif
//			SysPrintf("Interrupt (%x): %x %x\n", psxCurrentCycle, psxHu32(0x1070), psxHu32(0x1074));
			psxException(0x400, 0);
		}
	}

#if 0
	if (!Config.HLE && Config.PsxOut) {
		u32 call = psxRegs.GPR.n.t1 & 0xff;
		switch (psxRegs.pc & 0x1fffff) {
			case 0xa0:
#ifdef PSXBIOS_LOG
				if (call != 0x28 && call != 0xe) {
					PSXBIOS_LOG("Bios call a0: %s (%x) %x,%x,%x,%x\n", biosA0n[call], call, psxRegs.GPR.n.a0, psxRegs.GPR.n.a1, psxRegs.GPR.n.a2, psxRegs.GPR.n.a3); }
#endif
				if (biosA0[call])
					biosA0[call]();
				break;
			case 0xb0:
#ifdef PSXBIOS_LOG
				if (call != 0x17 && call != 0xb) {
					PSXBIOS_LOG("Bios call b0: %s (%x) %x,%x,%x,%x\n", biosB0n[call], call, psxRegs.GPR.n.a0, psxRegs.GPR.n.a1, psxRegs.GPR.n.a2, psxRegs.GPR.n.a3); }
#endif
				if (biosB0[call])
					biosB0[call]();
				break;
			case 0xc0:
#ifdef PSXBIOS_LOG
				PSXBIOS_LOG("Bios call c0: %s (%x) %x,%x,%x,%x\n", biosC0n[call], call, psxRegs.GPR.n.a0, psxRegs.GPR.n.a1, psxRegs.GPR.n.a2, psxRegs.GPR.n.a3);
#endif
				if (biosC0[call])
					biosC0[call]();
				break;
		}
	}
#endif
#endif
}

void psxExecuteBios() {
	while (psxRegs.pc != 0x80030000)
		psxCpu->ExecuteBlock();
}

