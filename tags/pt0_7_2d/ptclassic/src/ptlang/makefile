# Makefile for ptolemy language processor
# Version identification:
# @(#)makefile	2.25	01/26/99
# Copyright (c) 1990-1997 The Regents of the University of California.
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
BINDIR = $(ROOT)/bin.$(PTARCH)

# get configuration info
CONFIG=$(ROOT)/mk/config-$(PTARCH).mk
include $(CONFIG)

BINARY = ptlang$(BINARY_EXT)
all:	$(BINARY)

SRCS = 	ptlang.y makefile.vc

ifneq (,$(filter hppa%,$(PTARCH)))
# We ship a ptlang.c that was created under Solaris2.4
# Unfortunately, the solaris2.4 ptlang.c will not compile
# under hppa or hppa.cfront (alloca problems), so we
# recreate ptlang.c by hand here.
$(BINARY):	$(SRCS)
	cd $(VPATH); $(YACC) $^; mv y.tab.c ptlang.c
	rm -f $@
	$(CC) $(VPATH)/ptlang.c -o $@ $(PTLANGLIB)
else
$(BINARY):	ptlang.o
	rm -f $@
	$(CC) $^ -o $(BINARY) $(PTLANGLIB)
endif

ptlang.o:	ptlang.c

$(VPATH)/ptlang.c:	ptlang.y
		cd $(VPATH); $(YACC) ptlang.y; mv y.tab.c ptlang.c

# Debugging version of ptlang
ptlang.debug:	ptlang.debug.o
	rm -f $@
	$(CC) $^ -o $@ $(PTLANGLIB)
ptlang.debug.o: 	ptlang.debug.c
	$(CC) -g -c -o $@ -DYYERROR_VERBOSE -DYYDEBUG=1 $^
$(VPATH)/ptlang.debug.c:	ptlang.y
		cd $(VPATH); bison -y -t ptlang.y; mv y.tab.c ptlang.debug.c


install:	$(BINDIR)/ptlang$(BINARY_EXT)

$(BINDIR)/ptlang$(BINARY_EXT):	$(BINARY)
		rm -f $@; ln $^ $@

sources:	$(SRCS)

clean:
	rm -f ptlang.c ptlang.o ptlang ptlang.debug.*

realclean: clean

extraclean: realclean
	rm -f $(SRCS)

depend:
	echo no dependencies to make in this directory

checkjunk:
	@checkextra -v ptlang.y ptlang.c ptlang.o ptlang makefile SCCS \
		makefile.vc

sccsinfo:
	@sccs check || true

# Rule for making .o file from .c file (the Gnu-make default rule breaks
# on Vaxes running older versions of Ultrix)
.c.o:
	$(CC) $(CFLAGS) -c $<
