# Makefile for Ptolemy Tycho system, tycho/doc directory.
#
# $Id$
# Author: Christopher Hylands
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

# Root of Ptolemy directory
ROOT =		..

# True source directory
VPATH =		$(ROOT)/doc

# Get configuration info
CONFIG =	$(ROOT)/mk/tycho.mk
include $(CONFIG)

EXTRA_SRCS =	abbreviations.html \
		authors.html \
		bindings.html \
		brahe.html \
		bugs0.1.html \
		bugs.html \
		documentation.html \
		help.html \
		index.html \
		install.html \
		introduction.html \
		itcllinks.html \
		overview.html \
		policies.html \
		ranking.html \
		rationale.html \
		releasenotes.html \
		releasenotes0.1.html \
		releasenotes0.1.1.html \
		releasenotes0.2.html \
		running.html \
		sponsors.html \
		tim.html \
		troubleshooting.html \
		tychoClasses.html \
		tychoFaq.html \
		tymac.html \
		tynt.html \
		tyunix.html


# Files or directories that are present, but that 'make checkjunk' 
# should not complain about
MISC_FILES =		coding demo img itclhtml tychoClasses.dag

# make checkjunk will not report OPTIONAL_FILES as trash
# make realclean removes OPTIONAL_FILES
OPTIONAL_FILES = 	

# 'make realclean' should remove tclIndex
REALCLEAN_STUFF =	tychoClasses.dag

# Sub directories to run make in.
DIRS =			coding demo



all: tychoClasses.dag

FORCE:

tychoClasses.dag: FORCE
	@echo "Generating diagram of tycho classes"
	rm -f $@
	@if [ "$(TYCHO)" = "" ]; then \
		echo "TYCHO is not set so we assume we are part of PTOLEMY"; \
		echo "Running $(ITCLSH), \$$PTOLEMY=$(PTOLEMY)"; \
		echo "set TYCHO \"$(PTOLEMY)/tycho\"; source ../kernel/tychoMakeClassDAG.tcl; tychoStandardDAG" | $(ITCLSH); \
	else \
		echo "Running $(ITCLSH)"; \
		echo "set TYCHO \"$(TYCHO)\"; source ../kernel/tychoMakeClassDAG.tcl; tychoStandardDAG" | $(ITCLSH); \
	fi

HTMLDIRS= $(TYCHO)/doc \
	$(TYCHO)/doc/coding \
	$(TYCHO)/lib/tydoc/doc \
	$(TYCHO)/kernel/doc/usersGuides \
	$(TYCHO)/editors/textedit/doc/usersGuides \
	$(TYCHO)/editors/visedit/doc/usersGuides \
	$(TYCHO)/editors/slate/doc/usersGuides \
	$(TYCHO)/kernel/doc/internals \
	$(TYCHO)/editors/slate/doc/internals \
	$(TYCHO)/src/tytest \
	$(TYCHO)/src/tytimer \
	$(TYCHO)/java/doc \
	$(TYCHO)/java/pn \
	$(TYCHO)/java/pt/kernel \
	$(TYCHO)/java/tycho/doc/internals \
	$(TYCHO)/typt/doc \
	$(TYCHO)/typt/doc/internals \
	$(TYCHO)/typt/doc/usersGuides \
	$(TYCHO)/typt/editors/doc/usersGuides



# Traverse all the html files (except the codedoc files) and produce PostScript
tycho.ps: ../bin/html2ps
	netscape &
	-chmod a+x $^
	../bin/html2ps -d index.html tycho.ps $(HTMLDIRS)

# We don't include common.mk since we don't use the compiler here
include $(ROOT)/mk/no-compile.mk
