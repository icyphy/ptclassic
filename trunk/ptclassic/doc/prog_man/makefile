# Version $Id$
# Author:	T. M. Parks
# Created:	12 Jan 94
# Build PostScript files for chapters written with LaTeXinfo.

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

chapter1.dvi:		cgc.tex
cgc.dvi cgc.mml:	chapter1.dvi
	mv -f chapter.dvi cgc.dvi

chapter2.dvi:		cp.tex
cp.dvi cp.mml:		chapter2.dvi
	mv -f chapter.dvi cgc.dvi
