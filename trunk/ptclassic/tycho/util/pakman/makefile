# Makefile for the Tycho pakman package
#
# @Author: Christopher Hylands
#
# @Version: $Id$
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
VPATH =		$(ROOT)/util/pakman

# Get configuration info
CONFIG =	$(ROOT)/mk/tycho.mk
include $(CONFIG)

# The variables below are used to create distributions
TYPACKAGE = 	tycho.util.pakman
TYPACKAGE_ROOTDIR = $(ROOT)/..
TYPACKAGE_DIR = tycho/util/pakman
TYVERSION =	1.0
TYDIST =	$(TYPACKAGE)$(TYVERSION)
TYZIP =		$(TYPACKAGE_DIR)/$(TYPACKAGE).zip
TYJARFILE =

# Sub directories to run make in.
DIRS =	 bin demo doc test template.tycho template.simple

TCL_SRCS = \
	create.tcl \
	main.tcl \
	mirror.tcl \
	sccs.tcl

ITCL_SRCS = 

EXTRA_SRCS = $(TCL_SRCS) $(ITCL_SRCS) pakman.tcl pkgIndex.tcl

MISC_FILES =  $(DIRS) \
	README.txt \
	template.simple \
	template.tycho \
	tclIndex

# make checkjunk will not report OPTIONAL_FILES as trash
# make realclean removes OPTIONAL_FILES
OPTIONAL_FILES = 	adm

# 'make realclean' should remove tclIndex
REALCLEAN_STUFF = tclIndex

# Title for the tydoc index page
TYDOC_DESC =	Index of classes in tycho/util/pakman

# Do make sources so that we ensure that the subdirectories are checked out
all install: sources $(MISC_SRCS) tclIndex

# We don't include common.mk since we don't use the compiler here
include $(ROOT)/mk/no-compile.mk


