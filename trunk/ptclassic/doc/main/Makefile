# $Id$
#
# Makefile for sect1 of the Almagest
#

PTOLEMY       = ../..
PRINTSEC      = $(PTOLEMY)/doc/bin/printsec
PRINTTITLE    = $(PTOLEMY)/doc/bin/printtitle
HEADER        = $(PTOLEMY)/doc/headers/together
MAINSEC	      = overall
TMPFILE	      = $$HOME/Almagest.main
OPTIONS       =

DIRS = overview pigi ptcl ptlang domains documents xgraph filters vem 

MAKEVARS =	\
		"PTOLEMY=$(PTOLEMY)" \
		"PRINTER=$(PRINTER)" \
		"PRINTSEC=$(PRINTSEC)" \
		"HEADER=$(HEADER)"

$(DIRS):
	cd $@; $(MAKE) $(MFLAGS) $(MAKEVARS) print

print:
	$(PRINTTITLE) $(OPTIONS) title
	rm -f $(TMPFILE)
	for x in $(DIRS); do \
		cd $$x ; \
		soelim $(MAINSEC) >> $(TMPFILE) ; \
		cd .. ; \
	done
	$(PRINTSEC) $(OPTIONS) $(HEADER) init $(TMPFILE)
	
	$(PRINTTITLE) $(OPTIONS) title
#	/bin/rm $(TMPFILE)

preview:
	rm -f $(TMPFILE)
	for x in $(DIRS); do \
		cd $$x ; \
		soelim $(MAINSEC) >> $(TMPFILE) ; \
		cd .. ; \
	done
	$(PRINTSEC) -preview $(HEADER) init $(TMPFILE)

# this does preview a second time
review:
	$(PRINTSEC) -preview $(HEADER) init $(TMPFILE)
