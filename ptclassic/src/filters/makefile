# $Id$
#
# Ptolemy src Makefile
#

DIRS = wfir optfir
PTOLEMY = ../..

MAKEVARS =	\
		"ARCH=$(ARCH)" 

all install clean check:
	 for x in $(DIRS); do \
	 	cd $$x ; \
		$(MAKE) $(MFLAGS) $(MAKEVARS) PTOLEMY=../../.. VPATH=../../../src/filters/$$x $@ ;\
	  	cd .. ; \
	 done
