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

#ifndef __PLUGINS_H__
#define __PLUGINS_H__

typedef struct {
	unsigned long ulFreezeVersion;
	unsigned long ulStatus;
	unsigned long ulControl[256];
	unsigned char psxVRam[1024*512*2];
} GPUFreeze_t;

#if defined (__WIN32__)
#include "Win32\plugin.h"
#elif defined(__LINUX__) || defined(__MACOSX__)
typedef void* HWND;
#define CALLBACK
#include "Linux/Plugin.h"
#elif defined(__DREAMCAST__)
typedef void* HWND;
#define CALLBACK
#include "Dreamcast/Plugin.h"
#endif
#if defined(__GAMECUBE__)
typedef void* HWND;
#define CALLBACK
#include "Gamecube/Plugin.h"
#include "Gamecube/GamecubePlugins.h"
#endif
#ifndef EXT
#define EXT extern
#endif

#include "PSEmu_Plugin_Defs.h"
#include "Decode_XA.h"

int  LoadPlugins();
void ReleasePlugins();
int  OpenPlugins();
void ClosePlugins();
void ResetPlugins();


typedef unsigned long (CALLBACK* PSEgetLibType)(void);
typedef unsigned long (CALLBACK* PSEgetLibVersion)(void);
typedef char *(CALLBACK* PSEgetLibName)(void);

///GPU PLUGIN STUFF 
typedef long (CALLBACK* GPUinit)(void);
typedef long (CALLBACK* GPUshutdown)(void);
typedef long (CALLBACK* GPUclose)(void);
typedef void (CALLBACK* GPUwriteStatus)(unsigned long);
typedef void (CALLBACK* GPUwriteData)(unsigned long);
typedef void (CALLBACK* GPUwriteDataMem)(unsigned long *, int);
typedef unsigned long (CALLBACK* GPUreadStatus)(void);
typedef unsigned long (CALLBACK* GPUreadData)(void);
typedef void (CALLBACK* GPUreadDataMem)(unsigned long *, int);
typedef long (CALLBACK* GPUdmaChain)(unsigned long *,unsigned long);
typedef void (CALLBACK* GPUupdateLace)(void);
typedef long (CALLBACK* GPUconfigure)(void);
typedef long (CALLBACK* GPUtest)(void);
typedef void (CALLBACK* GPUabout)(void);
typedef void (CALLBACK* GPUmakeSnapshot)(void);
typedef void (CALLBACK* GPUkeypressed)(int);
typedef void (CALLBACK* GPUdisplayText)(char *);
typedef long (CALLBACK* GPUfreeze)(unsigned long, GPUFreeze_t *);
typedef long (CALLBACK* GPUgetScreenPic)(unsigned char *);
typedef long (CALLBACK* GPUshowScreenPic)(unsigned char *);
typedef void (CALLBACK* GPUclearDynarec)(void (CALLBACK *callback)(void));

//plugin stuff From Shadow
// *** walking in the valley of your darking soul i realize that i was alone
//Gpu function pointers
EXT GPUupdateLace    GPU_updateLace;
EXT GPUinit          GPU_init;
EXT GPUshutdown      GPU_shutdown; 
EXT GPUconfigure     GPU_configure;
EXT GPUtest          GPU_test;
EXT GPUabout         GPU_about;
EXT GPUopen          GPU_open;
EXT GPUclose         GPU_close;
EXT GPUreadStatus    GPU_readStatus;
EXT GPUreadData      GPU_readData;
EXT GPUreadDataMem   GPU_readDataMem;
EXT GPUwriteStatus   GPU_writeStatus; 
EXT GPUwriteData     GPU_writeData;
EXT GPUwriteDataMem  GPU_writeDataMem;
EXT GPUdmaChain      GPU_dmaChain;
EXT GPUkeypressed    GPU_keypressed;
EXT GPUdisplayText   GPU_displayText;
EXT GPUmakeSnapshot  GPU_makeSnapshot;
EXT GPUfreeze        GPU_freeze;
EXT GPUgetScreenPic  GPU_getScreenPic;
EXT GPUshowScreenPic GPU_showScreenPic;
EXT GPUclearDynarec  GPU_clearDynarec;

//cd rom plugin ;)
typedef long (CALLBACK* CDRinit)(void);
typedef long (CALLBACK* CDRshutdown)(void);
typedef long (CALLBACK* CDRopen)(void);
typedef long (CALLBACK* CDRclose)(void);
typedef long (CALLBACK* CDRgetTN)(unsigned char *);
typedef long (CALLBACK* CDRgetTD)(unsigned char , unsigned char *);
typedef long (CALLBACK* CDRreadTrack)(unsigned char *);
typedef unsigned char * (CALLBACK* CDRgetBuffer)(void);
typedef long (CALLBACK* CDRconfigure)(void);
typedef long (CALLBACK* CDRtest)(void);
typedef void (CALLBACK* CDRabout)(void);
typedef long (CALLBACK* CDRplay)(unsigned char *);
typedef long (CALLBACK* CDRstop)(void);
struct CdrStat {
	unsigned long Type;
	unsigned long Status;
	unsigned char Time[3];
};
typedef long (CALLBACK* CDRgetStatus)(struct CdrStat *);
typedef char* (CALLBACK* CDRgetDriveLetter)(void);
struct SubQ {
	char res0[11];
	unsigned char ControlAndADR;
	unsigned char TrackNumber;
	unsigned char IndexNumber;
	unsigned char TrackRelativeAddress[3];
	unsigned char Filler;
	unsigned char AbsoluteAddress[3];
	char res1[72];
};
typedef unsigned char* (CALLBACK* CDRgetBufferSub)(void);

//cd rom function pointers 
EXT CDRinit               CDR_init;
EXT CDRshutdown           CDR_shutdown;
EXT CDRopen               CDR_open;
EXT CDRclose              CDR_close; 
EXT CDRtest               CDR_test;
EXT CDRgetTN              CDR_getTN;
EXT CDRgetTD              CDR_getTD;
EXT CDRreadTrack          CDR_readTrack;
EXT CDRgetBuffer          CDR_getBuffer;
EXT CDRplay               CDR_play;
EXT CDRstop               CDR_stop;
EXT CDRgetStatus          CDR_getStatus;
EXT CDRgetDriveLetter     CDR_getDriveLetter;
EXT CDRgetBufferSub       CDR_getBufferSub;
EXT CDRconfigure          CDR_configure;
EXT CDRabout              CDR_about;

// spu plugin
typedef long (CALLBACK* SPUinit)(void);				
typedef long (CALLBACK* SPUshutdown)(void);	
typedef long (CALLBACK* SPUclose)(void);			
typedef void (CALLBACK* SPUplaySample)(unsigned char);		
typedef void (CALLBACK* SPUstartChannels1)(unsigned short);	
typedef void (CALLBACK* SPUstartChannels2)(unsigned short);
typedef void (CALLBACK* SPUstopChannels1)(unsigned short);	
typedef void (CALLBACK* SPUstopChannels2)(unsigned short);	
typedef void (CALLBACK* SPUputOne)(unsigned long,unsigned short);			
typedef unsigned short (CALLBACK* SPUgetOne)(unsigned long);			
typedef void (CALLBACK* SPUsetAddr)(unsigned char, unsigned short);			
typedef void (CALLBACK* SPUsetPitch)(unsigned char, unsigned short);		
typedef void (CALLBACK* SPUsetVolumeL)(unsigned char, short );		
typedef void (CALLBACK* SPUsetVolumeR)(unsigned char, short );		
//psemu pro 2 functions from now..
typedef void (CALLBACK* SPUwriteRegister)(unsigned long, unsigned short);	
typedef unsigned short (CALLBACK* SPUreadRegister)(unsigned long);		
typedef void (CALLBACK* SPUwriteDMA)(unsigned short);
typedef unsigned short (CALLBACK* SPUreadDMA)(void);
typedef void (CALLBACK* SPUwriteDMAMem)(unsigned short *, int);
typedef void (CALLBACK* SPUreadDMAMem)(unsigned short *, int);
typedef void (CALLBACK* SPUplayADPCMchannel)(xa_decode_t *);
typedef void (CALLBACK* SPUregisterCallback)(void (CALLBACK *callback)(void));
typedef long (CALLBACK* SPUconfigure)(void);
typedef long (CALLBACK* SPUtest)(void);			
typedef void (CALLBACK* SPUabout)(void);
typedef struct {
	unsigned char PluginName[8];
	unsigned long PluginVersion;
	unsigned long Size;
	unsigned char SPUPorts[0x200];
	unsigned char SPURam[0x80000];
	xa_decode_t xa;
	unsigned char *SPUInfo;
} SPUFreeze_t;
typedef long (CALLBACK* SPUfreeze)(unsigned long, SPUFreeze_t *);
typedef void (CALLBACK* SPUasync)(unsigned long);

//SPU POINTERS
EXT SPUconfigure        SPU_configure;
EXT SPUabout            SPU_about;
EXT SPUinit             SPU_init;
EXT SPUshutdown         SPU_shutdown;
EXT SPUtest             SPU_test;
EXT SPUopen             SPU_open;
EXT SPUclose            SPU_close;
EXT SPUplaySample       SPU_playSample;
EXT SPUstartChannels1   SPU_startChannels1;
EXT SPUstartChannels2   SPU_startChannels2;
EXT SPUstopChannels1    SPU_stopChannels1;
EXT SPUstopChannels2    SPU_stopChannels2;
EXT SPUputOne           SPU_putOne;
EXT SPUgetOne           SPU_getOne;
EXT SPUsetAddr          SPU_setAddr;
EXT SPUsetPitch         SPU_setPitch;
EXT SPUsetVolumeL       SPU_setVolumeL;
EXT SPUsetVolumeR       SPU_setVolumeR;
EXT SPUwriteRegister    SPU_writeRegister;
EXT SPUreadRegister     SPU_readRegister;
EXT SPUwriteDMA         SPU_writeDMA;
EXT SPUreadDMA          SPU_readDMA;
EXT SPUwriteDMAMem      SPU_writeDMAMem;
EXT SPUreadDMAMem       SPU_readDMAMem;
EXT SPUplayADPCMchannel SPU_playADPCMchannel;
EXT SPUfreeze           SPU_freeze;
EXT SPUregisterCallback SPU_registerCallback;
EXT SPUasync            SPU_async;

// PAD Functions

typedef long (CALLBACK* PADconfigure)(void);
typedef void (CALLBACK* PADabout)(void);
typedef long (CALLBACK* PADinit)(long);
typedef long (CALLBACK* PADshutdown)(void);	
typedef long (CALLBACK* PADtest)(void);		
typedef long (CALLBACK* PADclose)(void);
typedef long (CALLBACK* PADquery)(void);
typedef long (CALLBACK*	PADreadPort1)(PadDataS*);
typedef long (CALLBACK* PADreadPort2)(PadDataS*);
typedef long (CALLBACK* PADkeypressed)(void);
typedef unsigned char (CALLBACK* PADstartPoll)(int);
typedef unsigned char (CALLBACK* PADpoll)(unsigned char);
typedef void (CALLBACK* PADsetSensitive)(int);

//PAD POINTERS
EXT PADconfigure        PAD1_configure;
EXT PADabout            PAD1_about;
EXT PADinit             PAD1_init;
EXT PADshutdown         PAD1_shutdown;
EXT PADtest             PAD1_test;
EXT PADopen             PAD1_open;
EXT PADclose            PAD1_close;
EXT PADquery			PAD1_query;
EXT PADreadPort1		PAD1_readPort1;
EXT PADkeypressed		PAD1_keypressed;
EXT PADstartPoll        PAD1_startPoll;
EXT PADpoll             PAD1_poll;
EXT PADsetSensitive     PAD1_setSensitive;

EXT PADconfigure        PAD2_configure;
EXT PADabout            PAD2_about;
EXT PADinit             PAD2_init;
EXT PADshutdown         PAD2_shutdown;
EXT PADtest             PAD2_test;
EXT PADopen             PAD2_open;
EXT PADclose            PAD2_close;
EXT PADquery            PAD2_query;
EXT PADreadPort2		PAD2_readPort2;
EXT PADkeypressed		PAD2_keypressed;
EXT PADstartPoll        PAD2_startPoll;
EXT PADpoll             PAD2_poll;
EXT PADsetSensitive     PAD2_setSensitive;

// NET plugin

typedef long (CALLBACK* NETinit)(void);
typedef long (CALLBACK* NETshutdown)(void);
typedef long (CALLBACK* NETclose)(void);
typedef long (CALLBACK* NETconfigure)(void);
typedef long (CALLBACK* NETtest)(void);
typedef void (CALLBACK* NETabout)(void);
typedef void (CALLBACK* NETpause)(void);
typedef void (CALLBACK* NETresume)(void);
typedef long (CALLBACK* NETqueryPlayer)(void);
typedef long (CALLBACK* NETsendData)(void *, int, int);
typedef long (CALLBACK* NETrecvData)(void *, int, int);
typedef long (CALLBACK* NETsendPadData)(void *, int);
typedef long (CALLBACK* NETrecvPadData)(void *, int);

typedef struct {
	char EmuName[32];
	char CdromID[9];	// ie. 'SCPH12345', no \0 trailing character
	char CdromLabel[11];
	void *psxMem;
	GPUshowScreenPic GPU_showScreenPic;
	GPUdisplayText GPU_displayText;
	PADsetSensitive PAD_setSensitive;
	char GPUpath[256];	// paths must be absolute
	char SPUpath[256];
	char CDRpath[256];
	char MCD1path[256];
	char MCD2path[256];
	char BIOSpath[256];	// 'HLE' for internal bios
	char Unused[1024];
} netInfo;

typedef long (CALLBACK* NETsetInfo)(netInfo *);
typedef long (CALLBACK* NETkeypressed)(int)
;


// NET function pointers 
EXT NETinit               NET_init;
EXT NETshutdown           NET_shutdown;
EXT NETopen               NET_open;
EXT NETclose              NET_close; 
EXT NETtest               NET_test;
EXT NETconfigure          NET_configure;
EXT NETabout              NET_about;
EXT NETpause              NET_pause;
EXT NETresume             NET_resume;
EXT NETqueryPlayer        NET_queryPlayer;
EXT NETsendData           NET_sendData;
EXT NETrecvData           NET_recvData;
EXT NETsendPadData        NET_sendPadData;
EXT NETrecvPadData        NET_recvPadData;
EXT NETsetInfo            NET_setInfo;
EXT NETkeypressed         NET_keypressed;

int LoadCDRplugin(char *CDRdll);
int LoadGPUplugin(char *GPUdll);
int LoadSPUplugin(char *SPUdll);
int LoadPAD1plugin(char *PAD1dll);
int LoadPAD2plugin(char *PAD2dll);
int LoadNETplugin(char *NETdll);

void CALLBACK clearDynarec(void);

#endif /* __PLUGINS_H__ */
