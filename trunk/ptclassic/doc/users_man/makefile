# Version $Id$
# Author:	T. M. Parks
# Created:	12 Jan 94
# Build PostScript files for chapters written with LaTeXinfo.


ROOT= ../..
include $(ROOT)/mk/latexinfo.mk

CLEAN = chapter.clean

all:	cgc.ps cgc.contents cgc.index

clean:	$(CLEAN)
	rm -f front.aux other.aux null.aux 
	rm -f cgc.aux cgc.dvi

# Pattern rule for formatting a single chapter.
chapter%.dvi:
	$(ROOT)/doc/tex/mkchap $(TEX) $(<:.tex=) front,other $*
	$(ROOT)/doc/tex/mkchap $(TEX) $(<:.tex=) null $*
	$(ROOT)/doc/tex/mktoc chapter $* > $(<:.tex=.contents)
	$(ROOT)/doc/tex/mkindex chapter $* > $(<:.tex=.index)

chapter13.dvi:	cgc.tex
cgc.dvi:	chapter13.dvi
		mv -f chapter.dvi cgc.dvi

chapter11.dvi:	cp.tex
cp.dvi:		chapter11.dvi
		mv -f chapter.dvi cp.dvi

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
