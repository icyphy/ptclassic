# Makefile for all domains subdirectories
# Copyright (c) 1990-%Q% The Regents of the University of California.
#                       All Rights Reserved.
#		       
# Programmer:  J. T. Buck
# $Id$
#
# Important!  Do not run this makefile while in ~ptolemy/src/domains.  Instead
# run in from ~ptolemy/obj.$PTARCH/domains (through the symbolic link) -- that
# way a separate directory containing, say, Sun-3 and Sun-4 objects can be
# maintained.

# We need to see if $(INCLUDE_IPUS_DOMAIN) is set
# Root of Ptolemy directory
ROOT =		../..

# get configuration info
CONFIG =	$(ROOT)/mk/config-$(PTARCH).mk
include $(CONFIG)

# The EXPDOMAINS environment variable can be used to build experimental domains
# which are assumed to follow the same directory structure.
EXPDOMAINS = sr fsm

ifeq ($(INCLUDE_PN_DOMAIN),yes)
EXPDOMAINS += pn
endif

# Don't include IPUS if your are running a non GNU c++
ifeq ($(INCLUDE_IPUS_DOMAIN),yes)
EXPDOMAINS += ipus
endif

DOMAINS = sdf hof de ddf bdf thor cg cgc c50 cg56 vhdlb \
          vhdl mdsdf $(EXPDOMAINS) 

MAKEVARS = "PTARCH=$(PTARCH)"

makefiles all install clean depend realclean checkjunk sccsinfo:
	@for x in $(DOMAINS); do \
	    if [ -w $$x ] ; then \
		( cd $$x ; \
		echo making $@ in domains/$$x ; \
		$(MAKE) $(MFLAGS) $(MAKEVARS) VPATH=../../../src/domains/$$x $@ ;\
		) \
	    fi ; \
	done

# We have a separate target for sources so that we can build starHTML.idx
# after ptlang has been run on the star .pl files
sources: subsources starHTML.idx
subsources:
	@for x in $(DOMAINS); do \
	    if [ -w $$x ] ; then \
		( cd $$x ; \
		echo making sources in domains/$$x ; \
		$(MAKE) $(MFLAGS) $(MAKEVARS) VPATH=../../../src/domains/$$x sources ;\
		) \
	    fi ; \
	done

starHTML.idx: $(addsuffix /starHTML.idx, $(DOMAINS))
	@echo "Updating $@:"
	rm -f $@
	echo "set TYCHO $(PTOLEMY)/tycho; \
		source $(PTOLEMY)/tycho/lib/idx/tychoMakeIndex.tcl; \
		tychoMergeIndices \"All Ptolemy $(ME) stars\" $@ \
			$(addsuffix /starHTML.idx, $(DOMAINS)) \
			" | itclsh

.PHONY:	TAGS
TAGS:
	@for x in $(DOMAINS); do \
	    if [ -w $$x ] ; then \
		( cd $$x ; \
		echo making $@ in domains/$$x ; \
		$(MAKE) $(MFLAGS) $(MAKEVARS) \
			VPATH=../../../src/domains/$$x $@ ; \
		) \
	    fi ; \
	done
	-cat -s $(DOMAINS:%=%/TAGS) > $@

starindex:
	-mkdir $(ROOT)/doc
	-mkdir $(ROOT)/doc/star-demo-index
	mkStarIndex "$(sort $(DOMAINS))" $(ROOT)/doc/star-demo-index

