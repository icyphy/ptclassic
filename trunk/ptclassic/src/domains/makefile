# Makefile for all domains subdirectories
# Copyright (c) 1990 The Regents of the University of California.
#                       All Rights Reserved.
#		       
# Programmer:  J. T. Buck
# $Id$
#
# Important!  Do not run this makefile while in ~ptolemy/src/domains.  Instead
# run in from ~ptolemy/obj.$ARCH/domains (through the symbolic link) -- that
# way a separate directory containing, say, Sun-3 and Sun-4 objects can be
# maintained.


DIRS = sdf de ddf

MAKEVARS = "ARCH=$(ARCH)"

makefiles:
	@for x in $(DIRS); do \
		cd $$x ; \
		echo Updating makefiles in domains/$$x ; \
		$(MAKE) $(MFLAGS) $(MAKEVARS) VPATH=../../../src/domains/$$x makefiles ;\
		cd .. ; \
	done

all install clean:
	@for x in $(DIRS); do \
		cd $$x ; \
		echo making $@ in domains/$$x ; \
		$(MAKE) $(MFLAGS) $(MAKEVARS) VPATH=../../../src/domains/$$x $@ ;\
		cd .. ; \
	done
