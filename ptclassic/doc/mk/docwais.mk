# Version $Id$
# Author	Christopher Hylands
# Created:	16 Jun 95
# Create html from framemaker

# 1. Log on to watson, cd /users/ptdesign/doc/user_man or prog_man
# 2. make mifs
# 3. Set your display variable, the next step needs it
# 4. make man_html
#	This rule will fix a small problem in obtainPtolemy.MIF, and then 
#	generate the html text files for the users_man


# 6. The table of contents and the title page are messed up.  You will need
#	to copy these from wwwtocs
#		make update_local_www
# 7. Adjust the search scripts in each file
#		make fix_wais_script
# 8. Copy the files over to the ftp site:
#		make update_ftp_www
# 9. Update the wais server.
#	Log into messier first, or the wais entry will point to the machine you
#	are running on, which is probably not what you want.
#	rsh ptolemy "cd /users/ptdesign/doc/users_man; make wais_man"
# Jose writes:
# > I wrote a short script to fix the figure bug in webmaker.  After running
# > webmaker, before running wmgraph run this script on the .MIF files
# > in the wmwork/graphics directory:
# !/usr/sww/bin/perl -pi.bak
# s/PgfNumString/PgfNumFormat/g;

# Notes about tables of contents:
# Make sure that paragraph headings like 1HeadingTOC map to Ignore.
# almagest.wml should contain lines like:
#   PARAGRAPH "1HeadingTOC" TYPE Normal { USES "Ignore" } 
# If this is not done, you will get lots of html pages that have headers
# and footers, but no text.
#
# All books should contain one and only one paragraph called Title or ContTitle
# Title and ContTitle are used to trigger production of the table of contents.
# For Ptolemy docs, try looking in the generated TOC file(i.e. prog_manTOC.MIF)
# Also, you can use grep \`Title\' *.MIF to search for files that have
# More than one instance of the Title tag.


# Before including this makefile, be sure to set the following variables
# in your makefile.
# For example for the programmers manual
# VERSION=	0.5.2
# DOC_NAME=	prog_name
# DOC_DESCRIPTION= "Ptolemy $(VERSION) Programmers Manual"

# Webmaker binaries
# To use the wmgraph binary, place /usr/tools/webmaker1.4a/bin.sunos in your path
#WMCONFIG =	/usr/tools/webmaker1.4a/webmaker/bin/wmconfig
WEBMAKER =	/usr/tools/webmaker1.4a/webmaker/bin/webmaker
#WEBMAKER =	/usr/tools/webmaker/webmaker
#WMGRAPH =	/usr/tools/webmaker1.4a/webmaker/bin/wmgraph

# Webmaker configuration file for Almagest
ALMAGEST_WML =	$(ROOT)/doc/lib/almagest.wml

# Script to generate Wide Area Information Service (WAIS) indexes
WAISINDEX =	/usr/tools/wais/wais/waisindex

# Top of the WWW tree
WWWDIR =	/vol/ptolemy/pt0/ftp/pub/ptolemy/www

# Destination of wais index
WAIS_SOURCES =	$(WWWDIR)/wais-sources

# Destination for  html pages
MAN_ARCHIVEDIR =$(WWWDIR)/$(DOC_NAME).$(VERSION)

# All the .MIF files
MIFS =		$(DOCS:.doc=.MIF)
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
%.MIF: %.book
	echo "Open $<" > /tmp/fm2mif.fmb
	echo "SaveAs m $< $@" >> /tmp/fm2mif.fmb		
	fmbatch -v /tmp/fm2mif.fmb
	rm -f /tmp/fm2mif.fmb

# Rule to convert mif files to html files
%.html: %.MIF
	$(WEBMAKER) -c $(ALMAGEST_WML) -t $(basename $<) $<

# Don't run this, you'll clobber the configuration file we created by hand!
#$(ALMAGEST.WML): users_man.MIF
#	$(WMCONFIG) -c $@ $<

.PHONY:	man_html man_graph wais_man
man_html: $(MIFS) $(DOC_NAME).MIF $(OTHER_MIFS)
	$(WEBMAKER) -c $(ALMAGEST_WML) -t $(DOC_DESCRIPTOR) -n $(SHORT_DOC_NAME) \
		-w wmwork/www  $(DOC_NAME).MIF
wmwork/www/next.gif:
	cp $(dir $(WEBMAKER))/lib/*.gif $(dir $@)

#man_graph:
#	@echo "For best results run on babbage (image path name problems)"
#	$(WMGRAPH) -G -n $(DOC_NAME)

#  Tar file containing html version (used by mirror sites)
$(DOC_NAME).$(VERSION)_html.tar.gz:
	(cd wmwork/www; /usr/sww/bin/gtar -zcf ../../$@ .)

# Fix the wais script references.  Each volume of the manual has a separate
# wais database
fix_wais_script:
	for file in `ls wmwork/www/*`; \
	do \
		sed s=cgi-bin/almagest-wais.pl=cgi-bin/$(DOC_NAME)-wais.pl= \
		$$file > /tmp/fix_wais; \
		cp  /tmp/fix_wais $$file; \
	done
# Update the ftp site with the html docs and a tar file.
update_ftp_www: $(MAN_ARCHIVEDIR) \
			$(DOC_NAME).$(VERSION)_html.tar.gz \
			wmwork/www/$(DOC_NAME)_1.html
	(cd wmwork/www; tar -cf - . ) | rsh ptolemy "cd $<; tar -xf -"
	cp $(DOC_NAME).$(VERSION)_html.tar.gz $(WWWDIR)

$(MAN_ARCHIVEDIR): wmwork/www/$(DOC_NAME)_2.html
	if [ ! -d $@ ]; then mkdir $@; fi
wmwork/www/$(DOC_NAME)_1.html:
	(cd wmwork/www; ln -s $(DOC_NAME)_2.html $(DOC_NAME)_1.html)

wais_man:
	@echo "Run this from messier so the host name for the database is right"		
	$(WAISINDEX) -l 1 -export \
	 -d $(WAIS_SOURCES)/pt-$(VERSION)_$(DOC_NAME) \
	 -t URL $(WWWDIR) http://ptolemy.eecs.berkeley.edu \
	 -r $(MAN_ARCHIVEDIR)

#	 -t URL /users/cxh/public_html/wmwork/www \
#		http://ptolemy.eecs.berkeley.edu/~cxh/wmwork/www \
#	 -r /users/cxh/public_html/wmwork/www 

