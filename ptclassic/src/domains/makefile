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

# The EXPDOMAINS environment variable can be used to build experimental domains
# which are assumed to follow the same directory structure.
DIRS = sdf de ddf thor cg cg-ddf cgc cg56 $(EXPDOMAINS)

MAKEVARS = "ARCH=$(ARCH)"

makefiles all install clean sources depend:
	@for x in $(DIRS); do \
		cd $$x ; \
		echo making $@ in domains/$$x ; \
		$(MAKE) $(MFLAGS) $(MAKEVARS) VPATH=../../../src/domains/$$x $@ ;\
		cd .. ; \
	done
