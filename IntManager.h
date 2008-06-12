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

#ifndef __INTMANAGER_H__
#define __INTMANAGER_H__

#define MAX_TIMERS	20

typedef void (*int_handler)(void); 

typedef struct {
	int_handler handler;	// handler to call when the time is right
	u32 when;				// timestamp of when this event will trigger
} timer_s;

extern timer_s timed_interrupts[];

void TimedInterruptsReset(u32 time);
void TimedInterruptHandle();
void AddTimedInterrupt(int_handler handler, u32 when);
int RemoveTimedInterrupt(int_handler handler);

void psxIntAdd(unsigned long which);
void psxIntRemove(unsigned long which);
void psxExceptionTest();

#endif /* __INTMANAGER_H__ */
