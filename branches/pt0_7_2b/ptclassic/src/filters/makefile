# $Id$
#
# Ptolemy src Makefile
#

DIRS = wfir optfir

MAKEVARS =	\
		"PTOLEMY=$(PTOLEMY)" \
		"ARCH=$(ARCH)" 

all install clean check:
	 for x in $(DIRS); do \
	 	cd $$x ; \
		$(MAKE) $(MFLAGS) $(MAKEVARS) VPATH=../../src/filters/$$x $@ ;\
	  	cd .. ; \
	 done
