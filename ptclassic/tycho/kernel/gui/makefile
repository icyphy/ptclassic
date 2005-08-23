# Makefile for the Tycho tycho.kernel.gui package
#
# @Author: Christopher Hylands
#
# @Version: @(#)makefile	1.14 06/09/98
#
# @Copyright (c) 1998 The Regents of the University of California.
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


# Root of Tycho directory
ROOT =		../..

# True source directory
VPATH =		$(ROOT)/kernel/gui

# Get configuration info
CONFIG =	$(ROOT)/mk/tycho.mk
include $(CONFIG)

# Sub directories to run make in.
DIRS =	 bin demo doc test

TCL_SRCS = 

ITCL_SRCS = \
	ButtonBox.itcl \
	BuilderDialog.itcl \
	ColorBrowser.itcl \
	ColorManager.itcl \
	ColorPicker.itcl \
	Dialog.itcl \
	Displayer.itcl \
	Edit.itcl \
	EditSearch.itcl \
	EditSpell.itcl \
	EditStack.itcl \
	ErrorMessage.itcl \
	Exec.itcl \
	File.itcl \
	FileBrowser.itcl \
	FontManager.itcl \
	FontDialog.itcl \
	IndexBrowser.itcl \
	LabeledGrid.itcl \
	ListBrowser.itcl \
	MenuBar.itcl \
	MenuSupport.itcl \
	Message.itcl \
	Monitor.itcl \
	PopupMenu.itcl \
	StyleChooser.itcl \
	StyleEditor.itcl \
	Query.itcl \
	RevControl.itcl \
	RevControlSCCS.itcl \
	RevControlRCS.itcl \
	Shortcuts.itcl \
	StatusBar.itcl \
	StyleSheet.itcl \
	Subpanel.itcl \
	TArchetype.itcl \
	TclShell.itcl \
	ToolBar.itcl \
	TopLevel.itcl \
	TWidget.itcl \
	UserProfile.itcl \
	WidgetTree.itcl \
	View.itcl \
	YesNoCancel.itcl \
	YesNoQuery.itcl

EXTRA_SRCS = $(TCL_SRCS) $(ITCL_SRCS) \
	gui.tcl \
	README.txt \
	makefile \
	pkgIndex.tcl

MISC_FILES =  $(DIRS) tclIndex

# make checkjunk will not report OPTIONAL_FILES as trash
OPTIONAL_FILES = adm

# 'make realclean' should remove tclIndex
REALCLEAN_STUFF =

# Title for the tydoc index page
TYDOC_DESC =

# Do make sources so that we ensure that the subdirectories are checked out
all install: sources $(MISC_SRCS)

install: all subinstall

.PHONY: install all subinstall

test_exec_msg:
	@echo "This message is used to test the Exec class"

# We don't include common.mk since we don't use the compiler here
include $(ROOT)/mk/no-compile.mk


