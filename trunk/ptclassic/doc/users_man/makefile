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

chapter14.dvi:	cgc.tex
cgc.dvi:	chapter14.dvi
		mv -f chapter.dvi cgc.dvi

chapter12.dvi:	cp.tex
cp.dvi:		chapter12.dvi
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
DOCS=bdf.doc cg.doc cg56.doc cg96.doc ddf.doc de.doc documents.doc domains.doc filter.doc installation.doc mq.doc obtainPtolemy.doc overview.doc pigi.doc ptcl.doc ptglossary.doc references.doc sdf.doc silage.doc sproc.doc thor.doc title.doc troubleshooting.doc users_manIX.doc users_manLOM.doc users_manTOC.doc vem.doc x.doc xgraph.doc

# Update all the cross references and then Create postscript files
fm2ps: update_book
	fmprint2ps $(DOCS)

# Produce the obtainPtolemy docs in ascii format
/tmp/obtainPtolemy.txt: obtainPtolemy.doc /tmp/fmobtainPtolemy.fmb
	fmbatch -v /tmp/fmobtainPtolemy.fmb

/tmp/fmobtainPtolemy.fmb:
	echo "Open obtainPtolemy.doc" > $@ 
	echo "SaveAs t obtainPtolemy.doc /tmp/obtainPtolemy.txt" >> $@
	echo "Quit" >> $@ 

# Produce the installation docs in ascii format
/tmp/installation.txt: installation.doc /tmp/fminstallation.fmb
	fmbatch -v /tmp/fminstallation.fmb

/tmp/fminstallation.fmb:
	echo "Open installation.doc" > $@ 
	echo "SaveAs t installation.doc /tmp/installation.txt" >> $@
	echo "Quit" >> $@ 

# Produce the troubleshooting docs in ascii format
/tmp/troubleshooting.txt: troubleshooting.doc /tmp/fmtroubleshooting.fmb
	fmbatch -v /tmp/fmtroubleshooting.fmb

/tmp/fmtroubleshooting.fmb:
	echo "Open troubleshooting.doc" > $@ 
	echo "SaveAs t troubleshooting.doc /tmp/troubleshooting.txt" >> $@
	echo "Quit" >> $@ 

VERSION=0.5.1
OBTAINING_PTOLEMY=OBTAINING_PTOLEMY_$(VERSION)
OBTAINING_PTOLEMY: $(OBTAINING_PTOLEMY) 
$(OBTAINING_PTOLEMY): /tmp/obtainPtolemy.txt
	echo "The users_man/installation chapter has been split into three sections" >$@ 
	echo "	Obtaining Ptolemy" >> $@ 
	echo "	Installation" >> $@ 
	echo "	Trouble Shooting" >> $@ 
	echo "This text is generated from the users_man/obtainingPtolemy.ps file." >> $@ 
	echo "This text file contains the latest and greatest version of the" >> $@ 
	echo "installation appendix.  The version in the doc.tar.Z file" >> $@ 
	echo "may be out of date with respect to this version." >> $@ 
	echo "" >>$@ 
	col -b < $^ | \
	awk '{if(length($$0)>120) printf("%s\n\n",$$0); else print $$0  }' | \
	awk '{if($$0 ~ /^A\./) printf("\n%s\n\n",$$0); else print $$0}' | \
	sed 's/\\x11/ /g' | \
	/usr/sww/bin/gfold -s -w 65 | \
	awk '{if ($$0 ~ /setenv GCC_EXEC_PREFIX/) printf("%s",$$0); else print $$0 }' | \
	awk '{if ($$0 ~ /setenv CPLUS_INCLUDE_PATH/) printf("%s",substr($$0,1,length($$0)-1)); else print $$0 }' | \
	awk '{if ($$0 ~ /setenv CPLUS_INCLUDE_PATH.*inclu/) printf("%s",$$0); else print $$0 }' >> $@

INSTALL=INSTALL_$(VERSION)
INSTALL: $(INSTALL) 
$(INSTALL): /tmp/installation.txt
	echo "The users_man/installation chapter has been split into three sections" >$@ 
	echo "	Obtaining Ptolemy" >> $@ 
	echo "	Installation" >> $@ 
	echo "	Trouble Shooting" >> $@ 
	echo "This text is generated from the users_man/installation.ps file." >> $@ 
	echo "This text file contains the latest and greatest version of the" >> $@ 
	echo "installation appendix.  The version in the doc.tar.Z file" >> $@ 
	echo "may be out of date with respect to this version." >> $@ 
	echo "" >>$@ 
	col -b < $^ | \
	awk '{if(length($$0)>120) printf("%s\n\n",$$0); else print $$0  }' | \
	awk '{if($$0 ~ /^A\./) printf("\n%s\n\n",$$0); else print $$0}' | \
	sed 's/\\x11/ /g' | \
	/usr/sww/bin/gfold -s -w 65 | \
	awk '{if ($$0 ~ /setenv GCC_EXEC_PREFIX/) printf("%s",$$0); else print $$0 }' | \
	awk '{if ($$0 ~ /setenv CPLUS_INCLUDE_PATH/) printf("%s",substr($$0,1,length($$0)-1)); else print $$0 }' | \
	awk '{if ($$0 ~ /setenv CPLUS_INCLUDE_PATH.*inclu/) printf("%s",$$0); else print $$0 }' >> $@

TROUBLE_SHOOTING=TROUBLE_SHOOTING_$(VERSION)
TROUBLE_SHOOTING: $(TROUBLE_SHOOTING)
$(TROUBLE_SHOOTING): /tmp/troubleshooting.txt
	echo "The users_man/installation chapter has been split into three sections" >$@ 
	echo "	Obtaining Ptolemy" >> $@ 
	echo "	Installation" >> $@ 
	echo "	Trouble Shooting" >> $@ 
	echo "This text is generated from the users_man/troubleshooting.ps file." >> $@ 
	echo "This text file contains the latest and greatest version of the" >> $@ 
	echo "installation appendix.  The version in the doc.tar.Z file" >> $@ 
	echo "may be out of date with respect to this version." >> $@ 
	echo "" >>$@ 
	col -b < $^ | \
	awk '{if(length($$0)>120) printf("%s\n\n",$$0); else print $$0  }' | \
	awk '{if($$0 ~ /^A\./) printf("\n%s\n\n",$$0); else print $$0}' | \
	sed 's/\\x11/ /g' | \
	/usr/sww/bin/gfold -s -w 65 | \
	awk '{if ($$0 ~ /setenv GCC_EXEC_PREFIX/) printf("%s",$$0); else print $$0 }' | \
	awk '{if ($$0 ~ /setenv CPLUS_INCLUDE_PATH/) printf("%s",substr($$0,1,length($$0)-1)); else print $$0 }' | \
	awk '{if ($$0 ~ /setenv CPLUS_INCLUDE_PATH.*inclu/) printf("%s",$$0); else print $$0 }' >> $@

clean_OBTAINING_PTOLEMY:
	rm -f /tmp/obtainingPtolemy.txt /tmp/fmobtainingPtolemy.fmb
clean_INSTALL:
	rm -f /tmp/installation.txt /tmp/fminstallation.fmb
clean_TROUBLE_SHOOTING:
	rm -f /tmp/troubleshooting.txt /tmp/fmtroubleshooting.fmb
clean_ftp_txt:
	clean_OBTAINING_PTOLEMY
	clean_INSTALL
	clean_TROUBLE_SHOOTING

#FTPSITE=/vol/ptolemy/pt0/ftp/pub/ptolemy/ptolemy$(VERSION)
FTPSITE=/vol/ptolemy/pt0/ftp/pub/ptolemy/ptolemy$(VERSION)
diff_OBTAINING_PTOLEMY: $(OBTAINING_PTOLEMY) 
	diff $(FTPSITE)/$(OBTAINING_PTOLEMY) $(OBTAINING_PTOLEMY) 
diff_INSTALL: $(INSTALL) 
	-diff $(FTPSITE)/$(INSTALL) $(INSTALL) 
diff_TROUBLE_SHOOTING: $(TROUBLE_SHOOTING)
	-diff $(FTPSITE)/$(TROUBLE_SHOOTING) $(TROUBLE_SHOOTING) 
diff_ftp_txt:	diff_OBTAINING_PTOLEMY	diff_INSTALL	diff_TROUBLE_SHOOTING

update_OBTAINING_PTOLEMY:
	cp $(OBTAINING_PTOLEMY) $(FTPSITE)/$(OBTAINING_PTOLEMY) 
update_INSTALL:
	cp $(INSTALL) $(FTPSITE)/$(INSTALL) 
update_TROUBLE_SHOOTING:
	cp $(TROUBLE_SHOOTING) $(FTPSITE)/$(TROUBLE_SHOOTING) 

update_ftp_txt: \
	diff_OBTAINING_PTOLEMY diff_INSTALL diff_TROUBLE_SHOOTING \
	update_OBTAINING_PTOLEMY update_INSTALL update_TROUBLE_SHOOTING
