# Makefile included at the root of each domain (eg. SDF, DE, etc)
# $Id$
# Copyright (c) 1992 The Regents of the University of California.
#                       All Rights Reserved.
# Date of creation: 6/15/92.
# Written by: J. Buck

MAKEVARS = "ARCH=$(ARCH)"

makefiles:
	@for x in $(DIRS); do \
		( cd $$x ; \
		  echo Updating makefile in domains/$(ME)/$$x ; \
		  $(MAKE) -f make.template $(MFLAGS) $(MAKEVARS) \
			VPATH=../../../../src/domains/$(ME)/$$x $@ ; \
		) \
	done

all install clean sources realclean checkjunk sccsinfo:
	@for x in $(DIRS); do \
		( cd $$x ; \
		  echo making $@ in domains/$(ME)/$$x ; \
		  $(MAKE) $(MFLAGS) $(MAKEVARS) \
			VPATH=../../../../src/domains/$(ME)/$$x $@ ; \
		) \
	done

depend:
	@for x in $(DIRS); do \
		( cd $$x ; \
		  echo making $@ in domains/$(ME)/$$x ; \
		  $(MAKE) -f make.template $(MFLAGS) $(MAKEVARS) \
			VPATH=../../../../src/domains/$(ME)/$$x $@ ; \
		) \
	done
