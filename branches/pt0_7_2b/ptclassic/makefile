# Top-level Ptolemy makefile.
# Copyright (c) 1990 The Regents of the University of California.
#                       All Rights Reserved.
#		       
# Programmer:  J. T. Buck
# $Id$

all install clean:	obj.$(ARCH)
			cd obj.$(ARCH); $(MAKE) $(MFLAGS) makefiles $@

obj.$(ARCH):
	csh -f MAKEARCH
