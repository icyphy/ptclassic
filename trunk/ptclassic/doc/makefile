# $Id$
#
# Makefile for the Almagest
#

PTOLEMY       = /usr/users/ptolemy
PRINTER       = lw

DIRS =	$(PTOLEMY)/src/domains/mq/doc \
	$(PTOLEMY)/src/domains/capsim/doc \
	$(PTOLEMY)/src/domains/de/doc \
	$(PTOLEMY)/src/domains/ddf/doc \
	$(PTOLEMY)/src/domains/sdf/doc \
	$(PTOLEMY)/doc/main

MAKEVARS =	\
		"PTOLEMY=$(PTOLEMY)" \
		"PRINTER=$(PRINTER)"

almagest print:
	for x in $(DIRS); do \
		cd $$x ; \
		make print ; \
	done; \
	make prtitle; \
	make prindex

prtitle:
	cd $(PTOLEMY)/doc; \
	ditroff -me -eqn -P$(PRINTER) $(PTOLEMY)/doc/headers/shared title; \

prindex:
	/bin/rm -f $(PTOLEMY)/doc/index; \
	for x in $(DIRS); do \
		cd $$x ; \
		cat index >> $(PTOLEMY)/doc/index; \
		chmod g+w $(PTOLEMY)/doc/index; \
		cd .. ; \
	done; \
	cd $(PTOLEMY)/doc; \
	$(PTOLEMY)/doc/bin/prindex index
