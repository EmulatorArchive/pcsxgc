/*  Pcsx - Pc Psx Emulator
 *  Copyright (C) 1999-2002  Pcsx Team
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

#ifndef GAMECUBE_PLUGINS_H
#define GAMECUBE_PLUGINS_H

#ifndef NULL
#define NULL ((void*)0)
#endif
#include "Decode_XA.h"
#include "PSEmu_Plugin_Defs.h"

#define SYMS_PER_LIB 32
typedef struct {
	char* lib;
	int   numSyms;
	struct {
		char* sym;
		void* pntr;
	} syms[SYMS_PER_LIB];
} PluginTable;
#define NUM_PLUGINS 8

/* PAD */
typedef long (* PADopen)(unsigned long *);
extern long PAD__init(long);
extern long PAD__shutdown(void);	
extern long PAD__open(void);
extern long PAD__close(void);
extern long PAD__readPort1(PadDataS*);
extern long PAD__readPort2(PadDataS*);

/* SPU */
typedef long (* SPUopen)(void);
long SPU_d_init(void);				
long SPU_d_shutdown(void);	
long SPU_d_close(void);			
long SPU_d_open(void);			
void SPU_d_playSample(unsigned char);		
void SPU_d_startChannels1(unsigned short);	
void SPU_d_startChannels2(unsigned short);
void SPU_d_stopChannels1(unsigned short);	
void SPU_d_stopChannels2(unsigned short);	
void SPU_d_putOne(unsigned long,unsigned short);			
unsigned short SPU_d_getOne(unsigned long);			
void SPU_d_setAddr(unsigned char, unsigned short);			
void SPU_d_setPitch(unsigned char, unsigned short);		
void SPU_d_setVolumeL(unsigned char, short );		
void SPU_d_setVolumeR(unsigned char, short );		
long SPU_d_test(void);
long SPU_d_configure(void);
void SPU_d_about(void);
void SPU_d_playADPCMchannel(xa_decode_t *xap);
void SPU_d_writeDMA(unsigned short val);
unsigned short SPU_d_readDMA(void);
unsigned short SPU_d_readRegister(unsigned long reg);
void SPU_d_writeRegister(unsigned long reg, unsigned short val);

/* CDR */
long CDR__open(void);
long CDR__init(void);
long CDR__shutdown(void);
long CDR__open(void);
long CDR__close(void);
long CDR__getTN(unsigned char *);
long CDR__getTD(unsigned char , unsigned char *);
long CDR__readTrack(unsigned char *);
unsigned char *CDR__getBuffer(void);

#define EMPTY_PLUGIN \
	{ NULL,      \
	  0,         \
	  { { NULL,  \
	      NULL }, } }
	      
#define PAD1_PLUGIN \
	{ "PAD1",      \
	  5,         \
	  { { "PADinit",  \
	      PAD__init }, \
	    { "PADshutdown",	\
	      PAD__shutdown}, \
	    { "PADopen", \
	      PAD__open}, \
	    { "PADclose", \
	      PAD__close}, \
	    { "PADreadPort1", \
	      PAD__readPort1} \
	       } } 
	    
#define PAD2_PLUGIN \
	{ "PAD2",      \
	  5,         \
	  { { "PADinit",  \
	      PAD__init }, \
	    { "PADshutdown",	\
	      PAD__shutdown}, \
	    { "PADopen", \
	      PAD__open}, \
	    { "PADclose", \
	      PAD__close}, \
	    { "PADreadPort2", \
	      PAD__readPort2} \
	       } }

#define CDR_PLUGIN \
	{ "CDR",      \
	  8,         \
	  { { "CDRinit",  \
	      CDR__init }, \
	    { "CDRshutdown",	\
	      CDR__shutdown}, \
	    { "CDRopen", \
	      CDR__open}, \
	    { "CDRclose", \
	      CDR__close}, \
	    { "CDRgetTN", \
	      CDR__getTN}, \
	    { "CDRgetTD", \
	      CDR__getTD}, \
	    { "CDRreadTrack", \
	      CDR__readTrack}, \
	    { "CDRgetBuffer", \
	      CDR__getBuffer} \
	       } }
		

void SPU_d_playSample(unsigned char);		

#define SPU_PLUGIN \
	{ "SPU",      \
	  27,         \
	  { { "SPUinit",  \
	      SPU_d_init }, \
	    { "SPUshutdown",	\
	      SPU_d_shutdown}, \
	    { "SPUopen", \
	      SPU_d_open}, \
	    { "SPUclose", \
	      SPU_d_close}, \
	    { "SPUconfigure", \
	      SPU_d_configure}, \
	    { "SPUabout", \
	      SPU_d_about}, \
	    { "SPUtest", \
	      SPU_d_test}, \
	    { "SPUstartChannels1", \
	      SPU_d_startChannels1}, \
	    { "SPUstartChannels2", \
	      SPU_d_startChannels2}, \
	    { "SPUstopChannels1", \
	      SPU_d_stopChannels1}, \
	    { "SPUstopChannels2", \
	      SPU_d_stopChannels2}, \
	    { "SPUputOne", \
	      SPU_d_putOne}, \
	    { "SPUgetOne", \
	      SPU_d_getOne}, \
	    { "SPUsetAddr", \
	      SPU_d_setAddr}, \
	    { "SPUsetPitch", \
	      SPU_d_setPitch}, \
	    { "SPUsetVolumeL", \
	      SPU_d_setVolumeL}, \
	    { "SPUsetVolumeR", \
	      SPU_d_setVolumeR}, \
	    { "SPUwriteRegister", \
	      SPU_d_writeRegister}, \
	    { "SPUreadRegister", \
	      SPU_d_readRegister}, \
	    { "SPUwriteDMA", \
	      SPU_d_writeDMA}, \
	    { "SPUreadDMA", \
	      SPU_d_readDMA}, \
	    { "SPUwriteDMAMem", \
	      SPU_d_writeDMA}, \
	    { "SPUreadDMAMem", \
	      SPU_d_readDMA}, \
	    { "SPUplayADPCMchannel", \
	      SPU_d_playADPCMchannel}, \
	    { "SPUfreeze", \
	      SPU_d_close}, \
	    { "SPUregisterCallback", \
	      SPU_d_close}, \
	    { "SPUasync", \
	      SPU_d_close} \
	       } }
       
	       
#define PLUGIN_SLOT_0 EMPTY_PLUGIN
#define PLUGIN_SLOT_1 PAD1_PLUGIN
#define PLUGIN_SLOT_2 PAD2_PLUGIN
#define PLUGIN_SLOT_3 CDR_PLUGIN
#define PLUGIN_SLOT_4 SPU_PLUGIN
#define PLUGIN_SLOT_5 EMPTY_PLUGIN
#define PLUGIN_SLOT_6 EMPTY_PLUGIN
#define PLUGIN_SLOT_7 EMPTY_PLUGIN



#endif

