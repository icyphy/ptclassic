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
	main

MAKEVARS =	\
		"PTOLEMY=$(PTOLEMY)" \
		"PRINTER=$(PRINTER)"

$(DIRS):
	cd $@; make print

almagest print:
	for x in $(DIRS); do \
		cd $$x ; \
		make print ; \
		cd .. ; \
	done; \
	ditroff -me -eqn -P$(PRINTER) $(PTOLEMY)/doc/headers/shared title; \
	make index

index:
	/bin/rm $(PTOLEMY)/doc/index; \
	for x in $(DIRS); do \
		cd $$x ; \
		cat index >> $(PTOLEMY)/doc/index; \
		chmod g+w $(PTOLEMY)/doc/index; \
		cd .. ; \
	done; \
	$(PTOLEMY)/doc/bin/prindex index
