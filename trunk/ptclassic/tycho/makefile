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

# Please don't use GNU make extensions in this or any other Tycho makefile

DIRS = kernel editors lib domains slate devel doc #bin

# Root of Tycho directory
ROOT =		.

# Get configuration info
CONFIG =	$(ROOT)/mk/tycho.mk
include $(CONFIG)

MISC_FILES =	README README.files copyright

INDICES = 	tycho.idx codeDoc.idx

makefiles all clean depend realclean checkjunk sccsinfo itcldocs \
extraclean:
	@for x in $(DIRS); do \
	    if [ -w $$x ] ; then \
		( cd $$x ; \
		echo making $@ in $$x ; \
		$(MAKE) $(MFLAGS) $(MAKEVARS) $@ ;\
		) \
	    fi ; \
	done

install: subinstall $(INDICES)
subinstall:
	@for x in $(DIRS); do \
	    if [ -w $$x ] ; then \
		( cd $$x ; \
		echo making install in $$x ; \
		$(MAKE) $(MFLAGS) $(MAKEVARS) install ;\
		) \
	    fi ; \
	done

TAGS:
	echo "Making tags makes no sense in the tycho directory"

# "make sources" will do SCCS get on anything where SCCS file is newer.
# You probably don't want to add $(SRCS) to this, rather, change
# the makefile that includes this one to use $(EXTRA_SRCS)
sources:: subsources $(INDICES) makefile README.files

# We use a subtarget here so that we do make sources in the subdirs
# before trying to update tycho.idx
subsources:	$(EXTRA_SRCS) $(HDRS) $(MISC_FILES) makefile 
	@for x in $(DIRS); do \
	    if [ -w $$x ] ; then \
		( cd $$x ; \
		echo making sources in $$x ; \
		$(MAKE) $(MFLAGS) $(MAKEVARS) sources ;\
		) \
	    fi ; \
	done

# Creat all the indexes
indices: $(INDICES)

FORCE:

# Create a tycho index file.
# We could use the GNU make $(wildcard) extension here, but that would
# force users to have GNU make.
tycho.idx: FORCE
	@echo "Updating $@"
	rm -f $@
	@if [ "$(TYCHO)" = "" ]; then \
		echo "TYCHO is not set so we assume we are part of PTOLEMY"; \
		echo "Running $(ITCLSH), \$$PTOLEMY=$(PTOLEMY)"; \
		echo "set TYCHO $(PTOLEMY)/tycho; source kernel/tychoMakeIndex.tcl; tychoStandardIndex" | $(ITCLSH); \
	else \
		echo "Running $(ITCLSH)"; \
		echo "set TYCHO $(TYCHO); source kernel/tychoMakeIndex.tcl; tychoStandardIndex" | $(ITCLSH); \
	fi

# Create a tycho code index file
codeDoc.idx: FORCE
	@echo "Updating $@"
	rm -f $@
	@if [ "$(TYCHO)" = "" ]; then \
		echo "TYCHO is not set so we assume we are part of PTOLEMY"; \
		echo "Running $(ITCLSH), \$$PTOLEMY=$(PTOLEMY)"; \
		echo "set TYCHO $(PTOLEMY)/tycho; source kernel/tychoMakeIndex.tcl; tychoCodeDocIndex" | $(ITCLSH); \
	else \
		echo "Running $(ITCLSH)"; \
		echo "set TYCHO $(TYCHO); source kernel/tychoMakeIndex.tcl; tychoCodeDocIndex" | $(ITCLSH); \
	fi
