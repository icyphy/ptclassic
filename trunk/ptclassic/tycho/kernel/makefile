# Makefile for Ptolemy Tycho system, kernel directory.
#
# @Author: Christopher Hylands
#
# @Version: $Id$
#
# @Copyright (c) 1995-%Q% The Regents of the University of California.
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

# Root of Ptolemy directory
ROOT =		../..
# Path (absolute or relative) to the object directory root
OBJDIR =	../..
# True source directory
VPATH =		$(ROOT)/tycho/kernel

# get configuration info
CONFIG =	$(ROOT)/mk/config-$(PTARCH).mk
include $(CONFIG)

TCL_SRCS = \
	Color.tcl \
	Lib.tcl \
	tychoMakeIndex.tcl
  
ITCL_SRCS = \
	CircularList.itcl \
	Console.itcl \
	ConsoleText.itcl \
	DialogWindow.itcl \
	Dismiss.itcl \
	Edit.itcl \
	EditText.itcl \
	EditSearch.itcl \
	EditSpell.itcl \
	EditStack.itcl \
	EditStackText.itcl \
	EntryQuery.itcl \
	ErrorMessage.itcl \
	File.itcl \
	FileBrowser.itcl \
	FontManager.itcl \
	HTML.itcl \
	HTMLText.itcl \
	IndexBrowser.itcl \
	ListBrowser.itcl \
	Message.itcl \
	RevControl.itcl \
	RevControlSCCS.itcl \
	RevControlRCS.itcl \
	Stack.itcl \
	Subpanel.itcl \
	TopLevel.itcl \
	Tycho.tcl \
	TychoWidget.itcl \
	WelcomeMessage.itcl \
	YesNoCancel.itcl \
	YesNoQuery.itcl

EXTRA_SRCS =		$(TCL_SRCS) $(ITCL_SRCS)

MISC_FILES =		doc img test tclIndex 

# Sub directories to run make in.
DIRS =			doc

# 'make realclean' should remove tclIndex
REALCLEAN_STUFF =	tclIndex

# Include information on all stars and targets.
include $(ROOT)/mk/stars.mk

# Do make sources so that we ensure that the subdirectories are checked out
all install: sources $(MISC_SRCS) tclIndex

tclIndex: $(TCL_SRCS) $(ITCL_SRCS)
	@echo "Updating tclIndex"
	rm -f $@
	echo 'set auto_path [linsert $$auto_path 0 $(ITCL_LIBDIR)]; auto_mkindex . $(TCL_SRCS) $(ITCL_SRCS)' | $(ITCL2_0_SH)

# We don't include common.mk since we don't use the compiler here
include $(ROOT)/mk/no-compile.mk

# Don't add anything after the next line; makedepend will zap it.
# DO NOT DELETE THIS LINE -- make depend depends on it.

