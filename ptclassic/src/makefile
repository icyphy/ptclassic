# Makefile for all of Ptolemy source
#
# Copyright (c) 1990-1994 The Regents of the University of California.
#                       All Rights Reserved.
#		       
# Programmer:  J. T. Buck
# $Id$
#
# Important!  Do not run this makefile while in ~ptolemy/src.  Instead
# run in from ~ptolemy/obj.$ARCH (through the symbolic link) -- that
# way a separate directory containing, say, Sun-3 and Sun-4 objects can be
# maintained.

DIRS = kernel thread ptlang domains ptcl pigilib libgantt ptklib pigiRpc pxgraph filters utils

# "leaf" directories with a make.template
MTDIRS = kernel pigilib libgantt pxgraph ptcl ptklib

# "leaf" directories with no make.template
LDIRS = ptlang pigiRpc filters utils

# directories with subtrees
SDIRS = thread domains

MAKEVARS = "ARCH=$(ARCH)"

# This target makes sure all the makefiles are current
makefiles:
	@for x in $(MTDIRS); do \
	    if [ -w $$x ] ; then \
		( cd $$x ; \
		echo making makefile in $$x ; \
		$(MAKE) -f make.template $(MFLAGS) $(MAKEVARS) \
			VPATH=../../src/$$x makefile ;\
		) \
	    fi ; \
	done

	@for x in $(SDIRS); do \
	    if [ -w $$x ] ; then \
		( cd $$x ; \
		echo making $@ in $$x ; \
		$(MAKE) $(MFLAGS) $(MAKEVARS) VPATH=../../src/$$x $@ ;\
		) \
	    fi ; \
	done

all install clean realclean sources depend checkjunk sccsinfo:
	@for x in $(DIRS); do \
	    if [ -w $$x ] ; then \
		( cd $$x ; \
		echo making $@ in $$x ; \
		$(MAKE) $(MFLAGS) $(MAKEVARS) VPATH=../../src/$$x $@ ;\
		) \
	    fi ; \
	done

TAGS:
	@for x in $(DIRS); do \
	    if [ -w $$x ] ; then \
		( cd $$x ; \
		echo making $@ in $$x ; \
		$(MAKE) $(MFLAGS) $(MAKEVARS) VPATH=../../src/$$x $@ ; \
		) \
	    fi ; \
	done
	-cat -s $(DIRS:%=%/TAGS) > $@

