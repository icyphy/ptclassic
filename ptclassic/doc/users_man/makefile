# Version $Id$
# Authors:	T. M. Parks, Christopher Hylands
# Created:	12 Jan 94
# Build PostScript files for chapters written with LaTeXinfo.
# Build PostScript files for chapters written with Framemaker.
# Generate html from chapter written in Framemaker

ROOT= ../..
include $(ROOT)/mk/latexinfo.mk

CLEAN = chapter.clean

# Update all of the Framemaker cross references.
update_book: update.fmb
	fmbatch -v update.fmb

.PHONY:	update_book
update.fmb:
	echo "Open users_man.book" > $@
	echo "Update users_man.book" >> $@
	chmod g+w $@

# Framemaker files in the manual, in the order they appear
# title_small.doc is like title.doc, but does not have the large bitmap
DOCS=title.doc title_small.doc \
	users_manTOC.doc overview.doc pigi.doc ptcl.doc \
	domains.doc sdf.doc hof.doc ddf.doc bdf.doc pn.doc sr.doc fsm.doc \
	de.doc cg.doc cgc.doc cg56.doc c50.doc vhdl.doc documents.doc vem.doc \
	xgraph.doc \
	install.doc \
	x.doc filter.doc sharedlib.doc ptglossary.doc references.doc

# Script to convert Frame files to Postscript
FMPRINT2PS =	$(PTOLEMY)/doc/bin/fmprint2ps
# Script to fix up cg56.ps and cg96.ps postscript and to allow printing
# on printers w/o the exact size of paper (A4 vs European)
FIXFRAMEPS =	$(PTOLEMY)/doc/bin/fixframeps

FM2TXT =	$(PTOLEMY)/doc/bin/fm2txt

# Update all the cross references and then Create postscript files
fm2ps: update_book
	$(FMPRINT2PS) $(DOCS)
	@echo "Don't forget to print out install.ps separately"
	@echo "  Appendix A, the installation guide, is 3 separate files"
	@echo "First generate/update the book, then print out the 3 files that"
	@echo "make up appendix A as one file."
	$(FIXFRAMEPS) *.ps
	-chmod g+w *.ps
	rm -f *.ps.bak


FTPSITE=/vol/ptolemy/pt0/ftp/pub/ptolemy/ptolemy$(VERSION)


##########################################
# Generate html from .doc files

DOC_NAME=	users_man
SHORT_DOC_NAME = ptum
DOC_DESCRIPTOR=	"Ptolemy $(VERSION) Users Manual"
include $(ROOT)/doc/mk/docwais.mk

# Destination for users_man html pages
USERS_MAN_ARCHIVEDIR=$(WWWDIR)/users_man$(VERSION)


###################
# Update ~ptdesign/doc/html/install with appendix A
DOC_HTML_INSTALL = /users/ptdesign/doc/html/install

# This can be run many times until it is right.
ptdesign_doc_html_install:
	rm -rf $(DOC_HTML_INSTALL)
	mkdir $(DOC_HTML_INSTALL)
	-mkdir $(DOC_HTML_INSTALL)/../img
	-(cp $(ROOT)/doc/lib/buttons/*.gif $(DOC_HTML_INSTALL)/../img)
	-cp $(ROOT)/doc/lib/sorry.html \
		$(ROOT)/doc/lib/appendixAtitle.html $(DOC_HTML_INSTALL)
	(cd $(DOC_HTML_INSTALL); \
		ln -s sorry.html search.html; \
		ln -s sorry.html idx.html; \
		ln -s sorry.html x.doc.html; \
		ln -s appendixAtitle.html xgraph.doc5.html; \
		ln -s appendixAtitle.html content.html; \
		ln -s appendixAtitle.html title.doc.html; \
		ln -s appendixAtitle.html title.docA.html; \
		ln -s appendixAtitle.html index.html; \
		ln -s ../img/*.gif .)



