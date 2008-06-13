/* 	super basic PAD plugin for PCSX Gamecube
	by emu_kidid based on the DC port
	
	TODO: Re-write for proper PADs, Trigger/Analog support
*/

#include <gccore.h>
#include <stdint.h>
#include <sys/types.h>
#include <ogc/pad.h>
#include "plugins.h"

long  PadFlags = 0;

long PAD__init(long flags) {
	SysPrintf("start PAD_init()\r\n");

//	printf("Flags: %08x\n", flags);
	PadFlags |= flags;

	/* Read Configuration here */

	SysPrintf("end PAD_init()\r\n");
	
	return 0;
}

long PAD__shutdown(void) {
	return 0;
}

long PAD__open(void)
{
//	SysPrintf("start PAD_open()\r\n");

//	SysPrintf("end PAD_open()\r\n");
	return 0;
}

long PAD__close(void) {
	return 0;
}

long PAD__readPort1(PadDataS* pad) {
//	SysPrintf("start PAD1_readPort()\r\n");
	int b = PAD_ButtonsHeld(0);
	uint16_t pad_status = 0xffff;	//bit pointless why is this done this way? :\
	 
	if (!(b & PAD_BUTTON_START))
		pad_status &= ~(1<<3);
	if (!(b & PAD_BUTTON_A))
		pad_status &= ~(1<<14);
	if (!(b & PAD_BUTTON_B))
		pad_status &= ~(1<<13);
	if (!(b & PAD_BUTTON_X))
		pad_status &= ~(1<<15);
	if (!(b & PAD_BUTTON_Y))
		pad_status &= ~(1<<12);
	if (!(b & PAD_BUTTON_UP))
		pad_status &= ~(1<<4);
	if (!(b & PAD_BUTTON_DOWN))
		pad_status &= ~(1<<6);
	if (!(b & PAD_BUTTON_LEFT))
		pad_status &= ~(1<<7);
	if (!(b & PAD_BUTTON_RIGHT))
		pad_status &= ~(1<<5);
			
	pad->buttonStatus = pad_status;
	pad->controllerType = 4; // standard
//	printf("Pad1 Status: %04x\n", pad_status);
//	SysPrintf("end PAD1_readPort()\r\n");
	return 0;
}

long PAD__readPort2(PadDataS* pad) {
	int b = PAD_ButtonsHeld(1);
	uint16_t pad_status = 0xffff;	//bit pointless why is this done this way? :\
	 
	if (!(b & PAD_BUTTON_START))
		pad_status &= ~(1<<3);
	if (!(b & PAD_BUTTON_A))
		pad_status &= ~(1<<14);
	if (!(b & PAD_BUTTON_B))
		pad_status &= ~(1<<13);
	if (!(b & PAD_BUTTON_X))
		pad_status &= ~(1<<15);
	if (!(b & PAD_BUTTON_Y))
		pad_status &= ~(1<<12);
	if (!(b & PAD_BUTTON_UP))
		pad_status &= ~(1<<4);
	if (!(b & PAD_BUTTON_DOWN))
		pad_status &= ~(1<<6);
	if (!(b & PAD_BUTTON_LEFT))
		pad_status &= ~(1<<7);
	if (!(b & PAD_BUTTON_RIGHT))
		pad_status &= ~(1<<5);
			
	pad->buttonStatus = pad_status;
	pad->controllerType = 4; // standard
//	printf("Pad1 Status: %04x\n", pad_status);
//	SysPrintf("end PAD1_readPort()\r\n");
	return 0;
}
