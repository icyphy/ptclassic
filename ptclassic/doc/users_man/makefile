# Version $Id$
# Author:	T. M. Parks
# Created:	12 Jan 94
# Build PostScript files for chapters written with LaTeXinfo.


ROOT= ../..
include $(ROOT)/mk/latexinfo.mk

SRCS = \
	userMan.tex \
	other.tex \
	index.tex \
	../../src/domains/cgc/doc/CGCUser.tex

CLEAN = $(SRCS:.tex=.clean)

all:	userMan.ps userMan.master.info

clean:	$(CLEAN)

userMan.ps:		$(SRCS) userMan.cps
userMan.master.info:	$(SRCS)
