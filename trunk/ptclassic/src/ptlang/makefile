# Makefile for ptolemy language processor
# Version identification:
# $Id$
# Copyright (c) 1990 The Regents of the University of California.
#                       All Rights Reserved.

ROOT = ../..
VPATH = $(ROOT)/src/preproc
BINDIR = $(ROOT)/bin.$(ARCH)
YFLAGS =
CFLAGS = -O

all:	ptlang

ptlang:	ptlang.o
	cc ptlang.o -o ptlang

ptlang.o:	ptlang.y

install:	$(BINDIR)/ptlang

$(BINDIR)/ptlang:	ptlang
		rm -f $(BINDIR)/ptlang; cp ptlang $(BINDIR)

clean:
	rm *.o

sources:	ptlang.y

# Rule for making .o file from .c file (the Gnu-make default rule breaks
# on Vaxes running older versions of Ultrix)
.c.o:
	$(CC) $(CFLAGS) -c $<
