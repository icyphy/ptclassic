# Makefile for all of Ptolemy source
#
# Copyright (c) 1990 The Regents of the University of California.
#                       All Rights Reserved.
#		       
# Programmer:  J. T. Buck
# $Id$
#
# Important!  Do not run this makefile while in ~ptolemy/src.  Instead
# run in from ~ptolemy/obj.$ARCH (through the symbolic link) -- that
# way a separate directory containing, say, Sun-3 and Sun-4 objects can be
# maintained.

DIRS = kernel preproc domains interp ptcl pigilib libgantt pigiRpc xgraph filters

# directories with a make.template
MTDIRS = kernel pigilib libgantt xgraph

# "leaf" directories with no make.template
LDIRS = preproc interp pigiRpc filters

# directories with subtrees
SDIRS = domains

MAKEVARS = "ARCH=$(ARCH)"

# This target makes sure all the makefiles are current
makefiles:
	@for x in $(MTDIRS); do \
		cd $$x ; \
		echo making makefile in $$x ; \
		$(MAKE) -f make.template $(MFLAGS) $(MAKEVARS) \
			VPATH=../../src/$$x makefile ;\
		cd .. ; \
	done

	@for x in $(SDIRS); do \
		cd $$x ; \
		echo making $@ in $$x ; \
		$(MAKE) $(MFLAGS) $(MAKEVARS) VPATH=../../src/$$x $@ ;\
		cd .. ; \
	done

all install clean realclean sources depend:
	@for x in $(DIRS); do \
		cd $$x ; \
		echo making $@ in $$x ; \
		$(MAKE) $(MFLAGS) $(MAKEVARS) VPATH=../../src/$$x $@ ;\
		cd .. ; \
	done
