# Makefile included at the root of each domain (eg. SDF, DE, etc)
# $Id$
# Copyright (c) 1990-%Q% The Regents of the University of California.
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

all install clean  realclean checkjunk sccsinfo:
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

.PHONY:	TAGS
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

doc/stars/starHTML.idx: $(wildcard doc/stars/*.htm)
	@echo "Updating $@"
	rm -f $@
	(cd doc/stars; \
	echo "set TYCHO $(PTOLEMY)/tycho; \
		source $(PTOLEMY)/tycho/lib/idx/tychoMakeIndex.tcl; \
		ptolemyStarHTMLIndex \
		\"\{General Ptolemy $(notdir $(ME)) stars\}\"" | itclsh)


starHTML.idx: subdomainstarHTML doc/stars/starHTML.idx
	@echo "Updating $@:"
	rm -f $@
	@if [ "$(SUBDOMAINDIRS)" != "" ]; then \
		subdirs="$(addsuffix /doc/stars/starHTML.idx, $(SUBDOMAINDIRS))"; \
		echo "Merging doc/stars/starHTML.idx $$subdirs ";\
		echo "set TYCHO $(PTOLEMY)/tycho; \
			source $(PTOLEMY)/tycho/lib/idx/tychoMakeIndex.tcl; \
			tychoMergeIndices \"All Ptolemy $(ME) stars\" $@ \
				$$subdirs \
				doc/stars/starHTML.idx" | itclsh; \
	else \
		echo "Merging only doc/stars/starHTML.idx"; \
		echo "set TYCHO $(PTOLEMY)/tycho; \
			source $(PTOLEMY)/tycho/lib/idx/tychoMakeIndex.tcl; \
			tychoMergeIndices \"All Ptolemy $(ME) stars\" $@ \
				doc/stars/starHTML.idx" | itclsh; \
	fi

SUBDOMAINDIRS = . 
subdomainstarHTML:
	@for x in $(SUBDOMAINDIRS); do \
	    if [ -w $$x/ ] ; then \
		( cd $$x ; \
		  echo making doc/stars/starHTML.idx in domains/$$x ; \
		  $(MAKE) $(MFLAGS) $(MAKEVARS) \
			VPATH=../../../src/domains/$$x 	\
			doc/stars/starHTML.idx ; \
		) \
	    fi ; \
	done


