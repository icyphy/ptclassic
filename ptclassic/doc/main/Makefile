# $Id$
#
# Makefile for sect1 of the Almagest
#

PTOLEMY       = /usr/users/ptolemy
PRINTSEC      = $(PTOLEMY)/doc/bin/printsec
HEADER        = $(PTOLEMY)/doc/headers/together
MAINSEC	      = overall
TMPFILE	      = tmp_print
OPTIONS       =

DIRS = overview pigi interpreter ptlang domains documents xgraph filters
SECTIONS = overview/$(TMPFILE) pigi/$(TMPFILE) \
	interpreter/$(TMPFILE) ptlang/$(TMPFILE) \
	domains/$(TMPFILE) documents/$(TMPFILE) \
	xgraph/$(TMPFILE) filters/$(TMPFILE)

MAKEVARS =	\
		"PTOLEMY=$(PTOLEMY)" \
		"PRINTER=$(PRINTER)" \
		"PRINTSEC=$(PRINTSEC)" \
		"HEADER=$(HEADER)"

$(DIRS):
	cd $@; $(MAKE) $(MFLAGS) $(MAKEVARS) print

print:
	for x in $(DIRS); do \
		cd $$x ; \
		soelim $(MAINSEC) > $(TMPFILE) ; \
		cd .. ; \
	done ; \
	$(PRINTSEC) $(OPTIONS) $(HEADER) init $(SECTIONS); \
	ditroff -me -eqn -P$(PRINTER) $(PTOLEMY)/doc/headers/shared title; \
	/bin/rm $(SECTIONS)
