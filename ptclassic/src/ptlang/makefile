# Makefile for ptolemy language processor
# Version identification:
# $Id$
# Copyright (c) 1990-%Q% The Regents of the University of California.
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

ROOT = ../..
VPATH = $(ROOT)/src/ptlang
BINDIR = $(ROOT)/bin.$(ARCH)

# get configuration info
CONFIG=$(ROOT)/mk/config-$(ARCH).mk
include $(CONFIG)

all:	ptlang

ptlang:	ptlang.o
	rm -f ptlang
	$(CC) ptlang.o -o ptlang $(PTLANGLIB)

ptlang.o:	ptlang.c

$(VPATH)/ptlang.c:	ptlang.y
		cd $(VPATH); yacc ptlang.y; mv y.tab.c ptlang.c

install:	$(BINDIR)/ptlang

$(BINDIR)/ptlang:	ptlang
		rm -f $(BINDIR)/ptlang; ln ptlang $(BINDIR)

sources:	ptlang.y

clean:
	rm -f ptlang.c ptlang.o ptlang

depend:
	echo no dependencies to make in this directory

checkjunk:
	@checkextra -v ptlang.y ptlang.c ptlang.o ptlang makefile SCCS

sccsinfo:
	@sccs check || true

# Rule for making .o file from .c file (the Gnu-make default rule breaks
# on Vaxes running older versions of Ultrix)
.c.o:
	$(CC) $(CFLAGS) -c $<
