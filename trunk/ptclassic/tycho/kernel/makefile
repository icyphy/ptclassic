# Makefile for Ptolemy Tycho system, kernel directory.
#
# $Id$
# Author: Christopher Hylands
#
# Copyright (c) 1991-%Q% The Regents of the University of California.
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
##########################################################################

# If this file's name is "make.template" and there is no makefile in
# this directory, do
#	cp make.template makefile
#	make depend
# This will add the dependencies to the makefile.

# Root of Ptolemy directory
ROOT =		../..
# Path (absolute or relative) to the object directory root
OBJDIR =	../..
# True source directory
VPATH =		$(ROOT)/tycho/kernel_itk

# get configuration info
CONFIG =	$(ROOT)/mk/config-$(PTARCH).mk
include $(CONFIG)

TCL_SRCS = \
	Color.tcl \
	Lib.tcl \
	Options.tcl
  
ITCL_SRCS = \
	Console.itcl \
	DialogWindow.itcl \
	Dismiss.itcl \
	Edit.itcl \
	EditText.itcl \
	ErrorMessage.itcl \
	File.itcl \
	FileBrowser.itcl \
	Font.itcl \
	Message.itcl \
	Preferences.itcl \
	TopLevel.itcl \
	YesNoCancel.itcl \
	YesNoQuery.itcl

EXTRA_SRCS =		$(TCL_SRCS) $(ITCL_SRCS)

MISC_FILES =		Ptolemy.xbm tclIndex

# Include information on all stars and targets.
include $(ROOT)/mk/stars.mk

all: tclIndex

tclIndex: $(TCL_SRCS) $(ITCL_SRCS)
	@echo "Updating tclIndex"
	rm -f $@
	echo 'set auto_path [linsert $$auto_path 0 $(ITCL_LIBDIR)]; itcl_mkindex . $(TCL_SRCS) $(ITCL_SRCS)' | $(ITCL_SH)

install: tclIndex

LIB=dummy		# required since no LIB
# include common definitions, rules
include $(ROOT)/mk/common.mk

# Don't add anything after the next line; makedepend will zap it.
# DO NOT DELETE THIS LINE -- make depend depends on it.

