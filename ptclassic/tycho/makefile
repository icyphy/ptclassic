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


DIRS = kernel editors lib domains slate devel doc #bin

# Root of Tycho directory
ROOT =		.

# Get configuration info
CONFIG =	$(ROOT)/mk/tycho.mk
include $(CONFIG)

MISC_FILES =	README.files copyright

makefiles all clean depend realclean checkjunk sccsinfo itcldocs \
extraclean:
	@for x in $(DIRS); do \
	    if [ -w $$x ] ; then \
		( cd $$x ; \
		echo making $@ in tycho/lib/$$x ; \
		$(MAKE) $(MFLAGS) $(MAKEVARS) $@ ;\
		) \
	    fi ; \
	done

install: subinstall itcldocs tycho.idx
subinstall:
	@for x in $(DIRS); do \
	    if [ -w $$x ] ; then \
		( cd $$x ; \
		echo making $@ in tycho/lib/$$x ; \
		$(MAKE) $(MFLAGS) $(MAKEVARS) $@ ;\
		) \
	    fi ; \
	done

TAGS:
	echo "Making tags makes no sense in the tycho directory"

# "make sources" will do SCCS get on anything where SCCS file is newer.
# You probably don't want to add $(SRCS) to this, rather, change
# the makefile that includes this one to use $(EXTRA_SRCS)
sources:: subsources tycho.idx makefile README.files

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

# Create a tycho index file
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
		echo "Running $(ITCLSH)"; \
		echo "set TYCHO $(PTOLEMY)/tycho; source kernel/tychoMakeIndex.tcl; tychoStandardIndex" | $(ITCLSH); \
	else \
		echo "Running $(ITCLSH)"; \
		echo "set TYCHO $(TYCHO); source kernel/tychoMakeIndex.tcl; tychoStandardIndex" | $(ITCLSH); \
	fi
