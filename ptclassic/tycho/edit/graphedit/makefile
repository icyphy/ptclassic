# Makefile for tycho.edit.graph package
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

# True source directory
VPATH =		$(ROOT)/editors/graphedit

# Get configuration info
CONFIG =	$(ROOT)/mk/tycho.mk
include $(CONFIG)

TCL_SRCS =
  
ITCL_SRCS =  \
	AbstractGraph.itcl \
	Digraph.itcl \
	EdgeController.itcl \
	GraphEditor.itcl \
	IconLibrary.itcl \
	IconPalette.itcl \
	Junction.itcl \
	Layout.itcl \
	Library.itcl \
	Net.itcl \
	ParameterDialog.itcl \
	PortedGraph.itcl \
	ScrolledSlate.itcl \
	SlateView.itcl \
	StarLibrary.itcl \
	VertexController.itcl \
	ViewController.itcl

EXTRA_SRCS = $(TCL_SRCS) $(ITCL_SRCS) \
	graphedit.style \
	icons.tim \
	stars.tim \
	graphedit.tcl \
	README.txt \
	pkgIndex.tcl

# Sub directories to run make in.
DIRS =		bin demo doc test

MISC_FILES =	$(DIRS) tclIndex

# make checkjunk will not report OPTIONAL_FILES as trash
# make realclean removes OPTIONAL_FILES
OPTIONAL_FILES = 	adm

# 'make realclean' should remove tclIndex
REALCLEAN_STUFF =	tclIndex

# Title for the tydoc index page
TYDOC_DESC =		Index of classes in tycho/kernel/graph

all: sources $(MISC_SRCS) tclIndex 

install: all subinstall

.PHONY: all install subinstall

# We don't include common.mk since we don't use the compiler here
include $(ROOT)/mk/no-compile.mk

