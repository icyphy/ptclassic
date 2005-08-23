# Ptolemy Makefile for tkman
# Version identification:
# @(#)makefile	1.13 04/15/98
# Copyright (c) 1990-1998 The Regents of the University of California.
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
TKMAN_VERSION = 1.8b5
ROOT = ../../..
VPATH = $(ROOT)/src/utils/tkman-$(TKMAN_VERSION)
BINDIR = $(ROOT)/bin.$(PTARCH)

# get configuration info
CONFIG=$(ROOT)/mk/config-$(PTARCH).mk
include $(CONFIG)

SRCS =

KRUFT = 

MISC_FILES = 	Copyright MANIFEST Makefile.dist README-tkman contrib \
		searchbox.tcl taputils.tcl tkman-help.html tkman-help.tcl \
		tkman.1 tkman.tcl \
		ANNOUNCE-tkman CHANGES backslash face.xbm

# common.mk wants this, otherwise we get multiple targets for $(LIBDIR)/$(PTLIB)
PTLIB = foo


# We don't include common.mk since we don't use the compiler here
# include common definitions, rules
#include $(ROOT)/mk/common.mk

WISH=$(PTOLEMY)/bin.$(PTARCH)/iwish

all:	makefile  tkman
tkman:
	$(MAKE) -f $(VPATH)/Makefile.dist \
		VPATH=$(VPATH) \
		WISH=$(WISH) \
		BINDIR=$(BINDIR) \
		MANDIR=$(ROOT)/man/man1 \
		all

install:	$(ROOT)/man/man1 #tkman $(BINDIR)/tkman
	rm -f $(ROOT)/man/man1/tkman.1 $(BINDIR)/tkman
	$(MAKE) -f $(VPATH)/Makefile.dist \
		VPATH=$(VPATH) \
		WISH=$(WISH) \
		BINDIR=$(BINDIR) \
		MANDIR=$(ROOT)/man/man1 \
		install

# Can't use mkdir -p here, because not all machines have it
$(ROOT)/man/man1:
	if [ ! -d $(ROOT)/man ]; then mkdir $(ROOT)/man; fi
	if [ ! -d $(ROOT)/man/man1 ]; then mkdir $(ROOT)/man/man1; fi

$(BINDIR)/tkman:	tkman
	@echo Installing $<
	rm -f $@
	ln $< $@

$(ROOT)/man/man1/tkman.1: tkman.1
	@echo Installing $<
	rm -f $@
	ln $< $@
sources:	$(EXTRA_SRCS) $(SRCS) makefile

depend:
	@echo "no dependencies in tkman directory (contributed code)"

sccsinfo:
	sccs info
checkjunk:
	@checkextra -v $(SRCS) $(OBJS) $(MISC_FILES) makefile SCCS

clean:
	rm -f tkman *~
# Don't add anything after the next line; makedepend will zap it.
# DO NOT DELETE THIS LINE -- make depend depends on it.


