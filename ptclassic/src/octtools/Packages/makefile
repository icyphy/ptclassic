# Version Identification:
# $Id$
# Makefile for octtools Packages.  Used to compile vem
# Version identification:
# @(#)makefile	
#
# Copyright (c) 1990,1993 The Regents of the University of California.
#                       All Rights Reserved.
#		       
# Important!  Do not run this makefile while in ~ptolemy/src/vem/Packages.
# Instead run in from ~ptolemy/obj.$ARCH (through the symbolic link) -- that
# way a separate directory containing, say, Sun-3 and Sun-4 objects can be
# maintained.

DIRS = avl cp da errtrap fang fc gu harpoon kd lel list mm nle oct octmm \
	oh options pep port pq region st symbolic symlib tap th timer \
	tr uprintf utility vov vulcan


# "leaf" directories with a make.template
MTDIRS = avl cp da errtrap fang fc gu harpoon kd lel list mm nle oct octmm \
	oh options pep port pq region st symbolic symlib tap th timer \
	tr uprintf utility vov vulcan

# "leaf" directories with no make.template
LDIRS = 

# directories with subtrees
SDIRS = 

MAKEVARS = "ARCH=$(ARCH)"

# This target makes sure all the makefiles are current
makefiles:
	@for x in $(MTDIRS); do \
		cd $$x ; \
		echo making makefile in $$x ; \
		$(MAKE) -f make.template $(MFLAGS) $(MAKEVARS) \
			VPATH=../../../../src/vem/Packages/$$x makefile ; \
		cd .. ; \
	done

all install clean realclean sources depend checkjunk sccsinfo header:
	@for x in $(DIRS); do \
		cd $$x ; \
		echo making $@ in $$x ; \
		$(MAKE) $(MFLAGS) $(MAKEVARS) VPATH=../../../../src/vem/Packages/$$x $@ ;\
		cd .. ; \
	done

