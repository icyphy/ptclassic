# Version $Id$
# Authors:	T. M. Parks, Christopher Hylands
# Created:	12 Jan 94
# Build PostScript files for chapters written with LaTeXinfo.
# Convert Framemaker files to postscript

ROOT= ../..
include $(ROOT)/mk/latexinfo.mk

CLEAN = chapter.clean

all:	cgc.ps cp.ps

clean:	$(CLEAN)
	rm -f front.aux other.aux null.aux
	rm -f cgc.aux cgc.dvi
	rm -f cp.aux cp.dvi

# Pattern rule for formatting a single chapter.
chapter%.dvi:
	$(ROOT)/doc/tex/mkchap $(TEX) $(<:.tex=) front,other $*
	$(ROOT)/doc/tex/mkchap $(TEX) $(<:.tex=) null $*
	$(ROOT)/doc/fmconvert/tex2mml chapter $* > $(<:.tex=.mml)

chapter14.dvi:		cgc.tex
cgc.dvi cgc.mml:	chapter14.dvi
	mv -f chapter.dvi cgc.dvi

chapter12.dvi:		cp.tex
cp.dvi cp.mml:		chapter12.dvi
	mv -f chapter.dvi cp.dvi

# Update all of the Framemaker cross references.
update_book: update.fmb
	fmbatch update.fmb

.PHONY:	update_book
update.fmb:
	echo "Open prog_man.book" > $@
	echo "Update prog_man.book" >> $@
	chmod g+w $@

# Update all the cross references and then Create postscript files
# Script to fix up cg56.ps and cg96.ps postscript and to allow printing
# on printers w/o the exact size of paper (A4 vs European)
FIXFRAMEPS=/users/ptdesign/bin/fixframeps
fm2ps: update_book
	fmprint2ps *.doc
	$(FIXFRAMEPS) *.ps
	-chmod g+w *.ps
