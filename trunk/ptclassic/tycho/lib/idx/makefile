# Makefile for Tycho Index system, $TYCHO/lib/idx directory
#
# $Id$
#
# Copyright (c) 1996-%Q% The Regents of the University of California.
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

# Please don't use GNU make extensions in this or any other Tycho makefile

DIRS =

# Root of Tycho directory
ROOT =		../..

# Get configuration info
CONFIG =	$(ROOT)/mk/tycho.mk
include $(CONFIG)

TCL_SRCS = \
	HTMLMap2DAG.tcl \
	tychoMakeIndex.tcl

MISC_FILES =	

INDICES = \
	tycho.idx \
	codeDoc.idx

# Don't add $(INDICES) here, or running 'make sources' from the 
# top tycho directory will result in generating these files twice
EXTRA_SRCS =		$(TCL_SRCS) $(ITCL_SRCS)

# make clean will remove the indices.
KRUFT =		$(INDICES)

# We need not have 'make checkjunk' report that these are junk
OPTIONAL_FILES = 	$(INDICES) doc

all install: $(INDICES)

# Create all the indexes
indices: $(INDICES)

FORCE:

# Create a tycho index file.
# We could use the GNU make $(wildcard) extension here, but that would
# force users to have GNU make.
tycho.idx: FORCE
	@echo "About to create Tycho concept index"
	@echo "You should be sure to run 'make itcldocs' before updating $@"
	@echo "Updating $@"
	rm -f $@
	@if [ "$(TYCHO)" = "" ]; then \
		echo "TYCHO is not set so we assume we are part of PTOLEMY"; \
		echo "Running $(ITCLSH), \$$PTOLEMY=$(PTOLEMY)"; \
		echo "set TYCHO \"$(PTOLEMY)/tycho\"; source tychoMakeIndex.tcl; tychoStandardIndex" | $(ITCLSH); \
	else \
		echo "Running $(ITCLSH)"; \
		echo "set TYCHO \"$(TYCHO)\"; source tychoMakeIndex.tcl; tychoStandardIndex" | $(ITCLSH); \
	fi

# Create a tycho code index file
codeDoc.idx: FORCE
	@echo "About to create Tycho code index"
	@echo "You should be sure to run 'make itcldocs' before updating $@"
	@echo "Updating $@"
	rm -f $@
	@if [ "$(TYCHO)" = "" ]; then \
		echo "TYCHO is not set so we assume we are part of PTOLEMY"; \
		echo "Running $(ITCLSH), \$$PTOLEMY=$(PTOLEMY)"; \
		echo "set TYCHO \"$(PTOLEMY)/tycho\"; source tychoMakeIndex.tcl; tychoCodeDocIndex" | $(ITCLSH); \
	else \
		echo "Running $(ITCLSH)"; \
		echo "set TYCHO \"$(TYCHO)\"; source tychoMakeIndex.tcl; tychoCodeDocIndex" | $(ITCLSH); \
	fi

# We don't include common.mk since we don't use the compiler here
include $(ROOT)/mk/no-compile.mk
