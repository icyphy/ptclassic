# Makefile for Ptolemy Tycho system, editors/textedit directory.
#
# @Author: Christopher Hylands
#
# @Version: $Id$
#
# @Copyright (c) 1996-%Q% The Regents of the University of California.
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
VPATH =		$(ROOT)/editors/textedit

# Get configuration info
CONFIG =	$(ROOT)/mk/tycho.mk
include $(CONFIG)

TCL_SRCS = 
  
ITCL_SRCS = \
	EditC.itcl \
	EditCpp.itcl \
	EditDiff.itcl \
	EditEsterel.itcl \
	EditHTML.itcl \
	EditItcl.itcl \
	EditJava.itcl \
	EditMake.itcl \
	EditProgram.itcl \
	EditPtcl.itcl \
	EditPtlang.itcl \
	EditTcl.itcl \
	ItclClassList.itcl \
	Matlab.itcl \
	Mathematica.itcl

EXTRA_SRCS =		$(TCL_SRCS) $(ITCL_SRCS)

MISC_FILES =		doc img templates test tclIndex 

# Sub directories to run make in.
DIRS =			doc test templates

# 'make realclean' should remove tclIndex
REALCLEAN_STUFF =			tclIndex

# Title for the tydoc index page
TYDOC_DESC =	Index of classes in tycho/kernel/textedit

all install: sources tclIndex $(MISC_SRCS)

# We don't include common.mk since we don't use the compiler here
include $(ROOT)/mk/no-compile.mk
