# Makefile for ptolemy language processor
# Version identification:
# $Id$
# Copyright (c) 1990 The Regents of the University of California.
#                       All Rights Reserved.

ROOT = ../..
VPATH = $(ROOT)/src/preproc
BINDIR = $(ROOT)/bin.$(ARCH)
YFLAGS = -v
CFLAGS = -O

all:	ptlang

ptlang:	ptlang.o

install:	$(BINDIR)/ptlang

$(BINDIR)/ptlang:	ptlang
		rm -f $(BINDIR)/ptlang; cp ptlang $(BINDIR)

clean:
	rm *.o

sources:	ptlang.y
