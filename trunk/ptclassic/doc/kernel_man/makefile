# Version $Id$ $Revision$
ROOT= ../..
include $(ROOT)/mk/latexinfo.mk

SRC=	kernel.tex codeGen.tex

clean:	ptk.clean
	rm -f ptk.dvi $(SRC:.tex=.aux)

all:		ptk.ps ptk.info clean

ptk.info:	ptk.tex $(SRC)

ptk.dvi:	ptk.tex $(SRC) ptk.toc
