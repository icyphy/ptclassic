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

# The EXPDOMAINS environment variable can be used to build experimental domains
# which are assumed to follow the same directory structure.

# Include CP and PN domains on SunOS architectures.
ifneq (,$(filter sun%,$(PTARCH)))
EXPDOMAINS = cp pn
endif

# Include CP domain for cfront.
ifeq (cfront,$(PTARCH))
EXPDOMAINS = cp
endif

# Include PN doman for mips and Solaris.
ifneq (,$(filter mips sol%,$(PTARCH)))
EXPDOMAINS = pn
endif

# Don't include IPUS if your are running a non GNU c++
ifneq (, $(filter %cfront,$(PTARCH)))
IPUSDOMAIN=
else
IPUSDOMAIN= ipus
endif

DOMAINS = sdf hof de ddf bdf thor cg sproc silage cgc cg56 cg96 vhdlf vhdlb \
          mq mdsdf $(IPUSDOMAIN) $(EXPDOMAINS) 

MAKEVARS = "PTARCH=$(PTARCH)"

makefiles all install clean sources depend realclean checkjunk sccsinfo:
	@for x in $(DOMAINS); do \
	    if [ -w $$x ] ; then \
		( cd $$x ; \
		echo making $@ in domains/$$x ; \
		$(MAKE) $(MFLAGS) $(MAKEVARS) VPATH=../../../src/domains/$$x $@ ;\
		) \
	    fi ; \
	done

.PHONY:	TAGS
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
