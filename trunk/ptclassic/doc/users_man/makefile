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

# This sort of barely works. Here's the procedure:
# 1. Log on to babbage, cd /users/ptdesign/doc/users/man
# 2. make mifs
# 3. make users_man.html
#	This rule will fix a small problem in obtainPtolemy.MIF, and then 
#	generate the html text files for the users_man
# 4. Set your display variable, the next step needs it
# 5. make users_man_graph
#	This will take several hours to run
# 6. The table of contents and the title page are messed up.  You will need
#	to copy these from wwwtocs
#		make update_local_www
# 7. For some reason the first several hundred html files are bogus, you will
# 	have to remove them.
#		make fix_www
# 8. Copy the files over to the ftp site:
#		make update_ftp_www
# 9. Update the wais server.
#	Log into messier first, or the wais entry will point to the machine you
#	are running on, which is probably not what you want.
#	rsh ptolemy "cd /users/ptdesign/doc/users_man; make wais_man"
# Webmaker binaries
# To use the wmgraph binary, place /usr/tools/webmaker/bin.sunos in your path
WMCONFIG =	/usr/tools/webmaker/bin/wmconfig
WEBMAKER =	/usr/tools/webmaker/bin/webmaker
WMGRAPH =	/usr/tools/webmaker/bin/wmgraph

# All the .MIF files
MIFS =		$(DOCS:.doc=.MIF) #users_man.MIF
mifs: $(MIFS)

# All the html files
HTMLS =		$(MIFS:.MIF=.html)
htmls: $(HTMLS)


# Rule to convert .doc files to .MIF. See also /usr/tools/webmaker/bin/fm2mif
%.MIF: %.doc
	echo "Open $<" > /tmp/fm2mif.fmb
	echo "SaveAs m $< $@" >> /tmp/fm2mif.fmb		
	fmbatch -v /tmp/fm2mif.fmb
	rm -f /tmp/fm2mif.fmb
%.MIF: %.book
	echo "Open $<" > /tmp/fm2mif.fmb
	echo "SaveAs m $< $@" >> /tmp/fm2mif.fmb		
	fmbatch -v /tmp/fm2mif.fmb
	rm -f /tmp/fm2mif.fmb
# Rule to convert mif files to html files
%.html: %.MIF
	$(WEBMAKER) -c users_man.wml -t $(basename $<) $<

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

# This rule generates all the .html files.
# Note that you may have to edit users_man_1.html and the file that contains
# Appendix A, section 4.3
users_man.html: $(MIFS) obtainPtolemy-fixed.MIF
	$(WEBMAKER) -c users_man.wml -t "Ptolemy $(VERSION) Users Manual" users_man.MIF

users_man_graph:
	@echo "For best results run on babbage (image path name problems)"
	$(WMGRAPH) -G -n users_man	

update_local_www:
	cp wwwtocs/users_man_0.html wmwork/www
	cp wwwtocs/users_man_1.html wmwork/www
	cp wwwtocs/users_man_2.html wmwork/www

# Remove users_man_3.html -> users_man_485.html
# Note that if the docs change, then these number will change
fix_www:
	csh -c "rm wmwork/www/users_man_[1-9]?.html wmwork/www/users_man_[123]??.html wmwork/www/users_man_4[0-7]?.html wmwork/www/users_man_48[0-5].html"

update_ftp_www:
	(cd wmwork/www; tar -cf - .) | rsh ptolemy "cd /vol/ptolemy/pt0/ftp/pub/ptolemy/www/users_man0.5.1; tar -xpf -"

# Don't run this, you'll clobber the configuration file we created by hand!
#users_man.wml: users_man.MIF
#	$(WMCONFIG) -c $@ $<


mif_links:
	for filename in $(MIFS); \
	do \
		ln -s users_man.html wmwork/www/`basename $$filename .MIF`_1.html;\
	done

# Top of the WWW tree
WWWDIR=/vol/ptolemy/pt0/ftp/pub/ptolemy/www

# Destination for mailing list archives
ARCHIVEDIR=$(WWWDIR)/users_man0.5.1

# Destination of wais index
WAIS_SOURCES=$(WWWDIR)/wais-sources

wais_man:
	-echo "Run this from messier so the host name for the database is right"
	/usr/sww/bin/waisindex -l 1 -export \
	 -d $(WAIS_SOURCES)/pt-0.5.1-users-man \
	 -t URL $(WWWDIR) http://ptolemy.eecs.berkeley.edu \
	 -r $(ARCHIVEDIR)

#	 -t URL /users/cxh/public_html/wmwork/www \
#		http://ptolemy.eecs.berkeley.edu/~cxh/wmwork/www \
#	 -r /users/cxh/public_html/wmwork/www 

