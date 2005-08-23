# Makefile windows dumpexts binary, used to create dlls
# Version identification:
# @(#)makefile	1.3 08/25/97
# Copyright (c) 1997 The Regents of the University of California.
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
# Date of creation: 8/23/97
# Authors: Christopher Hylands
#
# If this file's name is "make.template" and there is no makefile in
# this directory, do
#	cp make.template makefile
#	make depend
# This will add the dependencies to the makefile.

# Root of Ptolemy directory
ROOT =		../../..
# Path (absolute or relative) to the object directory root
OBJDIR =	../..
# True source directory
VPATH =		$(ROOT)/src/utils/dumpexts

BINDIR = $(ROOT)/bin.$(PTARCH)

# get configuration info
CONFIG =	$(ROOT)/mk/config-$(PTARCH).mk
include $(CONFIG)

DUMPEXTSBIN = 	dumpexts.exe
all: $(DUMPEXTSBIN)

NON_LIB_CSRCS =	winDumpExts.c
SRCS =		$(NON_LIB_CSRCS)

# Header files
HDRS =

# Objects that make up the library
OBJS =		$(LIB_CSRCS:.c=.o)

$(DUMPEXTSBIN): $(SRCS)
	$(cc32) $(CON_CFLAGS) -Fo$(TMPDIR)\ $?
	set LIB=$(TOOLS32)\lib
	$(link32) $(linkdebug) $(conlflags) -out:$@ $(TMPDIR)\winDumpExts.o $(guilibs) libc.lib

install: $(BINDIR)/$(DUMPEXTSBIN)
$(BINDIR)/$(DUMPEXTSBIN): $(DUMPEXTSBIN)
	rm -f $@
	ln $(DUMPEXTSBIN) $(BINDIR)
sources: $(SRCS)

clean:
	rm -f $(DUMPEXTBIN) *.o

depend:
	echo no dependencies here
checkjunk:
	@checkextra -v $(SRCS) $(DUMPEXTSBIN) makefile SCCS

sccsinfo:
	@sccs check || true

