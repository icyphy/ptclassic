# Version $Id$ $Revision$
ROOT= ../..
LATEXINFO= /usr/tools/gnu/src/latexinfo
TEXINPUTS= .:$(ROOT)/doc/tex:$(LATEXINFO)/styles:

export LATEXINFO
export TEXINPUTS

INFO=	-l $(ROOT)/doc/elisp/default.el -l $(LATEXINFO)/.emacs \
	-l latexinfo -l latexnfo-mde

all:	ptk.info ptk.dvi ptk.ps.Z chapter.dvi chapter.ps.Z

# Update all nodes and menus.
%.update:	%.tex
	emacs -batch $(INFO) $< \
		-f latexinfo-every-node-update \
		-f latexinfo-all-menus-update \
		-f save-buffer -kill

# Generate an info file from the LaTeXinfo source.
%.info:		%.tex
	emacs -batch $(INFO) $< \
		-f latexinfo-format-buffer \
		-f save-buffer -kill

# Run LaTeX twice to resolve cross references.
%.dvi:		%.tex
		latex $<
		latex $<
		rm -f *.aux *.log *.toc

%.ps.Z:		%.dvi
		dvips -f $< | compress > $@

SRC=	kernel.tex title.tex copyright.tex

ptk.info:	ptk.tex $(SRC)

ptk.dvi:	ptk.tex $(SRC)

chapter.dvi:	chapter.tex $(SRC)
