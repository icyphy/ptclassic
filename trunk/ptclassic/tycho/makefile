# Makefile for Ptolemy Tycho system, top Tycho directory
#
# $Id$
#
# Copyright (c) 1991-%Q% The Regents of the University of California.
# All rights reserved.
# 
# Permission is hereby granted, without written agreement and without
# license or royalty fees, to use, copy, modify, and distribute this
# software and its documentation for any purpose, provided that the
# above copyright notice and the following two paragraphs appear in all
# copies of this software.
# 
# IN NO EVENT SHALL THE UNIVERSITY OF CALIFORNIA BE LIABLE TO ANY PARTY
# FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES
# ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF
# THE UNIVERSITY OF CALIFORNIA HAS BEEN ADVISED OF THE POSSIBILITY OF
# SUCH DAMAGE.
# 
# THE UNIVERSITY OF CALIFORNIA SPECIFICALLY DISCLAIMS ANY WARRANTIES,
# INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
# MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. THE SOFTWARE
# PROVIDED HEREUNDER IS ON AN "AS IS" BASIS, AND THE UNIVERSITY OF
# CALIFORNIA HAS NO OBLIGATION TO PROVIDE MAINTENANCE, SUPPORT, UPDATES,
# ENHANCEMENTS, OR MODIFICATIONS.
# 
# 						PT_COPYRIGHT_VERSION_2
# 						COPYRIGHTENDKEY
##########################################################################

# Please don't use GNU make extensions in this or any other Tycho makefile

# The doc directory should be last, since it contains the Tycho class diagram
DIRS = kernel editors lib devel typt doc java #bin

# Root of Tycho directory
ROOT =		.

# Get configuration info
CONFIG =	$(ROOT)/mk/tycho.mk
include $(CONFIG)

MISC_FILES =	README README.files copyright .glimpse_exclude

# "make sources" will do SCCS get on anything where SCCS file is newer.
# You probably don't want to add $(SRCS) to this, rather, change
# the makefile that includes this one to use $(EXTRA_SRCS)
# This rule should be the first rule in this makefile so that 'make'
# will run 'make sources'.
sources:: subsources indices makefile README.files

# We use a subtarget here so that we do make sources in the subdirs
# before trying to update the indices
subsources:	$(EXTRA_SRCS) $(HDRS) $(MISC_FILES) makefile 
	@for x in $(DIRS); do \
	    if [ -w $$x ] ; then \
		( cd $$x ; \
		echo making sources in $$x ; \
		$(MAKE) $(MFLAGS) $(MAKEVARS) sources ;\
		) \
	    fi ; \
	done

indices:
	(cd lib/idx; $(MAKE))

# If JAVAHOME is not set, then don't go down into the java directory
makefiles all clean depend realclean checkjunk sccsinfo sccsclean itcldocs \
	tests extraclean weblint:
	@for x in $(DIRS); do \
	    if [ -w $$x ] ; then \
		if [ $$x = "java" ]; then \
			if [ ! -d $(JAVAHOME) ]; then \
				echo "JAVAHOME = $(JAVAHOME), which does not exist, so no need to run in the java directory"; \
				echo "mk/tycommon.mk contains the line that sets JAVAHOME"; \
				continue; \
			fi ; \
		fi; \
		( cd $$x ; \
		echo making $@ in $$x ; \
		$(MAKE) $(MFLAGS) $(MAKEVARS) $@ ;\
		) \
	    fi ; \
	done

# We make itcldocs twice because they use the codeDoc.idx index, which
# is created from the itcldocs
# If JAVAHOME is not set, then don't go down into the java directory
install: subinstall itcldocs indices itcldocs_again
subinstall:
	@for x in $(DIRS); do \
	    if [ -w $$x ] ; then \
		if [ $$x = "java" ]; then \
			if [ ! -d $(JAVAHOME) ]; then \
				echo "JAVAHOME = $(JAVAHOME), which does not exist, so no need to run in the java directory"; \
				echo "mk/tycommon.mk contains the line that sets JAVAHOME"; \
				continue; \
			fi ; \
		fi; \
		( cd $$x ; \
		echo making install in $$x ; \
		$(MAKE) $(MFLAGS) $(MAKEVARS) install ;\
		) \
	    fi ; \
	done

# We have to have a itcldocs and a itcldocs_again rule, 'make install'
# will not run 'make itcldocs' twice.
itcldocs_again:
	@echo "Running make itcldocs again so we pick up the codeDoc index"
	@for x in $(DIRS); do \
	    if [ -w $$x ] ; then \
		( cd $$x ; \
		echo making itcldocs in $$x ; \
		$(MAKE) $(MFLAGS) $(MAKEVARS) itcldocs ;\
		) \
	    fi ; \
	done

TAGS:
	echo "Making tags makes no sense in the tycho directory"

############################################################################
# Rules that compile Tcl extensions.
#
# You can use Tycho without these extensions, though certain features,
# such as the Tcl/Java interface will be missing.

tclexts: config_tclexts all_tclexts install_tclexts

# Run configure
# You can pass arguments to configure like so:
# make tclexts CONFIGURE_ARGS=--with-tclsrc=/users/cxh/pt/src/tcltk/itcl2.2/tcl7.6
# If you don't have Java, then do 
# make tclexts CONFIGURE_ARGS=--without-java
#
# If you are trying to build with Java on the HP, see
# tyjava/tcljava/makefile.in
#
# If you add an Tcl extension, add the makefile.in to the list of dependencies
# so that it will be retrieved when others do 'make sources'.
config_tclexts: obj.$(PTARCH) src/configure mk/tcl.mk.in mk/java.mk.in \
		src/makefile.in src/profile/makefile.in \
		src/tyjava/makefile.in src/tyjava/eprintf/makefile.in \
		src/tyjava/tcljava/makefile.in \
		src/tksteal/makefile.in \
		src/tytimer/makefile.in \
		src/tytest/makefile.in \
		src/tycgc/makefile.in
	(cd obj.$(PTARCH); ../src/configure --srcdir ../src $(CONFIGURE_ARGS))
	(cd obj.$(PTARCH)/tyjava/tcljava; rm -f tcl; \
		ln -s ../../../src/tyjava/tcljava/tcl .)

# The autoconf script is not shipped with Tycho, but src/configure
# _is_ shipped, so this rule is not usually needed.
src/configure: src/configure.in
	(cd src; autoconf)

all_tclexts: 
	(cd obj.$(PTARCH); $(MAKE) all)

install_tclexts: lib.$(PTARCH)
	(cd obj.$(PTARCH); $(MAKE) install)

obj.$(PTARCH):
	if [ "$(PTARCH)" = "" ]; then \
		echo "The PTARCH variable must be set."; \
		echo "Under csh, try 'setenv PTARCH `$$TYCHO/bin/ptarch`'"; \
	fi
	mkdir $@

lib.$(PTARCH):
	mkdir $@

############################################################################
# Rules that create indices and class diagrams

# This rule should contains things that might take a little while
# to run, so it is run nightly
nightly: sources doc/tychoClasses.dag typt/lib/PtolemyFacets.fst \
			itcldocs indices glimpse

# Empty rule to force certain makefile rules to run
FORCE: 

# Create a Directed Acyclic Graph that contains the Tycho classes
doc/tychoClasses.dag: FORCE
	(cd doc; $(MAKE) tychoClasses.dag)

# Create a forest that contains all the Ptolemy universes
typt/lib/PtolemyFacets.fst: FORCE
	(cd typt/lib; rm -f PtolemyFacets.fst; $(MAKE) PtolemyFacets.fst)

# Glimpse is a tool that prepares an index of a directory tree.
# glimpse is not included with Tycho, see http://glimpse.cs.arizona.edu
GLIMPSEINDEX =	/usr/sww/bin/glimpseindex
glimpse: .glimpse_exclude
	@echo "Saving .glimpse_exclude, removing the .glimpse* files"
	rm -f glimpse_exclude
	cp .glimpse_exclude glimpse_exclude
	rm -f .glimpse*
	cp  glimpse_exclude .glimpse_exclude
	if [ "$(TYCHO)" = "" ]; then \
		$(GLIMPSEINDEX) -H "$(PTOLEMY)/tycho" "$(PTOLEMY)/tycho"; \
		chmod a+r $(PTOLEMY)/tycho/.glimpse_*; \
	else \
		$(GLIMPSEINDEX) -H "$(TYCHO)" "$(TYCHO)"; \
		chmod a+r $(TYCHO)/.glimpse_*; \
	fi
	rm -f glimpse_exclude

############################################################################
# Rules used to test a release 

ALLTESTS = \
	kernel/test/alltests.itcl \
	editors/textedit/test/alltests.itcl \
	editors/visedit/test/alltests.itcl \
	editors/slate/test/alltests.itcl \

alltests.itcl: makefile
	echo "# Automatically generated by tycho/makefile" > $@
	echo "set top [pwd]" >> $@
	for x in $(ALLTESTS); do \
		echo "cd [file join \$$top [file dirname $$x]]" >> $@; \
		echo "if [catch {source [file tail $$x]} errmsg] {" >> $@; \
		echo " puts \$$errmsg" >> $@; \
		echo "}" >> $@; \
	done
alltests: alltests.itcl
	tycho -noconsole -e alltests.itcl

# Check html docs for problems
# htmlchek is not shipped with tycho, see:
# 	ftp://ftp.cs.buffalo.edu/pub/htmlchek/
#
# See $TYCHO/doc/coding/testing.html for instructions.
#
HTMLCHEK=/usr/tools/www/htmlchek
HTMLCHEKOUT=htmlchekout
htmlchek:
	rm -f $(HTMLCHEKOUT)*
	@HTMLCHEK=$(HTMLCHEK); export HTMLCHEK; \
	if [ "$(TYCHO)" = "" ]; then \
		echo "TYCHO is not set so we assume we are part of PTOLEMY"; \
		sh $(HTMLCHEK)/runachek.sh $(PTOLEMY)/tycho $(HTMLCHEKOUT) \
			$(PTOLEMY)/tycho map=1 \
			netscape=1 nowswarn=1 arena=1 strictpair=TCL,AUTHOR; \
	else \
		sh $(HTMLCHEK)/runachek.sh $(TYCHO) $(HTMLCHEKOUT) \
			$(TYCHO) map=1 \
			netscape=1 nowswarn=1 arena=1 strictpair=TCL,AUTHOR; \
	fi
	-chmod g+w $(HTMLCHEKOUT)*
	@echo "The following links in $(HTMLCHEKOUT).HREF might be bogus"
	ls `egrep '\.html' htmlchekout.HREF | grep -v http`

spellcheck:
	cat `find . -name "*.html" -print | grep -v html_library` | spell
