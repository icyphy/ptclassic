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

# Root of Tycho directory
ROOT =		..

# True source directory
VPATH =		$(ROOT)/kernel

# Get configuration info
CONFIG =	$(ROOT)/mk/tycho.mk
include $(CONFIG)

TCL_SRCS = \
	List.tcl \
	Loader.tcl \
	Lib.tcl \
	Math.tcl \
	Misc.tcl \
	Path.tcl \
	String.tcl \
	tychoMakeClassDAG.tcl

ITCL_SRCS = \
	AbstractGraph.itcl \
	ButtonBox.itcl \
	Build.itcl \
	BuildDialog.itcl \
	CircularList.itcl \
	Class.itcl \
	ColorBrowser.itcl \
	ColorManager.itcl \
	Compile.itcl \
	CompileCTcl.itcl \
	CompileJava.itcl \
	ControlPanel.itcl \
	ControlSubpanel.itcl \
	Dialog.itcl \
	Digraph.itcl \
	Displayer.itcl \
	DirectedAcyclicGraph.itcl \
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
	Forest.itcl \
	Glimpse.itcl \
	Graph.itcl \
	HTML.itcl \
	HTMLMessage.itcl \
	HyperlinkIndex.itcl \
	IndexBrowser.itcl \
	Interchange.itcl \
	KeyedDigraph.itcl \
	ListBrowser.itcl \
	MenuBar.itcl \
	MenuSupport.itcl \
	Message.itcl \
	Model.itcl \
	Monitor.itcl \
	MultiPanel.itcl \
	Object.itcl \
	PreferenceDialog.itcl \
	PreferenceEditor.itcl \
	PopupMenu.itcl \
	Query.itcl \
	RevControl.itcl \
	RevControlSCCS.itcl \
	RevControlRCS.itcl \
	Scheduler.itcl \
	Stack.itcl \
	StatusBar.itcl \
	Subpanel.itcl \
	TclShell.itcl \
	ToolBar.itcl \
	TopLevel.itcl \
	TWidget.itcl \
	Tycho.tcl \
	Uninstantiable.itcl \
	View.itcl \
	WelcomeMessage.itcl \
	WidgetPreferences.itcl \
	YesNoCancel.itcl \
	YesNoQuery.itcl

EXTRA_SRCS =		$(TCL_SRCS) $(ITCL_SRCS)

MISC_FILES =		doc img test tclIndex Preferences

# Sub directories to run make in.
DIRS =			doc test

# 'make realclean' should remove tclIndex
REALCLEAN_STUFF =	tclIndex

# Title for the tydoc index page
TYDOC_DESC =	Index of classes in tycho/kernel

# Do make sources so that we ensure that the subdirectories are checked out
all install: sources $(MISC_SRCS) tclIndex

# We don't include common.mk since we don't use the compiler here
include $(ROOT)/mk/no-compile.mk
