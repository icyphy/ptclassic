# $Id$
#
# Makefile for sect1 of the Almagest
#

PTOLEMY       = ../..
DOCBIN	      = $(PTOLEMY)/doc/bin
PRINTSEC      = printsec
PRINTTITLE    = printtitle
MAINSEC	      = overall
TMPFILE	      = $$HOME/Almagest.main
# The following may need to be gsoelim with groff
SOELIM	      = soelim
OPTIONS       =

DIRS = overview pigi ptcl ptlang domains documents xgraph filters vem 

MAKEVARS =	\
		"DOCBIN=$(DOCBIN)" \
		"PTOLEMY=$(PTOLEMY)" \
		"PRINTER=$(PRINTER)" \
		"PRINTSEC=$(PRINTSEC)" \
		"HEADER=$(HEADER)"

$(DIRS):
	cd $@; $(MAKE) $(MFLAGS) $(MAKEVARS) print

print:
	rm -f $(TMPFILE)
	for x in $(DIRS); do \
		cd $$x ; \
		$(SOELIM) $(MAINSEC) >> $(TMPFILE) ; \
		cd .. ; \
	done
	$(DOCBIN)/$(PRINTSEC) $(OPTIONS) -H together init $(TMPFILE)
	
	$(DOCBIN)/$(PRINTTITLE) $(OPTIONS) title
#	/bin/rm $(TMPFILE)

preview:
	rm -f $(TMPFILE)
	for x in $(DIRS); do \
		cd $$x ; \
		$(SOELIM) $(MAINSEC) >> $(TMPFILE) ; \
		cd .. ; \
	done
	$(DOCBIN)/$(PRINTSEC) $(OPTIONS) -preview -H together init $(TMPFILE)

# this does preview a second time
review:
	$(DOCBIN)/$(PRINTSEC) $(OPTIONS) -preview -H together init $(TMPFILE)

