# Makefile for CG domain support
# Copyright (c) 1990 The Regents of the University of California.
#                       All Rights Reserved.
#		       
# Programmer:  J. T. Buck
# $Id$
#
# Important!  Do not run this makefile while in ~ptolemy/src/domains/cg.
# Instead run in from ~ptolemy/obj.$ARCH/domains/cg (through the symbolic
# link) -- that way a separate directory containing, say, Sun-3 and Sun-4
# objects can be maintained.


DIRS = kernel stars targets

ME = cg56

MAKEVARS = "ARCH=$(ARCH)"

makefiles:
	@for x in $(DIRS); do \
		cd $$x ; \
		echo Updating makefile in domains/$(ME)/$$x ; \
		$(MAKE) -f make.template $(MFLAGS) $(MAKEVARS) \
			VPATH=../../../../src/domains/$(ME)/$$x makefile ;\
		cd .. ; \
	done

all install clean sources depend:
	@for x in $(DIRS); do \
		cd $$x ; \
		echo making $@ in domains/$(ME)/$$x ; \
		$(MAKE) $(MFLAGS) $(MAKEVARS) \
			VPATH=../../../../src/domains/$(ME)/$$x $@ ;\
		cd .. ; \
	done
