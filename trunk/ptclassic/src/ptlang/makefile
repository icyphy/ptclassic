# Makefile for ptolemy language processor
# Version identification:
# $Id$
# Copyright (c) 1990 The Regents of the University of California.
#                       All Rights Reserved.

ROOT = ../..
VPATH = $(ROOT)/src/ptlang
BINDIR = $(ROOT)/bin.$(ARCH)
YFLAGS =
CFLAGS = -O

all:	ptlang

ptlang:	ptlang.o
	rm -f ptlang
	cc ptlang.o -o ptlang

ptlang.o:	ptlang.c

ptlang.c:	ptlang.y
		cd $(VPATH); yacc ptlang.y; mv y.tab.c ptlang.c

install:	$(BINDIR)/ptlang

$(BINDIR)/ptlang:	ptlang
		rm -f $(BINDIR)/ptlang; ln ptlang $(BINDIR)

clean:
	rm -f *.o

realclean:
	rm -f *.o ptlang.c

sources:	ptlang.y

depend:
	echo no dependencies to make in this directory

sccsinfo:
		sccs info

# Rule for making .o file from .c file (the Gnu-make default rule breaks
# on Vaxes running older versions of Ultrix)
.c.o:
	$(CC) $(CFLAGS) -c $<
