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

ifneq (,$(filter sun%,$(ARCH)))
DIRS = Awesime awe lwp
endif

ifeq (cfront,$(ARCH))
DIRS = lwp
endif

ifeq (mips,$(ARCH))
DIRS = Awesime awe
endif

MAKEVARS = "ARCH=$(ARCH)"

makefiles all install clean sources depend realclean checkjunk sccsinfo:
ifneq (,$(DIRS)) 
	@for x in $(DIRS); do \
	    if [ -w $$x ] ; then \
		( cd $$x ; \
		  echo making $@ in thread/$$x ; \
		  $(MAKE) $(MFLAGS) $(MAKEVARS) \
		      VPATH=../../../src/thread/$$x $@ ; \
		) \
	    fi ; \
	done
else
	@echo "No need to do anything in thread directory for this ARCH."
endif
