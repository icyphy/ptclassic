# $Id$
#
# Makefile for sect1 of the Almagest
#

PTOLEMY       = /usr/users/ptolemy
PRINTER       = lw
PRINTSEC      = $(PTOLEMY)/doc/bin/printsec
HEADER        = $(PTOLEMY)/doc/headers/together
MAINSEC	      = overall
TMPFILE	      = tmp_print

DIRS = overview pigi interpreter ptlang domains
SECTIONS = overview/$(TMPFILE) pigi/$(TMPFILE) \
	interpreter/$(TMPFILE) ptlang/$(TMPFILE) \
	domains/$(TMPFILE)

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
	$(PRINTSEC) $(HEADER) init $(SECTIONS); \
	ditroff -me -eqn -P$(PRINTER) $(PTOLEMY)/doc/headers/shared title; \
	/bin/rm $(SECTIONS)
