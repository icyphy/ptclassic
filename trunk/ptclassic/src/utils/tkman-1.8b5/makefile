# Ptolemy Makefile for tkman
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
TKMAN_VERSION = 1.7.3
ROOT = ../../..
VPATH = $(ROOT)/src/utils/tkman-$(TKMAN_VERSION)
BINDIR = $(ROOT)/bin.$(ARCH)

# get configuration info
CONFIG=$(ROOT)/mk/config-$(ARCH).mk
include $(CONFIG)

SRCS =

KRUFT =

# common.mk wants this, otherwise we get multiple targets for $(LIBDIR)/$(LIB)
LIB = foo


# include common definitions, rules
#include $(ROOT)/mk/common.mk
WISH=$(PTOLEMY)/bin.$(PTARCH)/wish4.0

all:	makefile  tkman
tkman:
	$(MAKE) -f $(VPATH)/Makefile.dist \
		VPATH=$(VPATH) \
		WISH=$(WISH) \
		BINDIR=$(BINDIR) \
		MANDIR=$(ROOT)/man/man1 \
		all

install:	#tkman $(BINDIR)/tkman $(ROOT)/man/man1/tkman.1
	$(MAKE) -f $(VPATH)/Makefile.dist \
		VPATH=$(VPATH) \
		WISH=$(WISH) \
		BINDIR=$(BINDIR) \
		MANDIR=$(ROOT)/man/man1 \
		install

$(BINDIR)/tkman:	tkman
	@echo Installing $<
	rm -f $@
	ln $< $@

$(ROOT)/man/man1/tkman.1: tkman.1
	@echo Installing $<
	rm -f $@
	ln $< $@
sources:	$(PTLANG_IN_OBJ) $(EXTRA_SRCS) $(SRCS) $(HDRS) make.template 
clean:
	rm -f tkman *~
# Don't add anything after the next line; makedepend will zap it.
# DO NOT DELETE THIS LINE -- make depend depends on it.


