# Makefile for Ptolemy Tycho system, devel directory.
#
# $Id$
# Makefile Author: Christopher Hylands
#
# Copyright (c)  The Regents of the University of California.
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
VPATH =		$(ROOT)/devel

# Get configuration info
CONFIG =	$(ROOT)/mk/tycho.mk
include $(CONFIG)

TCL_SRCS = \
	tychoSend.tcl
  
ITCL_SRCS = \
	Curriculum.itcl \
	EditLattice.itcl \
	GanttView.itcl \
	GanttDF.itcl \
	HyperGraph.itcl \
	IconIndex.itcl \
	Index.itcl \
	Lattice.itcl \
	ModelDebug.itcl \
	ModelSchema.itcl \
	MultiGraph.itcl \
	Options.tcl \
	Runcontrol.itcl \
	Schedule.itcl \
	UndirectedGraph.itcl \
	VSM.itcl \
	Strip.itcl \
	SlateView.itcl 

EXTRA_SRCS =		$(TCL_SRCS) $(ITCL_SRCS)

MISC_FILES =		tclIndex doc

# Sub directories to run make in.
DIRS =			# doc

# 'make realclean' should remove tclIndex
REALCLEAN_STUFF =	tclIndex

all install: $(MISC_SRCS) #tclIndex

# We don't include common.mk since we don't use the compiler here
include $(ROOT)/mk/no-compile.mk

