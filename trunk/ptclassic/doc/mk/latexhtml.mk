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

# Tex files 
PAPERFILE = $(PAPER).tex
PAPERDEPEND = $(PAPERFILE) $(PAPER).bbl

# Parameters for tex-to-html converter
TEXTTOHTML = latex2html
HTMLDIR = www
L2HARGS = -address $(ADDRESS) -t $(TITLE) -dir $(HTMLDIR) -auto_navigation

# Rule to convert between .tex and .dvi
.tex.dvi:
	latex $<
	latex $<

$(PAPER).dvi: $(PAPERDEPEND)

$(PAPER).bbl:	$(PAPERBIB)
	latex $(PAPER)
	bibtex $(PAPER)

# Convert to HTML and make gif image backgrounds transparent
$(HTMLDIR)/$(PAPER):	$(PAPERDEPEND)
	$(TEXTTOHTML) $(L2HARGS) $(PAPERFILE)
	convertAllGif $(HTMLDIR)/$(PAPER)/*.gif
	-cd $(HTMLDIR)/$(PAPER); ln -s $(PAPER).html index.html

$(PAPER).ps:	$(PAPER).dvi
	dvips -o $(PAPER).ps $(PAPER).dvi

$(PAPER).ps.Z:	$(PAPER).ps
	-rm $(PAPER).ps.Z
	compress $(PAPER).ps

install: $(PAPER).dvi $(PAPER).ps.Z $(HTMLDIR)/$(PAPER)
