# $Id$
#
# Makefile to process a LaTeX file into dvi, Postscript, and HTML formats
#  
# Before inclusion, the master makefile should set the following:
#
# PAPER = paper
# PAPERBIB = paper.bib
# TITLE = title
# ADDRESS = address
# PAPERDEPEND += <other_dependencies>
#

# Rule to convert between .tex and .dvi
.tex.dvi:
	latex $<
	latex $<

$(PAPER).dvi: $(PAPERDEPEND)

$(PAPER).bbl:	$(PAPERBIB)
	latex $(PAPER)
	bibtex $(PAPER)

# Convert to HTML and make gif image backgrounds transparent
# The palatino.sty file is necessary for good-looking pdf files
# but causes latex2html to fail, so we work around the problem
$(HTMLDIR)/$(PAPER)/$(PAPER).html:	$(PAPERDEPEND)
	-mkdir -p $(HTMLDIR)
	-mv palatino.sty palatino.$$$$.sty
	echo '\typeout{Stub palatino.sty file for latex2html compatibility}' >palatino.sty
	$(TEXTTOHTML) $(L2HARGS) $(PAPERFILE)
	-rm palatino.sty
	-mv palatino.$$$$.sty palatino.sty
	-convertAllGif $(HTMLDIR)/$(PAPER)/*.gif
	-cd $(HTMLDIR)/$(PAPER); ln -s $(PAPER).html index.html

$(PAPER).ps:	$(PAPER).dvi
	dvips -o $(PAPER).ps $(PAPER).dvi

$(PAPER).ps.Z:	$(PAPER).ps
	-rm $(PAPER).ps.Z
	compress -c $(PAPER).ps >$(PAPER).ps.Z

$(PAPER).pdf:	$(PAPER).dvi $(PAPER).ps
	-acrodistill $(PAPER).ps

install: $(INSTALL_LIST)
