# Version $Id$
# Authors:	T. M. Parks, Christopher Hylands
# Created:	12 Jan 94
# Build PostScript files for chapters written with LaTeXinfo.
# Build PostScript files for chapters written with Framemaker.
# Generate html from chapter written in Framemaker

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
DOCS=bdf.doc cg.doc cg56.doc cg96.doc ddf.doc de.doc documents.doc domains.doc filter.doc hof.doc installation.doc mq.doc obtainPtolemy.doc overview.doc pigi.doc ptcl.doc ptglossary.doc references.doc sdf.doc silage.doc sproc.doc thor.doc title.doc troubleshooting.doc users_manIX.doc users_manLOM.doc users_manTOC.doc vem.doc x.doc xgraph.doc

# Script to convert Frame files to Postscript
FMPRINT2PS =	$(PTOLEMY)/doc/bin/fmprint2ps
# Script to fix up cg56.ps and cg96.ps postscript and to allow printing
# on printers w/o the exact size of paper (A4 vs European)
FIXFRAMEPS =	$(PTOLEMY)/bin/fixframeps

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

VERSION=0.5.2
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
	/usr/sww/bin/gexpand | \
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
	/usr/sww/bin/gexpand | \
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
	/usr/sww/bin/gexpand | \
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
	-diff $(FTPSITE)/$(OBTAINING_PTOLEMY) $(OBTAINING_PTOLEMY) 
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

##########################################
# Generate html from .doc files

DOC_NAME=	users_man
DOC_DESCRIPTOR=	"Ptolemy $(VERSION) Users Manual"
OTHER_MIFS=	obtainPtolemy-fixed.MIF
include $(ROOT)/doc/mk/docwais.mk

# Destination for users_man html pages
USERS_MAN_ARCHIVEDIR=$(WWWDIR)/users_man$(VERSION)


# You will need to edit obtainPtolemy.MIF by hand and change 2Heading
# to 2Heading-alpha, or else webmaker will get an error
# <Para 
#  <Unique 626839>
#  <PgfTag `2Heading'>
#  <PgfNumString `A.2  '>
#  <ParaLine 
#   <String `Ptolemy mailing lists'>
#  >

# Look for the uniqu ide 627416, then print 3Heading instead of 2Heading
obtainPtolemy-fixed.MIF: obtainPtolemy.MIF
	awk '{if ($$0 ~ /627416/){print $$0;printf("  <PgfTag %c3Heading%c>\n",96,39); getline } else print$$0}' $^ > $@
	-diff $@ $^
	cp $@ $^

# You may want to add the following lines to title.MIF for 
# table of contents generation (uncomment them first).
# <Para 
#  <Unique 627418>
#  <PgfTag `ExternalTOC'>
# >

update_local_www:
	cp wwwtocs/users_man_0.html wmwork/www
	cp wwwtocs/users_man_1.html wmwork/www
	cp wwwtocs/users_man_2.html wmwork/www


.PHONY:	obtainPtolemy.html installation.html troubleshooting.html \
	appendix_A_html update_appendix_A_html

wmwork/www/obtainPtolemy-fixed_2.html: obtainPtolemy-fixed.MIF
	$(WEBMAKER) -c $(ALMAGEST_WML) -t "Ptolemy $(VERSION) Users Manual" $^
wmwork/www/installation_1.html: installation.MIF
	$(WEBMAKER) -c $(ALMAGEST_WML) -t "Ptolemy $(VERSION) Users Manual" $^
wmwork/www/troubleshooting_1.html: troubleshooting.MIF
	$(WEBMAKER) -c $(ALMAGEST_WML) -t "Ptolemy $(VERSION) Users Manual" $^

WWW_APPENDIXA =	$(WWWDIR)/appendixA

appendixA_html: wmwork/www/obtainPtolemy-fixed_2.html \
		wmwork/www/installation_1.html \
		wmwork/www/troubleshooting_1.html


.PHONY:	update_appendixA_html
update_appendixA_html: 	$(WWW_APPENDIXA) \
		$(WWW_APPENDIXA)/obtainPtolemy-fixed_1.html \
		$(WWW_APPENDIXA)/installation_1.html \
		$(WWW_APPENDIXA)/troubleshooting_1.html

$(WWW_APPENDIXA)/obtainPtolemy-fixed_1.html: \
					wmwork/www/obtainPtolemy-fixed_2.html
	(cd wmwork/www; tar -cf - obtainPtolemy-fixed*) | \
		rsh ptolemy "cd $(dir $@); tar -xpf -"
	(cd $(dir $@); rm -f $@; \
		ln -s appendixA.html obtainPtolemy-fixed_1.html; \
		ln -s appendixA.html index.html);

$(WWW_APPENDIXA)/installation_1.html: wmwork/www/installation_1.html
	(cd wmwork/www; tar -cf - installation*) | rsh ptolemy "cd $(dir $@); tar -xpf -"
	(cd $(dir $@); rm -f $@; ln -s appendixA.html installation_1.html)


$(WWW_APPENDIXA)/troubleshooting_1.html: wmwork/www/troubleshooting_1.html
	(cd wmwork/www; tar -cf - troubleshooting*) | rsh ptolemy "cd $(dir $@); tar -xpf -"
	(cd $(dir $@); rm -f $@; ln -s appendixA.html troubleshooting_1.html)

$(WWW_APPENDIXA): appendixA.html
	if [ ! -d $@ ]; then mkdir $@; fi
	cp $^ $@

.PHONY:	appendixA.html
appendixA.html: 
	@echo "Creating new $@"
	@echo '<!-- Generated from (cd ~ptdesign/doc/users_man; make appendixA.html) -->' > $@
	@echo '<HTML>' >> $@
	@echo '<HEAD>' >> $@
	@echo '<TITLE>Ptolemy 0.5.2 Users Manual: Appendix A: Installation and Troubleshooting</TITLE>' >> $@
	@echo '</HEAD>' >> $@
	@echo '<BODY>' >> $@
	@echo '<A HREF="obtainPtolemy-fixed_2.html">[Next] </A>[Contents] [Index] <A HREF="http://ptolemy.eecs.berkeley.edu/cgi-bin/users-man-wais.pl">[Search] </A><P>' >> $@
	@echo '<A NAME=HEADING1>' >> $@
	@echo '<H1>Appendix A: Installation and Troubleshooting  </H1>' >> $@
	@echo '<HR>' >> $@
	@echo '<DL>' >> $@
	@echo '<DT><A HREF="obtainPtolemy-fixed_3.html#HEADING2"><B>A.1  </B> - Obtaining Ptolemy</A><DD>' >> $@
	@echo '<DT><A HREF="obtainPtolemy-fixed_8.html#HEADING7"><B>A.2  </B> - Ptolemy mailing lists</A><DD>' >> $@
	@echo '<DT><A HREF="obtainPtolemy-fixed_9.html#HEADING8"><B>A.3  </B> - Ptolemy Newsgroup</A><DD>' >> $@
	@echo '<DT><A HREF="installation_2.html#HEADING2"><B>A.4  </B> - Installation</A><DD>' >> $@
	@echo '<DT><A HREF="troubleshooting_2.html#HEADING2"><B>A.5  </B> - Troubleshooting </A><DD>' >> $@
	@echo '</DL>' >> $@
	@echo '' >> $@
	@echo '<HR>' >> $@
	@echo "<ADDRESS>Ptolemy 0.5.2 Users Manual `date` </ADDRESS>" >> $@
	@echo '<A HREF="obtainPtolemy-fixed_2.html">[Next] </A>[Contents] [Index] <A HREF="http://ptolemy.eecs.berkeley.edu/cgi-bin/users-man-wais.pl">[Search] </A><P>' >> $@
	@echo '</BODY>' >> $@
	@echo '</HTML>' >> $@
	@echo "Done creating new $@"

# Remove users_man_3.html -> users_man_485.html
# Note that if the docs change, then these number will change
fix_www:
	csh -c "rm wmwork/www/users_man_[1-9]?.html wmwork/www/users_man_[123]??.html wmwork/www/users_man_4[0-7]?.html wmwork/www/users_man_48[0-5].html"

update_ftp_www:
	(cd wmwork/www; tar -cf - .) | rsh ptolemy "cd $(USERS_MAN_ARCHIVEDIR); tar -xpf -"

mif_links:
	for filename in $(MIFS); \
	do \
		ln -s users_man.html wmwork/www/`basename $$filename .MIF`_1.html;\
	done

