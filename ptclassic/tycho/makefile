# Makefile for Ptolemy Tycho system, top Tycho directory
#
# $Id$
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


DIRS = kernel editors lib domains slate #bin doc 

# Root of Ptolemy directory
ROOT =		..

# get configuration info
CONFIG =	$(ROOT)/mk/config-$(PTARCH).mk
include $(CONFIG)

tycho.idx: kernel/tychoMakeIndex.tcl \
	    $(wildcard kernel/doc/*.html \
		    kernel/doc/usersGuides/*.html \
		    kernel/doc/codeDoc/*.html \
		    kernel/doc/internals/*.html \
		    editors/textedit/doc/*.html \
		    editors/textedit/doc/usersGuides/*.html)
	@echo "Updating $@"
	rm -f $@
	@if [ "$(TYCHO)" = "" ]; then \
		echo "TYCHO is not set so we assume we are part of PTOLEMY"; \
		echo "Running $(ITCL2_0_SH)"; \
		echo "set TYCHO $(PTOLEMY)/tycho; source kernel/tychoMakeIndex.tcl; tychoStandardIndex" | $(ITCL2_0_SH); \
	else \
		echo "Running $(ITCL2_0_SH)"; \
		echo "set TYCHO $(TYCHO); source kernel/tychoMakeIndex.tcl; tychoStandardIndex" | $(ITCL2_0_SH); \
	fi

# We don't include common.mk since we don't use the compiler here
include $(ROOT)/mk/no-compile.mk

# This rule must be after include no-compile.mk so that get any sources
# in the subdirs before generating the index
sources:: tycho.idx
