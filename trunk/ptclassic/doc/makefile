# $Id$
#
# Makefile for the Almagest
#

PTOLEMY       = /usr/users/ptolemy

DIRS =	\
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
	for x in $(DIRS); do \
		cd $$x ; \
		make $(MAKEVARS) print ; \
	done; \
	cd $(PTOLEMY)/doc; \
	make $(MAKEVARS) prtitle; \
	make $(MAKEVARS) prindex

prtitle:
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
