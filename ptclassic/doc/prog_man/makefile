# Version $Id$
# Author:	T. M. Parks
# Created:	12 Jan 94
# Build PostScript files for chapters written with LaTeXinfo.


ROOT= ../..
include $(ROOT)/mk/latexinfo.mk

SRCS = \
	progMan.tex \
	start.tex \
	../../src/domains/cgc/doc/CGCProg.tex

CLEAN = $(SRCS:.tex=.clean)

all:	progMan.ps progMan.info

clean:	$(CLEAN)

progMan.ps:	$(SRCS) progMan.aux
progMan.info:	$(SRCS)
