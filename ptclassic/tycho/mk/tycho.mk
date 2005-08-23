# Makefile for tycho standalone distribution
#
# Version identification:
# @(#)tycho.mk	1.40 09/24/98
#
# Copyright (c) 1996-1998 The Regents of the University of California.
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
# Date of creation: 7/31/96
# Author: Christopher Hylands

# This makefile should be included at the top of every Tycho makefile.
# This makefile uses the ROOT makefile variable.

# The destination directory for any libraries created.
# Usually this is an architecture dependent library.
LIBDIR =	$(ROOT)/lib.$(PTARCH)

# Provide an initial value for LIB_DEBUG so we don't get messages about
# multiply defined rules for $(LIBDIR)/$(LIB_DEBUG) if LIB_DEBUG is empty.
LIBR_DEBUG =	libdummy_g

# Itcl shell
#ITCLSH =	$(PTOLEMY)/tcltk/tcl.$(PTARCH)/bin/itclsh
ITCLSH =	itclsh

# Tcl script that generates html from itcl files
# Note that $(ROOT) here is relative to the tycho directory, not
# the Ptolemy directory.
TYDOC=$(ROOT)/util/tydoc/bin/tydoc
