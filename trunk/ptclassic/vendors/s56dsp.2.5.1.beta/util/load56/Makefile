#
# Makefile for load56
#
# Copyright (c) 1991, Berkeley Camera Engineering, All Rights Reserved.
#	$Id$
#

CFLAGS = -O2 -I${S56DSP}/include

all:	load56
	mv -f load56 ${S56DSP}/bin

test:	load56

load56: load56.o
	$(CC) -o load56 load56.o ${S56DSP}/lib/libqckMon.a

clean:
	rm -f *.o make*.out *.lst

distribution:	clean
	rm -f load56 ${S56DSP}/bin/load56 *.lod

check:
	sccs check; sccs prt -y *.[ch] > /dev/null
