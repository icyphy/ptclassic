# $Id$
#
# Makefile for changeIconMasters document
#

PTOLEMY       = /usr/users/ptolemy
PRINTSEC      = $(PTOLEMY)/doc/bin/printsec
PREVSEC       = $(PTOLEMY)/doc/bin/prevsec
HEADER        = $(PTOLEMY)/doc/headers/standalone

MAKEVARS =	\
		"PTOLEMY=$(PTOLEMY)" \
		"PRINTER=$(PRINTER)" \
		"PRINTSEC=$(PRINTSEC)" \
		"HEADER=$(HEADER)"

print:
	 $(PRINTSEC) $(HEADER) overall

preview:
	$(PREVSEC) $(HEADER) overall

