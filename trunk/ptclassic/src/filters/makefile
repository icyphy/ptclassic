# $Id$
#
# Ptolemy src Makefile
#

DIRS = wfir optfir
PTOLEMY = ../..

MAKEVARS =	\
		"ARCH=$(ARCH)" 

all install clean check sources:
	 @for x in $(DIRS); do \
	 	cd $$x ; \
		echo making $@ in filters/$$x ; \
		$(MAKE) $(MFLAGS) $(MAKEVARS) PTOLEMY=../../.. VPATH=../../../src/filters/$$x $@ ;\
	  	cd .. ; \
	 done

sccsinfo:
		sccs info
