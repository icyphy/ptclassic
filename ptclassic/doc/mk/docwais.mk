# Version $Id$
# Author	Christopher Hylands
# Created:	16 Jun 95
# Create html from framemaker

# Wpublish overview:
# users_man.wdt is the Quadralay wpublish template file for the user's manual
# Things to fix:
# title.doc.html, references.doc: get rid of gifs at the top
# users_manTOC.doc.html, users_manIX.doc.html: change <a href="#"> to
#  </a> everywhere. These seem to happen because quadralay can't handle
#  the extra index markers we use.  There could also be a problem with
#  nested html, where if the marker is supposed to be in bold, we get hosed.
#  It would also be a good idea to add a line to the top of these
#  two files that mention that they could have problems
# users_manTOC.doc: at the bottom, make sure that the references and index
#  are visible
# pigi.doc.html: get rid of table in the front
# Some of the links in the index and toc don't work
#

# How to create PostScript, PDF and HTML versions of the docs
#
# 1) Regenerate the html for both the programmer's and kernel manual.
#	As ptuser:
#	cd ~ptdesign/doc/prog_man
#	rm -f wmwork/*
#	
#	Then start up wpublish and regenerate the html
#
#	Do the same for kernel_man
#
#2) Run 'make tars' in both directories.
#	You should probably quickly check out the html version of the
#	manual and make sure it looks ok.
#
#3) Print the manual as single sided pages
#	Work on hubble, so you can see the printer.
#	Check the output as it comes out, so that you are sure there
#	are not printing duplex etc.
#	
#	setenv PRINTER lwc524
#	Remove the duplexer from the printer, put lots of paper in it,
#	Send email saying that you will be printing it.
#	Run print_prog_man in the prog_man directory
#	Look for missing chapters
#	If it prints out ok, do:
#	Run print_kernel_man in the kernel_man directory
#	Look for missing chapters


# Before including this makefile, be sure to set the following variables
# in your makefile.
# For example for the programmers manual
# VERSION=	0.5.2
# DOC_NAME=	prog_name
# DOC_DESCRIPTION= "Ptolemy $(VERSION) Programmers Manual"

# Script to generate Wide Area Information Service (WAIS) indexes
WAISINDEX =	/usr/tools/wais/wais/waisindex

# Top of the WWW tree
WWWDIR =	/vol/ptolemy/pt0/ftp/pub/ptolemy/www

# Destination of wais index
WAIS_SOURCES =	$(WWWDIR)/wais-sources

# Destination for  html pages
MAN_ARCHIVEDIR =$(WWWDIR)/$(DOC_NAME)$(VERSION).html

# All the .MIF files
MIFS =		$(DOCS:.doc=.MIF) $(DOCS:.fm=.MIF)
mifs: $(MIFS)

# All the html files
HTMLS =		$(MIFS:.MIF=.html)
htmls: $(HTMLS)


# Rule to convert .doc files to .MIF. See also /usr/tools/webmaker1.4a/bin/fm2mif
%.MIF: %.doc
	echo "Open $<" > /tmp/fm2mif.fmb
	echo "SaveAs m $< $@" >> /tmp/fm2mif.fmb		
	fmbatch -v /tmp/fm2mif.fmb
	rm -f /tmp/fm2mif.fmb

#%.MIF: %.fm
#	echo "Open $<" > /tmp/fm2mif.fmb
#	echo "SaveAs m $< $@" >> /tmp/fm2mif.fmb		
#	fmbatch -v /tmp/fm2mif.fmb
#	rm -f /tmp/fm2mif.fmb

%.fm: %.MIF
	echo "Open $<" > /tmp/mif2fm.fmb
	echo "SaveAs d $< $@" >> /tmp/mif2fm.fmb		
	fmbatch -v /tmp/mif2fm.fmb
	rm -f /tmp/mif2fm.fmb

%.MIF: %.book
%.MIF: %.book
	echo "Open $<" > /tmp/fm2mif.fmb
	echo "SaveAs m $< $@" >> /tmp/fm2mif.fmb		
	fmbatch -v /tmp/fm2mif.fmb
	rm -f /tmp/fm2mif.fmb

%.txt: %.doc
	$(PTOLEMY)/doc/bin/fm2txt $^


# Copy over chars for the title
CHARS= captau.gif eta.gif epsilon.gif capalpha.gif lambda.gif alpha.gif \
	gamma.gif sigma.gif tau.gif \
	capmu.gif iota.gif chi.gif rho.gif omicron.gif \
	pi.gif

fix_wmwork: wmwork/search.html
	-cp $(ROOT)/doc/lib/buttons/*.gif wmwork
	/users/ptdesign/doc/bin/addtoc $(DOCS)
	(cd wmwork; rm -f idx.html; ln -s $(DOC_NAME)IX.doc.html idx.html)
	rm -f wmwork/content.html
	/users/ptdesign/doc/bin/shorttoc $(DOC_NAME)TOC.doc.html > wmwork/content.html
	#(cd wmwork; rm -f content.html; ln -s $(DOC_NAME)TOC.doc.html content.html)
	-mkdir wmwork/chars
	for file in $(CHARS); \
	do \
		cp /usr/tools/tools2/www/quadralay/htdocs/chars/$$file wmwork/chars; \
	done

	# Fix the table of contents and index
	sed 's|<a href="#">|</a>|' wmwork/$(DOC_NAME)TOC.doc.html > $(DOC_NAME)TOC.doc.html.tmp
	mv $(DOC_NAME)TOC.doc.html.tmp wmwork/$(DOC_NAME)TOC.doc.html
	sed 's|<a href="#">|</a>|' wmwork/$(DOC_NAME)IX.doc.html > $(DOC_NAME)IX.doc.html.tmp
	mv $(DOC_NAME)IX.doc.html.tmp wmwork/$(DOC_NAME)IX.doc.html
	rm -f wmwork/index.html
	(cd wmwork; ln -s content.html index.html)
	rm -f wmwork/mif*

addtoc:
	/users/ptdesign/doc/bin/addtoc $(DOCS)

wmwork/search.html:
	@echo "Generating $@ for $(DOC_DESCRIPTION)"
	@echo "<html>" >> $@
	@echo "<head>" >> $@
	@echo "<title>Search $(DOC_DESCRIPTOR)</title>" >> $@
	@echo "<body bgcolor=\"#eef7ff\">" >> $@
	@echo "<h1>Search $(DOC_DESCRIPTOR)</h1>" >> $@
	@echo "Click on the link below to go to the search page." >> $@
	@echo "<p>" >> $@
	@echo "<A HREF=\"http://ptolemy.eecs.berkeley.edu/cgi-bin/$(DOC_NAME)-wais.pl\"><IMG ALIGN = BOTTOM SRC=\"search.gif\" ALT=\"Search\"></a>" >> $@
	@echo "</body>" >> $@
	@echo "</html>" >> $@

####################################################
# Build tar files for the PostScript, PDF and HTML

tars:	ps.tar pdf.tar html.tar
ps.tar:
	$(MAKE) fm2ps
	rm -f *LOM*.ps
	(cd ..; gtar -zcf $(DOC_NAME)/$(DOC_NAME)$(VERSION).tar.gz \
		$(DOC_NAME)/*.ps \
		$(DOC_NAME)/print_$(DOC_NAME)ual)

pdf.tar:
	$(MAKE) fm2ps
	rm -f *LOM*.ps
	distill *.ps
	(cd ..; gtar -zcf $(DOC_NAME)/$(DOC_NAME)$(VERSION).pdf.tar.gz $(DOC_NAME)/*.pdf)

html.tar:
	@echo "We are assuming that you ran wpublish and converted the frame files"
	rm -rf $(DOC_NAME)$(VERSION).html
	$(MAKE) fix_wmwork
	cp -r wmwork $(DOC_NAME)$(VERSION).html
	gtar -zcf $(DOC_NAME)$(VERSION).html.tar.gz $(DOC_NAME)$(VERSION).html


wais_man:
	@echo "Run this from messier so the host name for the database is right"		
	$(WAISINDEX) -l 1 -export \
	 -d $(WAIS_SOURCES)/pt-$(VERSION)_$(DOC_NAME) \
	 -t URL $(WWWDIR) http://ptolemy.eecs.berkeley.edu \
	 -r $(MAN_ARCHIVEDIR)


####################################################
# Generate a text only version of the document.  This is useful
# for spell checking everything at once, in case there are bogus
# words in the dictionary.
all.txt:
	$(FM2TXT) $(DOCS)
	cat $(DOCS:.doc=.txt) > all.txt

fmdoall:
	../bin/fmdoall $(DOCS)
