# Version $Id$
# Author:	T. M. Parks
# Created:	12 Jan 94
# Build PostScript files for chapters written with LaTeXinfo.


ROOT= ../..
include $(ROOT)/mk/latexinfo.mk

DOM=$(ROOT)/src/domains
MORE_TEXINPUTS=$(DOM)/cg/doc:$(DOM)/cgc/doc:$(DOM)/cp/doc:

SRCS = \
	userMan.tex \
	other.tex \
	index.tex \
	$(DOM)/cg/doc/CGUser.tex \
	$(DOM)/cgc/doc/CGCUser.tex \
	$(DOM)/cp/doc/CPUser.tex

CLEAN = $(SRCS:.tex=.clean)

all:	userMan.ps userMan.info

clean:	$(CLEAN)

userMan.ps:	$(SRCS) userMan.aux userMan.cps
userMan.info:	$(SRCS)
