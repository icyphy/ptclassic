# Top-level Ptolemy makefile.
# Copyright (c) 1990 The Regents of the University of California.
#                       All Rights Reserved.
#		       
# Programmer:  J. T. Buck
#
# The ARCH environment variable must be set.
# $Id$

defaultTarget:		install
	@echo Installation for $(ARCH) is complete.

makefiles:		obj.$(ARCH)
	@echo Updating makefiles.  Ignore messages about needing to rerun make --
	@echo the top-level makefile will make sure it is done right.
	cd obj.$(ARCH); $(MAKE) $(MFLAGS) makefiles

all install clean sources:	obj.$(ARCH)
	cd obj.$(ARCH); umask 2; $(MAKE) $(MFLAGS) makefiles $@

obj.$(ARCH):		MAKEARCH
	csh -f MAKEARCH

interpreter:	obj.$(ARCH)
		cd obj.$(ARCH); umask 2; $(MAKE) $(MFLAGS) \
			DIRS="kernel preproc domains interp" install

pigi:		obj.$(ARCH)
		cd obj.$(ARCH); umask 2; $(MAKE) $(MFLAGS) \
			DIRS="kernel preproc domains pigiRpc" install
