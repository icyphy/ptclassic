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
VPATH =		$(ROOT)/src/domains

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

DOMAINS = sdf hof de ddf bdf cg cgc c50 cg56 vhdlb \
          vhdl mdsdf $(EXPDOMAINS) 

MAKEVARS = "PTARCH=$(PTARCH)"

makefiles all install clean depend realclean clean_indices checkjunk sccsinfo \
		weblint:
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
# Don't include domain.idx, it takes 30 minutes to rebuild
sources: subsources starHTML.idx #starDemo.idx domain.idx
subsources:
	@for x in $(DOMAINS); do \
	    if [ -w $$x ] ; then \
		( cd $$x ; \
		echo making sources in domains/$$x ; \
		$(MAKE) $(MFLAGS) $(MAKEVARS) VPATH=../../../src/domains/$$x sources ;\
		) \
	    fi ; \
	done

#####
# star index
# Remove thor, add starHTML.idx, add the VPATH.
ALLSTARHTMLIDXS =	$(addprefix $(VPATH)/, \
			$(addsuffix /starHTML.idx, \
				$(filter-out thor, $(DOMAINS))))

# Star index of all the star index files
starHTML.idx: domainstarHTML $(TYCHOMAKEINDEX)
	@echo "Merging $(VPATH)/$@"
	@rm -f $(VPATH)/$@
	@(cd $(VPATH); \
	 echo "set TYCHO $(PTOLEMY)/tycho; \
		source $(TYCHOMAKEINDEX); \
		tychoVpathMergeIndices {} $(VPATH)/$@ $(VPATH)/ \
			$(ALLSTARHTMLIDXS)" \
		| itclsh)

domainstarHTML:
	@for x in $(DOMAINS); do \
	    if [ -w $$x/ ] ; then \
		( cd $$x ; \
		  echo making starHTML.idx in domains/$$x ; \
		  $(MAKE) $(MFLAGS) $(MAKEVARS) \
			VPATH=../../../src/domains/$$x 	\
			starHTML.idx ; \
		) \
	    fi ; \
	done

#####
# star/demo xref index 
# Remove thor, add starHTML.idx, add the VPATH.
ALLDEMOIDXS =	$(addprefix $(VPATH)/, \
			$(addsuffix /starDemo.idx, \
				$(filter-out thor, $(DOMAINS))))

# Star index of all the star index files
starDemo.idx: domainstardemoidx $(TYCHOMAKEINDEX)
	@echo "Merging $(VPATH)/$@"
	@rm -f $(VPATH)/$@
	@(cd $(VPATH); \
	 echo "set TYCHO $(PTOLEMY)/tycho; \
		source $(TYCHOMAKEINDEX); \
		tychoVpathMergeIndices {} $(VPATH)/$@ $(VPATH)/ \
			$(ALLDEMOIDXS)" \
		| itclsh)

domainstardemoidx:
	@for x in $(DOMAINS); do \
	    if [ -w $$x/ ] ; then \
		( cd $$x ; \
		  echo making $@ in domains/$$x ; \
		  $(MAKE) $(MFLAGS) $(MAKEVARS) \
			VPATH=../../../src/domains/$$x 	\
			starDemo.idx ; \
		) \
	    fi ; \
	done

#####
# Master Index for all indices in a domain
# Remove thor, add starHTML.idx, add the VPATH.
# Remove thor, add starHTML.idx, add the VPATH.
ALLDOMAINIDXS =	$(addprefix $(VPATH)/, \
			$(addsuffix /domain.idx, \
				$(filter-out thor, $(DOMAINS))))

.PHONY:	TAGS domain.idx force

# Star index of all the star index files
domain.idx:  $(ALLDOMAINIDXS) $(TYCHOMAKEINDEX)
	@echo "Merging $(VPATH)/$@"
	@rm -f $(VPATH)/$@
	@(cd $(VPATH); \
	 echo "set TYCHO $(PTOLEMY)/tycho; \
		source $(TYCHOMAKEINDEX); \
		tychoVpathMergeIndices {} $(VPATH)/$@ $(VPATH)/ \
			$(ALLDOMAINIDXS)" \
		| itclsh)

$(ALLDOMAINIDXS): force
	@if [ -w `basename $(@D)` ] ; then \
		( cd `basename $(@D)` ; \
		  echo making $@ in domains/`basename $(@D)` ; \
		  $(MAKE) $(MFLAGS) $(MAKEVARS) \
			VPATH=../../../src/domains/`basename $(@D)` \
			domain.idx ; \
		) \
	fi	

#domainidx:
#	@for x in $(DOMAINS); do \
#	    if [ -w $$x/ ] ; then \
#		( cd $$x ; \
#		  echo making $@ in domains/$$x ; \
#		  $(MAKE) $(MFLAGS) $(MAKEVARS) \
#			VPATH=../../../src/domains/$$x 	\
#			domain.idx ; \
#		) \
#	    fi ; \
#	done

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

starindex: $(STARINDEX)
	-mkdir $(ROOT)/doc
	-mkdir $(ROOT)/doc/star-demo-index
	echo "source $(STARINDEX); \
		starindex_WriteWWWStarDemoDir \
			\"$(sort $(DOMAINS))\" \
			$(ROOT)/doc/star-demo-index" | itclsh
