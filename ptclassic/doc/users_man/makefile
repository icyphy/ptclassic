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

cgc.dvi:	cgc.tex chapter.tex other.tex
	$(ROOT)/doc/tex/mkchap $(TEX) cgc front,other 10
	$(ROOT)/doc/tex/mkchap $(TEX) cgc null 10
	mv -f chapter.dvi cgc.dvi

cgc.contents:	chapter.toc
	$(ROOT)/doc/tex/mktoc chapter 10 > cgc.contents

cgc.index:	chapter.cp chapter.tp chapter.fn
	$(ROOT)/doc/tex/mkindex chapter 10 > cgc.index

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
