# Version $Id$
# Author:	T. M. Parks
# Created:	12 Jan 94
# Build PostScript files for chapters written with LaTeXinfo.


ROOT= ../..
include $(ROOT)/mk/latexinfo.mk

SRCS =	userMan.tex \
	other.tex \
	cg.tex \
	cgc.tex \
	cp.tex

CLEAN = $(SRCS:.tex=.clean) index.clean

all:	userMan.ps userMan.info

clean:	$(CLEAN)

userMan.dvi:	$(SRCS) userMan.toc userMan.cps
userMan.info:	$(SRCS)

# Update all of the Framemaker cross references.
update_book: update.fmb
	fmbatch update.fmb

.PHONY:	update_book
update.fmb:
	echo "Open users_man.book" > $@
	echo "Update users_man.book" >> $@
	chmod g+w $@

# Update all the cross references and then Create postscript files
fm2ps: update_book
	fmprint2ps *.doc
