# Makefile included at the root of each domain (eg. SDF, DE, etc)
# @(#)topdomain.mk	1.34	04/08/97
# Copyright (c) 1990-1996 The Regents of the University of California.
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
# Date of creation: 6/15/92.
# Written by: J. Buck

MAKEVARS = "PTARCH=$(PTARCH)"

makefiles:
	@for x in $(DIRS); do \
	    if [ -w $$x ] ; then \
		( cd $$x ; \
		  echo Updating makefile in domains/$(ME)/$$x ; \
		  $(MAKE) -f make.template $(MFLAGS) $(MAKEVARS) \
			VPATH=../../../../src/domains/$(ME)/$$x $@ ; \
		) \
	    fi ; \
	done

all install clean realclean extraclean checkjunk sccsinfo weblint:
	@for x in $(DIRS); do \
	    if [ -w $$x ] ; then \
		( cd $$x ; \
		  echo making $@ in domains/$(ME)/$$x ; \
		  $(MAKE) $(MFLAGS) $(MAKEVARS) \
			VPATH=../../../../src/domains/$(ME)/$$x $@ ; \
		) \
	    fi ; \
	done

sources: subsources starHTML.idx
subsources:
	@for x in $(DIRS); do \
	    if [ -w $$x ] ; then \
		( cd $$x ; \
		  echo making sources in domains/$(ME)/$$x ; \
		  $(MAKE) $(MFLAGS) $(MAKEVARS) \
			VPATH=../../../../src/domains/$(ME)/$$x sources ; \
		) \
	    fi ; \
	done

.PHONY:	TAGS starHTML.idx domain.idx
TAGS:
	@for x in $(DIRS); do \
	    if [ -w $$x ] ; then \
		( cd $$x ; \
		  echo making $@ in domains/$(ME)/$$x ; \
		  $(MAKE) $(MFLAGS) $(MAKEVARS) \
			VPATH=../../../../src/domains/$(ME)/$$x $@ ; \
		) \
	    fi ; \
	done
	-cat -s $(DIRS:%=%/TAGS) > $@

depend:
	@for x in $(DIRS); do \
	    if [ -w $$x ] ; then \
		( cd $$x ; \
		  echo making $@ in domains/$(ME)/$$x ; \
		  $(MAKE) -f make.template $(MFLAGS) $(MAKEVARS) \
			VPATH=../../../../src/domains/$(ME)/$$x $@ ; \
		) \
	    fi ; \
	done

