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
