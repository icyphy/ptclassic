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

DIRS = kernel editors lib domains slate devel doc typt #bin

# Root of Tycho directory
ROOT =		.

# Get configuration info
CONFIG =	$(ROOT)/mk/tycho.mk
include $(CONFIG)

MISC_FILES =	README README.files copyright .glimpse_exclude

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
sources:: subsources indices makefile README.files

# We use a subtarget here so that we do make sources in the subdirs
# before trying to update the indices
subsources:	$(EXTRA_SRCS) $(HDRS) $(MISC_FILES) makefile 
	@for x in $(DIRS); do \
	    if [ -w $$x ] ; then \
		( cd $$x ; \
		echo making sources in $$x ; \
		$(MAKE) $(MFLAGS) $(MAKEVARS) sources ;\
		) \
	    fi ; \
	done

indices:
	(cd lib/idx; $(MAKE))


# This rule should contains things that might take a little while
# to run, so it is run nightly
nightly: sources doc/tychoClasses.dag typt/lib/PtolemyFacets.fst \
			itcldocs indices glimpse

# Empty rule to force certain makefile rules to run
FORCE: 

# Create a Directed Acyclic Graph that contains the Tycho classes
doc/tychoClasses.dag: FORCE
	(cd doc; $(MAKE) tychoClasses.dag)

# Create a forest that contains all the Ptolemy universes
typt/lib/PtolemyFacets.fst: FORCE
	(cd typt/lib; rm -f PtolemyFacets.fst; $(MAKE) PtolemyFacets.fst)

# Glimpse is a tool that prepares an index of a directory tree.
# glimpse is not included with Tycho, see http://glimpse.cs.arizona.edu
GLIMPSEINDEX =	/usr/sww/bin/glimpseindex
glimpse:
	$(GLIMPSEINDEX) -H $(PTOLEMY)/tycho $(PTOLEMY)/tycho
	chmod a+r $(PTOLEMY)/tycho/.glimpse_*
