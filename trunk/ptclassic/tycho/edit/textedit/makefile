# Makefile for Ptolemy Tycho system, tycho.edit.textedit package
#
# @Author: Christopher Hylands
#
# @Version: @(#)makefile	1.31 02/04/99
#
# @Copyright (c) 1996-1999 The Regents of the University of California.
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
# Path (absolute or relative) to the object directory root
OBJDIR =	../..
# True source directory
VPATH =		$(ROOT)/edit/textedit

# Get configuration info
CONFIG =	$(ROOT)/mk/tycho.mk
include $(CONFIG)

TCL_SRCS = \
	genVCMakefile.tcl
  
ITCL_SRCS = \
	EditC.itcl \
	EditCpp.itcl \
	EditEsterel.itcl \
	EditHTML.itcl \
	EditItcl.itcl \
	EditJava.itcl \
	EditMake.itcl \
	EditMail.itcl \
	EditProgram.itcl \
	EditSDL.itcl \
	EditTcl.itcl \
	HTMLOutline.itcl \
	Matlab.itcl \
	Mathematica.itcl

EXTRA_SRCS = $(TCL_SRCS) $(ITCL_SRCS) \
	editcolors.style \
	edithtml.style \
	makefile.style \
	pkgIndex.tcl \
	textedit.tcl

MISC_FILES = doc img templates test tclIndex 

# Sub directories to run make in.
DIRS =	doc test templates

# 'make realclean' should remove tclIndex
REALCLEAN_STUFF = tclIndex

# Title for the tydoc index page
TYDOC_DESC =	Index of classes in tycho/edit/textedit

all install: sources tclIndex $(MISC_SRCS)

# We don't include common.mk since we don't use the compiler here
include $(ROOT)/mk/no-compile.mk
