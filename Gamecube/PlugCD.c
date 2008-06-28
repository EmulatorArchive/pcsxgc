/* 	super basic CD plugin for PCSX Gamecube
	by emu_kidid based on the DC port
	
	TODO: Fix Missing CDDA support(?)
*/
#include <gccore.h>
#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdarg.h>
#include <errno.h>
#include <string.h>
#include <gccore.h>
#include "plugins.h"
#include "PlugCD.h"
#include "PsxCommon.h"
// gets track 
long getTN(unsigned char* buffer)
{
	int numtracks = getNumTracks();

	//SysPrintf("start getTn()\r\n");

 	if (-1 == numtracks)
 	{
		buffer[0]=buffer[1]=1;
    	return -1;
 	}

 	buffer[0]=CD.tl[0].num;
 	buffer[1]=numtracks;

	//printf("getnumtracks %d %d\n", (int)buffer[0], (int)buffer[1]);
	//SysPrintf("end getTn()\r\n");
   return 0;
}

// if track==0 -> return total length of cd
// otherwise return start in bcd time format
long getTD(int track, unsigned char* buffer)
{
	// SysPrintf("start getTD()\r\n");

	if (track > CD.numtracks)
	{
	// printf("getTD bad %2d\n", track);
		return -1;
	}

   if (track == 0)
   {
      buffer[0] = CD.tl[CD.numtracks-1].end[0];
      buffer[1] = CD.tl[CD.numtracks-1].end[1];
      buffer[2] = CD.tl[CD.numtracks-1].end[2];
   }
	else
	{
		buffer[0] = CD.tl[track-1].start[0];
		buffer[1] = CD.tl[track-1].start[1];
		buffer[2] = CD.tl[track-1].start[2];
	}
	// printf("getTD %2d %02d:%02d:%02d\n", track, (int)buffer[0],
	//       (int)buffer[1], (int)buffer[2]);

	// bcd encode it
	buffer[0] = intToBCD(buffer[0]);
	buffer[1] = intToBCD(buffer[1]);
	buffer[2] = intToBCD(buffer[2]);
	// SysPrintf("end getTD()\r\n");
	return 0;
}

// opens a binary cd image and calculates its length
void openBin(const char* filename)
{
	long end, size, blocks;
	CD.cd = fopen(filename, "rb");
		
	if (CD.cd == 0)
	{
		SysPrintf("Error opening cd\r\n");
		return;
	}
	
	end = fseek(CD.cd, 0, SEEK_END);
	end = ftell(CD.cd);
	size = end;
	SysPrintf("size of CD in MB = %d\r\n",size/1048576);
	
	rc = fseek(CD.cd, 0, SEEK_SET);
	blocks = size / 2352;
	
	// put the track length info in the track list
	CD.tl = (Track*) malloc(sizeof(Track));
	
	CD.tl[0].type = Mode2;
	CD.tl[0].num = 1;
	CD.tl[0].start[0] = 0;
	CD.tl[0].start[1] = 0;
	CD.tl[0].start[2] = 0;
	CD.tl[0].end[2] = blocks % 75;
	CD.tl[0].end[1] = ((blocks - CD.tl[0].end[2]) / 75) % 60;
	CD.tl[0].end[0] = (((blocks - CD.tl[0].end[2]) / 75) - CD.tl[0].end[1]) / 60;
	
	CD.tl[0].start[1] += 2;
	CD.tl[0].end[1] += 2;
	
	normalizeTime(CD.tl[0].end);
	
	CD.numtracks = 1;
	
	//CD.bufferSize = 0;
    CD.bufferPos = 0x7FFFFFFF;
  //  CD.status = 0x00;
}

void addBinTrackInfo()
{	
	CD.tl = realloc(CD.tl, (CD.numtracks + 1) * sizeof(Track));
	CD.tl[CD.numtracks].end[0] = CD.tl[0].end[0];
	CD.tl[CD.numtracks].end[1] = CD.tl[0].end[1];
	CD.tl[CD.numtracks].end[2] = CD.tl[0].end[2];
	CD.tl[CD.numtracks].start[0] = CD.tl[0].start[0];
	CD.tl[CD.numtracks].start[1] = CD.tl[0].start[1];
	CD.tl[CD.numtracks].start[2] = CD.tl[0].start[2];

}

// new file types should be added here and in the CDOpen function
void newCD(const char * filename)
{
	CD.type = Bin;
	openBin(filename);
	addBinTrackInfo();
	
	CD.bufferPos = 0x7FFFFFFF;
	seekSector(0,2,0);
}


// return the sector address - the buffer address + 12 bytes for offset.
unsigned char* getSector(int subchannel)
{
	return CD.buffer + (CD.sector - CD.bufferPos) + ((subchannel) ? 0 : 12);
}

// returns the number of tracks
char getNumTracks()
{
	if (CD.cd == 0) {
	    return -1;
	}
	return CD.numtracks;
}

void readit(const unsigned char m, const unsigned char s, const unsigned char f)
{
	
	// fakie ISO support.  iso is just cd-xa data without the ecc and header.
	// read in the same number of sectors then space it out to look like cd-xa
	/* if (CD.type == Iso)
	{
		unsigned char temptime[3];
		long tempsector = (( (m * 60) + (s - 2)) * 75 + f) * 2048;
		fs_seek(CD.cd, tempsector, SEEK_SET);
		fs_read(CD.buffer, sizeof(unsigned char), 2048*BUFFER_SECTORS, CD.cd);
		
		// spacing out the data here...
		for(tempsector = BUFFER_SECTORS - 1; tempsector >= 0; tempsector--)
		{
			memcpy(&CD.buffer[tempsector*2352 + 24],&CD.buffer[tempsector*2048], 2048);
			
			// two things - add the m/s/f flags in case anyone is looking
			// and change the xa mode to 1
			temptime[0] = m;
			temptime[1] = s;
			temptime[2] = f + (unsigned char)tempsector;
			
			normalizeTime(temptime);
			CD.buffer[tempsector*2352+12] = intToBCD(temptime[0]);
			CD.buffer[tempsector*2352+12+1] = intToBCD(temptime[1]);
			CD.buffer[tempsector*2352+12+2] = intToBCD(temptime[2]);
			CD.buffer[tempsector*2352+12+3] = 0x01;
		}
	}
	else */
	{
		rc = fseek(CD.cd, CD.sector, SEEK_SET);
		rc = fread(CD.buffer, BUFFER_SIZE, 1, CD.cd);
	}
	
	CD.bufferPos = CD.sector;
}


void seekSector(const unsigned char m, const unsigned char s, const unsigned char f)
{
	// calc byte to search for
	int sector = (( (m * 60) + (s - 2)) * 75 + f);
	CD.sector = sector * 2352;
	
	// is it cached?
	if ((CD.sector >= CD.bufferPos) && (CD.sector < (CD.bufferPos + BUFFER_SIZE)) ) 
	{
	    return;
	}
	// not cached - read a few blocks into the cache
	else
	{
		readit(m,s,f);
	}
}

long CDR__open(void)
{
	char str[256];
	
	SysPrintf("start CDR_open()\r\n");
	
	// newCD("/cd/cd.bin");
	strcpy(str, CDConfiguration.dn);
	strcat(str, "/");
	strcat(str, CDConfiguration.fn);
	
	newCD(str);
	
	SysPrintf("end CDR_open()\r\n");
	return 0;
}

long CDR__init(void) {
	SysPrintf("start CDR_init()\r\n");
	strcpy(CDConfiguration.dn, "PSXISOS");
	strcpy(CDConfiguration.fn, "GAME.ISO");
	SysPrintf("end CDR_init()\r\n");
	return 0;
}

long CDR__shutdown(void) {
	return 0;
}

long CDR__close(void) {
	SysPrintf("start CDR_close()\r\n");
	fclose(CD.cd);
	free(CD.tl);
	SysPrintf("end CDR_close()\r\n");
	return 0;
}

long CDR__getTN(unsigned char *buffer) {
    return getTN(buffer);
}

long CDR__getTD(unsigned char track, unsigned char *buffer) {
	
	unsigned char temp[3];
	int result = getTD((int)track, temp);
	
	if (result == -1) return -1;
	
	buffer[1] = temp[1];
	buffer[2] = temp[0];
	return 0;
}

/* called when the psx requests a read */
long CDR__readTrack(unsigned char *time) {
	if (CD.cd != 0)
		seekSector(BCDToInt(time[0]), BCDToInt(time[1]), BCDToInt(time[2]));
	return PSE_CDR_ERR_SUCCESS;
}

/* called after the read should be finished, and the data is needed */
unsigned char *CDR__getBuffer(void) {
	if (CD.cd == 0)
		return NULL;
    return getSector(0);
}

unsigned char *CDR__getBufferSub(void) {
    return getSector(1);
}

/*
long (CALLBACK* CDRconfigure)(void);
long (CALLBACK* CDRtest)(void);
void (CALLBACK* CDRabout)(void);
long (CALLBACK* CDRplay)(unsigned char *);
long (CALLBACK* CDRstop)(void);
*/
