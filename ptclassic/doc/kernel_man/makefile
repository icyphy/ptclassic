# Version $Id$ $Revision$
ROOT= ../..
LATEXINFO= /usr/tools/gnu/src/latexinfo
TEXINPUTS= .:$(ROOT)/doc/tex:$(LATEXINFO)/styles:

export LATEXINFO
export TEXINPUTS

INFO=	-l $(ROOT)/doc/elisp/default.el -l $(LATEXINFO)/.emacs \
	-l latexinfo -l latexnfo-mde

all:	ptk.dvi ptk.info ptk.ps.Z

# Update all nodes and menus.
%.update:	%.tex
	emacs -batch $(INFO) $< \
		-f latexinfo-every-node-update \
		-f latexinfo-all-menus-update \
		-f save-buffer -kill

# Generate an info file from the LaTeXinfo source.
ptk.info:	ptk.tex kernel.tex
	emacs -batch $(INFO) ptk.tex \
		-f latexinfo-format-buffer \
		-f save-buffer -kill

# Run LaTeX twice to resolve cross references.
ptk.dvi:	ptk.tex kernel.tex
		latex ptk
		latex ptk
		rm kernel.aux ptk.aux ptk.toc ptk.log

chapter.dvi:	chapter.tex kernel.tex
		latex chapter
		latex chapter
		rm chapter.aux chapter.log

ptk.ps.Z:	ptk.dvi
		dvips -f ptk.dvi | compress > ptk.ps.Z

