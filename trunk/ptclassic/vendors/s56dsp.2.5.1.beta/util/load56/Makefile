#
# Makefile for load56
#
# $Id$

S56DSP = /usr/local/s56dsp

CFLAGS = -O2 -I$(S56DSP)/include

all: load56

load56: load56.o
	$(CC) -o load56 load56.o $(S56DSP)/lib/libqckMon.a

clean:
	rm -f load56 *.o make*.out *.lod *.lst
