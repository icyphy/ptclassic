# $Id$
#
# Makefile for the Almagest
#

PTOLEMY = ..

DIRS =	\
	$(PTOLEMY)/src/domains/cg56/doc \
	$(PTOLEMY)/src/domains/cg96/doc \
	$(PTOLEMY)/src/domains/cgc/doc \
	$(PTOLEMY)/src/domains/cg/doc \
	$(PTOLEMY)/src/domains/thor/doc \
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

almagest print:
	rm -f $$HOME/Almagest.index
	for x in $(DIRS); do \
		(cd $$x ; make print) ; \
	done; \
	make prtitle
	make prindex


prtitle:
	lpr title.ps; \

prindex:
	$(PTOLEMY)/doc/bin/prindex $(HOME)/Almagest.index
