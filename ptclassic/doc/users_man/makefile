# Version $Id$
# Authors:	T. M. Parks, Christopher Hylands
# Created:	12 Jan 94
# Build PostScript files for chapters written with LaTeXinfo.
# Build PostScript files for chapters written with Framemaker.

ROOT= ../..
include $(ROOT)/mk/latexinfo.mk

CLEAN = chapter.clean

all:	cgc.ps cgc.contents cgc.index

clean:	$(CLEAN)
	rm -f front.aux other.aux null.aux 
	rm -f cgc.aux cgc.dvi

# Pattern rule for formatting a single chapter.
chapter%.dvi:
	$(ROOT)/doc/tex/mkchap $(TEX) $(<:.tex=) front,other $*
	$(ROOT)/doc/tex/mkchap $(TEX) $(<:.tex=) null $*
	$(ROOT)/doc/fmconvert/tex2mml chapter $* > $(<:.tex=.mml)

chapter13.dvi:	cgc.tex
cgc.dvi:	chapter13.dvi
		mv -f chapter.dvi cgc.dvi

chapter11.dvi:	cp.tex
cp.dvi:		chapter11.dvi
		mv -f chapter.dvi cp.dvi

# Update all of the Framemaker cross references.
update_book: update.fmb
	fmbatch -v update.fmb

.PHONY:	update_book
update.fmb:
	echo "Open users_man.book" > $@
	echo "Update users_man.book" >> $@
	chmod g+w $@

# Framemaker files in the manual.
# Be _sure_ to exclude cgc.doc cp.doc, because these are dummy files
DOCS=bdf.doc cg.doc cg56.doc cg96.doc ddf.doc de.doc documents.doc domains.doc filter.doc installation.doc mq.doc overview.doc pigi.doc ptcl.doc ptglossary.doc references.doc sdf.doc silage.doc sproc.doc thor.doc title.doc users_manIX.doc users_manLOM.doc users_manTOC.doc vem.doc x.doc xgraph.doc

# Update all the cross references and then Create postscript files
fm2ps: update_book
	fmprint2ps $(DOCS)

# Produce the installation docs in ascii format
/tmp/installation.txt: installation.doc /tmp/fmascii.fmb
	fmbatch -v /tmp/fmascii.fmb

/tmp/fmascii.fmb:
	echo "Open installation.doc" > $@ 
	echo "SaveAs t installation.doc /tmp/installation.txt" >> $@
	echo "Quit" >> $@ 

VERSION=0.5
INSTALL=INSTALL_$(VERSION)
INSTALL: $(INSTALL) 
$(INSTALL): /tmp/installation.txt
	echo "This text is generated from the users_man/installation.ps file." > $@ 
	echo "The $(TROUBLE_SHOOTING) file is a subset of this file." >> $@ 
	echo "This text file contains the latest and greatest version of the" >> $@ 
	echo "installation appendix.  The version in the doc.tar.Z file" >> $@ 
	echo "may be out of date with respect to this version." >> $@ 
	echo "" >>$@ 
	col -b < $^ | \
	awk '{if(length($$0)>120) printf("%s\n\n",$$0); else print $$0  }' | \
	awk '{if($$0 ~ /^A\./) printf("\n%s\n\n",$$0); else print $$0}' | \
	sed 's/\\x11/ /g' | \
	/usr/sww/bin/gfold -s -w 65 >> $@

TROUBLE_SHOOTING=TROUBLE_SHOOTING_$(VERSION)
TROUBLE_SHOOTING: $(TROUBLE_SHOOTING)
$(TROUBLE_SHOOTING): $(INSTALL)
	echo "This text is generated from the users_man/installation.ps file" > $@ 
	echo "This file is a subset of $(INSTALL)" >> $@ 
	echo "" >>$@ 
	awk '$$0 ~ /^A.[0-9][ 	]*Troubleshooting/ || printit==1 {printit=1;print $$0 }' $^ | \
	sed 's/\\x11/ /g' >> $@

clean_INSTALL:
	rm -f /tmp/installation.txt /tmp/fmascii.fmb
