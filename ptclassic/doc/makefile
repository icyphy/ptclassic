# $Id$
#
# Makefile for the Almagest
#

PTOLEMY       = ..

DIRS =	\
	$(PTOLEMY)/src/domains/cg/doc \
	$(PTOLEMY)/src/domains/thor/doc \
	$(PTOLEMY)/src/domains/de/doc \
	$(PTOLEMY)/src/domains/ddf/doc \
	$(PTOLEMY)/src/domains/sdf/doc \
	$(PTOLEMY)/doc/main

# omitted from main distribution
#	$(PTOLEMY)/src/domains/mq/doc \
#	$(PTOLEMY)/src/domains/capsim/doc \

MAKEVARS =	\
		"PTOLEMY=$(PTOLEMY)" \
		"PRINTER=$(PRINTER)"

almagest print:
	rm -f $$HOME/Almagest.index
	for x in $(DIRS); do \
		cd $$x ; \
		make $(MAKEVARS) print ; \
	done; \
	cd $(PTOLEMY)/doc; \
	make $(MAKEVARS) prtitle
	make $(MAKEVARS) prindex


prtitle:
	lpr -P$(PRINTER) title.ps; \

prindex:
	cd $$HOME; $(PTOLEMY)/doc/bin/prindex Almagest.index
