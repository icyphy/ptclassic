# Copyright (C) 1994 The Regents of the University of California.
#                       All Rights Reserved.
#		       
# Author:	T. M. Parks
# $Id$
#
# Important!  Do not run this makefile while in ~ptolemy/src/thread.  Instead
# run in from ~ptolemy/obj.$ARCH/thread (through the symbolic link) -- that
# way a separate directory containing, say, Sun-3 and Sun-4 objects can be
# maintained.

DIRS =

ifeq ($(ARCH),sun4)
DIRS = Awesime awe lwp
endif

ifeq ($(ARCH),mips)
DIRS = Awesime awe
endif

MAKEVARS = "ARCH=$(ARCH)"

makefiles all install clean sources depend realclean checkjunk sccsinfo:
	@for x in $(DIRS); do \
	    if [ -w $$x ] ; then \
		( cd $$x ; \
		  echo making $@ in thread/$$x ; \
		  $(MAKE) $(MFLAGS) $(MAKEVARS) \
		      VPATH=../../../src/thread/$$x $@ ;\
		) \
	    fi ; \
	done
