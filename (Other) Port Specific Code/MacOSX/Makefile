#
# PCSX Makefile for MacOSX
#

MAJ = 1
MIN = 5
VERSION = ${MAJ}.${MIN}

CC = gcc
STRIP = strip
RM = rm

# Set to TRUE to compile with the old gtk1 gui
DISABLE_GTK2 = TRUE

all: pcsx

CPU = powerpc

OPTIMIZE = -O2 -fomit-frame-pointer -finline-functions -ffast-math
FLAGS = -I/sw/include -IRPC -I. -DPCSX_VERSION=\"${VERSION}\" -D__MACOSX__
#FLAGS+= -DENABLE_NLS

# this includes the option -rdynamic and we don't want that
ifeq (${DISABLE_GTK2}, FALSE)
	LIBST = $(shell pkg-config gtk+-2.0 --libs)
	LIBS = $(subst -rdynamic,,${LIBST}) -lz
else
	LIBST = $(shell gtk-config --libs)
	LIBS = $(subst -rdynamic,,${LIBST}) -lz
endif

OBJS = ../PsxBios.o ../CdRom.o ../PsxCounters.o ../PsxDma.o ../DisR3000A.o \
       ../Spu.o ../Sio.o ../PsxHw.o ../Mdec.o ../PsxMem.o ../Misc.o \
       ../plugins.o ../Decode_XA.o ../R3000A.o ../PsxInterpreter.o \
       ../Gte.o ../PsxHLE.o
OBJS+= ../Linux/LnxMain.o ../Linux/Plugin.o ../Linux/Config.o

ifeq (${DISABLE_GTK2}, FALSE)
	OBJS+= ../Linux/Gtk2Gui.o ../Linux/Glade2Gui.o ../Linux/Glade2Funcs.o
else
	OBJS+= ../Linux/GtkGui.o ../Linux/GladeGui.o ../Linux/GladeFuncs.o
endif

CFLAGS = -g -Wall ${OPTIMIZE} -I. -I.. ${FLAGS}
ifeq (${DISABLE_GTK2}, FALSE)
	CFLAGS+= $(shell pkg-config gtk+-2.0 --cflags)
else
	CFLAGS+= $(shell gtk-config --cflags)
endif

pcsx: ${OBJS}
	${CC} ${CFLAGS} ${OBJS} -o pcsx ${LIBS}
	${STRIP} pcsx

.PHONY: clean pcsx pofile

clean:
	${RM} -f *.o ../*.o ../${CPU}/*.o pcsx

../%.o: ../%.c
	${CC} ${CFLAGS} -c -o $@ $<

%.o: %.c
	${CC} ${CFLAGS} -c -o $@ $<

../${CPU}/%.o: ../${CPU}/%.c
	${CC} ${CFLAGS} -c -o $@ $<

../ix86/iR3000A.o: ../ix86/iR3000A.c ../ix86/iGte.h
