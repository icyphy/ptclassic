# Makefile for Ptolemy graphical interface edg 5/23/90
# Version identification:
# @(#)make.template	1.20	6/4/96
#
# Copyright (c) 1990-1996 The Regents of the University of California.
# All rights reserved.
# 
# Permission is hereby granted, without written agreement and without
# license or royalty fees, to use, copy, modify, and distribute this
# software and its documentation for any purpose, provided that the
# above copyright notice and the following two paragraphs appear in all
# copies of this software.
# 
# IN NO EVENT SHALL THE UNIVERSITY OF CALIFORNIA BE LIABLE TO ANY PARTY
# FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES
# ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF
# THE UNIVERSITY OF CALIFORNIA HAS BEEN ADVISED OF THE POSSIBILITY OF
# SUCH DAMAGE.
# 
# THE UNIVERSITY OF CALIFORNIA SPECIFICALLY DISCLAIMS ANY WARRANTIES,
# INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
# MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. THE SOFTWARE
# PROVIDED HEREUNDER IS ON AN "AS IS" BASIS, AND THE UNIVERSITY OF
# CALIFORNIA HAS NO OBLIGATION TO PROVIDE MAINTENANCE, SUPPORT, UPDATES,
# ENHANCEMENTS, OR MODIFICATIONS.
# 
# 						PT_COPYRIGHT_VERSION_2
# 						COPYRIGHTENDKEY
# 
# Authors: Jose Luis Pino, Christopher Hylands
#
# This makefile may be copied and modified to make your own version of
# pigiRpc, with a different set of compiled-in stars or support for a
# different set of domains.  If you do this, change ROOT to point to
# the root of the Ptolemy source tree, change OBJDIR to point to
# the directory containing the object files, and revise ALLSTARS and
# ALLTARGETS to specify the stars and targets you wish to include
# (using symbols defined in the stars.mk file or your own additions).

# root of source tree
ROOT =		../..
# Path (absolute or relative) to the object directory root
OBJDIR =	..
# VPATH points to the "real" source directory
VPATH =		$(ROOT)/src/pigiRpc

# get configuration info
CONFIG =	$(ROOT)/mk/config-$(PTARCH).mk
include $(CONFIG)

-include override.mk

ifndef BASENAME
ifndef PIGI
	ALLBINARIES =	1
	BASENAME =	pigiRpc
endif
endif

TK=1
OCT=1

INCL = -I$(ROOT)/src/pigilib $(PTCOMPAT_INCSPEC)

SRCS =		pigiMain.cc

PT_DEPEND +=	pigiMain.o

ADD_OBJS =	palettes.$(PIGI).o

PIGI_OBJS +=	pigiMain.o $(ADD_OBJS)

# common.mk wants this, otherwise we get multiple targets for $(LIBDIR)/$(PTLIB)
PTLIB = foo

# 'make clean' should remove these derived files
KRUFT = $(wildcard palettes.*.c) version.c *_linklog

include $(ROOT)/mk/ptbin.mk

all: $(PIGI) $(PIGI).ptiny $(PIGI).ptrim

# Can't use echo -n on hppa, see the hppa echo man page.
# First we substitute 'pal:' for 'pal '
# Then we substitute \$$PTOLEMY for PTOLEMY
palettes.$(PIGI).c: $(PT_DEPEND)
	echo "char* defaultPalettes(){ return \"$(subst PTOLEMY,\$$PTOLEMY,$(subst pal ,pal:,$(sort $(PALETTES)))):./user.pal:./init.pal\";}" > palettes.$(PIGI).c
	$(CC) -c palettes.$(PIGI).c	

include $(ROOT)/mk/common.mk

# Don't add anything after the next line; makedepend will zap it.
# DO NOT DELETE THIS LINE -- make depend depends on it.

pigiMain.o: pigiMain.cc ../../src/pigilib/kernelCalls.h \
 ../../src/pigilib/sol2compat.h ../../src/pigilib/pthreadsCompat.h \
 ../../src/compat/ptolemy/compat.h ../../src/pigilib/paramStructs.h \
 ../../src/pigilib/pigiLoader.h ../../src/pigilib/xfunctions.h
