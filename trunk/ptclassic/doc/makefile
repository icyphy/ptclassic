# $Id$
#
# Makefile for the Almagest
#

PTOLEMY       = ..

DIRS =	\
	
	$(PTOLEMY)/src/domains/cg/doc \
	$(PTOLEMY)/src/domains/cg56/doc \
	$(PTOLEMY)/src/domains/cg96/doc \
	$(PTOLEMY)/src/domains/cgc/doc \
	$(PTOLEMY)/src/domains/tho/doc \
	$(PTOLEMY)/src/domains/de/doc \
	$(PTOLEMY)/src/domains/ddf/doc \
	$(PTOLEMY)/src/domains/sdf/doc \
	$(PTOLEMY)/doc/main

# omitted from main distribution
#	$(PTOLEMY)/src/domains/cg-ddf/doc \
#	$(PTOLEMY)/src/domains/bdf/doc \
#	$(PTOLEMY)/src/domains/silage/doc \
#	$(PTOLEMY)/src/domains/sproc/doc \
#	$(PTOLEMY)/src/domains/vhdl/doc \
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
