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

# Tex files 
PAPERFILE = $(PAPER).tex
PAPERDEPEND = $(PAPERFILE) $(PAPER).bbl

# Parameters for tex-to-html converter
TEXTTOHTML = latex2html
HTMLDIR = www
L2HARGS = -address $(ADDRESS) -t $(TITLE) -dir $(HTMLDIR) -auto_navigation

# List of files to install
INSTALL_LIST = $(PAPER).dvi $(PAPER).pdf $(PAPER).ps.Z \
		$(HTMLDIR)/$(PAPER)/$(PAPER).html

