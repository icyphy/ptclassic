# Version $Id$
# Author	Christopher Hylands
# Created:	16 Jun 95
# Create html from framemaker


# Webmaker binaries
# To use the wmgraph binary, place /usr/tools/webmaker/bin.sunos in your path
WMCONFIG =	/usr/tools/webmaker/bin/wmconfig
WEBMAKER =	/usr/tools/webmaker/bin/webmaker
WMGRAPH =	/usr/tools/webmaker/bin/wmgraph

# Webmaker configuration file for Almagest
ALMAGEST_WML=$(ROOT)/doc/lib/almagest.wml

# Top of the WWW tree
WWWDIR=/vol/ptolemy/pt0/ftp/pub/ptolemy/www

# Destination of wais index
WAIS_SOURCES=$(WWWDIR)/wais-sources

# All the .MIF files
MIFS =		$(DOCS:.doc=.MIF)
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
	$(WEBMAKER) -c $(ALMAGEST_WML) -t $(basename $<) $<


