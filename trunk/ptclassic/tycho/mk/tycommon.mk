# Makefile rules that are common to most files.
#
# Version Identification:
# $Id$
# Copyright (c) 1990-%Q% The Regents of the University of California.
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

# Please don't use GNU make extensions in this file, such as 'ifdef' or '%'.
# If you really must use an GNU make extension, please label it.

# This file should not include any Ptolemy specific rules, such as rules
# to create stars etc. Ptolemy rules should be in 'common.mk'

# External makefile variables that this file uses
#
# Directories:
# DIRS		Subdirectories to run make in. 
# LIBDIR	The destination directory for any libraries created.
#			Usually this is an architecture dependent library.
#
# Files:
# SRCS 		Files that are compiled, such as .c, .cc and .java files.
# EXTRA_SRCS	Files that are not compiled, such as .tcl and .itcl files.
# HTMLS		HTML files
# ITCL_SRCS	.itcl files
# TCL_SRCS	.tcl files
# HDRS		.h files.
# JSRCS		.java files
# OPTIONAL_JSRCS derived .java files (i.e. created by javacc)
# JCLASS	.class files
# OBJS		.o files
# LIBR		The name of the library being created.  We can't just call
#		this LIB because of problems with the LIB environment variable
#		under NT
# EXP		???
# MISC_FILES	Non-source files such as README files.
# OPTIONAL_FILES Files that are derived from other files, but we don't
#			want 'make checkjunk' to complain about

# Variables used by cleaning rules:
# KRUFT		Files to be removed by 'make clean'
# REALCLEAN_STUFF Files to be removed by 'make realclean'
#
# Variables used by tests:
# SIMPLE_TESTS	Itcl tests that don't require a graphical front end
# GRAPHICAL_TESTS	Itcl tests that do require a graphical front end
#
# Scripts:
# TYDOC		The Tycho Tydoc script, see $TYCHO/util/tydoc.
# ITCLSH	The Itcl 'itclsh' binary 
#
# C and C++ Compiler variables:
# CC		The C Compiler
# CPLUSPLUS	The C++ Compiler
# C_SHAREDFLAGS CC_SHAREDFLAGS  Flags to build shared objects for C and C++
# CFLAGS GPPFLAGS  	The C and C++ Compiler Flags
# C_INCL INCL		The C and C++ Include Flags
#
# Java Variables
# JAVAHOME	The home of the Java Developer's Kit (JDK)
# JAVAC		The 'javac' compiler.
# JFCHOME	The home of the Java Foundation Classes (JFC) aka Swing
# JFLAGS	Flags to pass to javac.
# JAVADOC	The 'javadoc' program
# JDOCFLAGS	Flags to pass to javadoc.
# TYZIP		Zip file of classes to be produced.
# JARFILE	Jar file of classes to be produced.
# JBEANCLASSES  Classes that are included in the jar file as Java Beans.
# JDIST		The name and version of the tar.gz and zip files of the sources
# JTESTHTML	Test html file for a java class.
# JTCLSH	TclBlend Tcl/Java interface shell.
#
# The variables below are for the SunTest JavaScope code coverage tool
# See http://www.suntest.com/JavaScope
# JSINSTR	The 'jsinstr' command, which instruments Java code.
# JSINTRFLAGS	Flags to pass to jsintr.
# JSRESTORE	The 'jsrestore' command which uninstruments Java code.

##############
# Under no circumstances should this makefile include 'all', 'install'
# or 'depend' rules.  These rules should go in the makefile that 
# includes this makefile, or into no-compile.mk
# The reason is that we want to avoid duplicate 'all', 'install' 
# and 'depend' rules without using the possibly unportable double-colon
# makefile convention.

# Run make all in the subdirs
suball:
	@if [ "x$(DIRS)" != "x" ]; then \
		set $(DIRS); \
		for x do \
		    if [ -w $$x ] ; then \
			( cd $$x ; \
			echo making all in $$x ; \
			$(MAKE) $(MFLAGS) $(MAKEVARS) all ;\
			) \
		    fi ; \
		done ; \
	fi

# Run make install in the subdirs
subinstall:
	@if [ "x$(DIRS)" != "x" ]; then \
		set $(DIRS); \
		for x do \
		    if [ -w $$x ] ; then \
			( cd $$x ; \
			echo making install in $$x ; \
			$(MAKE) $(MFLAGS) $(MAKEVARS) install ;\
			) \
		    fi ; \
		done ; \
	fi

# Install links for scripts in $PTOLEMY/bin and $TYCHO/bin
tybin_install:
	-if [ "x$(EXTRA_SRCS)" != "x" ]; then \
		set $(EXTRA_SRCS); \
		for f do \
			if [ -w "$(PTOLEMY)/bin" ]; then \
				echo "Installing $$f in $(PTOLEMY)/bin";\
				rm -f $(PTOLEMY)/bin/$$f; \
				ln $$f $(PTOLEMY)/bin; \
			fi; \
			if [ -w "$(PTOLEMY)/tycho/bin" ]; then \
				echo "Installing $$f in $(PTOLEMY)/tycho/bin";\
				rm -f $(PTOLEMY)/tycho/bin/$$f; \
				ln $$f $(PTOLEMY)/tycho/bin; \
			else \
				if [ -w "$(TYCHO)/bin" ]; then \
					echo "Installing $$f in $(TYCHO)/bin";\
					rm -f $(TYCHO)/bin/$$f; \
					ln $$f $(TYCHO)/bin; \
				fi; \
			fi; \
		done ; \
	fi

# "make sources" will do SCCS get on anything where SCCS file is newer.
sources::	$(SRCS) $(EXTRA_SRCS) $(HDRS) $(MISC_FILES) makefile
	@if [ "x$(DIRS)" != "x" ]; then \
		set $(DIRS); \
		for x do \
		    if [ -w $$x ] ; then \
			( cd $$x ; \
			echo making $@ in $$x ; \
			$(MAKE) $(MFLAGS) $(MAKEVARS) $@ ;\
			) \
		    fi ; \
		done ; \
	fi

# "check" does not print anything if nothing is being edited.
sccsinfo:
	@if [ "x$(DIRS)" != "x" ]; then \
		set $(DIRS); \
		for x do \
		    if [ -w $$x ] ; then \
			( cd $$x ; \
			echo making $@ in $$x ; \
			$(MAKE) $(MFLAGS) $(MAKEVARS) $@ ;\
			) \
		    fi ; \
		done ; \
	fi
	@sccs check || true

# Remove everything that can be retrieved from SCCS, except files that
# are being edited.
sccsclean:
	@if [ "x$(DIRS)" != "x" ]; then \
		set $(DIRS); \
		@for x do \
		    if [ -w $$x ] ; then \
			( cd $$x ; \
			echo making $@ in $$x ; \
			$(MAKE) $(MFLAGS) $(MAKEVARS) $@ ;\
			) \
		    fi ; \
		done ; \
	fi
	sccs clean

makefiles: makefile
	@if [ "x$(DIRS)" != "x" ]; then \
		set $(DIRS); \
		for x do \
		    if [ -w $$x ] ; then \
			( cd $$x ; \
			echo making $@ in $$x ; \
			$(MAKE) $(MFLAGS) $(MAKEVARS) $@ ;\
			) \
		    fi ; \
		done ; \
	fi

##############
# Rules for Itcl/Tcl

# Generate html files from itcl files, requires itclsh and tycho
itcldocs: $(ITCL_SRCS) $(TCL_SRCS)
	@if [ "$(TYDOC_DESC)" = "" ] ; then \
		chmod a+x $(TYDOC); \
		echo "$(TYDOC) -notestdir -d $(ITCL_SRCS) $(TCL_SRCS)"; \
	 	$(TYDOC) -notestdir -d $(ITCL_SRCS) $(TCL_SRCS); \
	else \
		chmod a+x $(TYDOC); \
		echo "$(TYDOC) -notestdir -d -t "$(TYDOC_DESC)" $(ITCL_SRCS) $(TCL_SRCS)"; \
		$(TYDOC) -notestdir -d -t "$(TYDOC_DESC)" $(ITCL_SRCS) $(TCL_SRCS); \
	fi
	@if [ "x$(DIRS)" != "x" ]; then \
		set $(DIRS); \
		for x do \
		    if [ -w $$x ] ; then \
			( cd $$x ; \
			echo making $@ in $$x ; \
			$(MAKE) $(MFLAGS) $(MAKEVARS) $@ ;\
			) \
		    fi ; \
		done ; \
	fi

# Generate idoc files from itcl and java files, requires itclsh and tycho
# Note that $(ROOT) here is relative to the tycho directory, not
# the Ptolemy directory.
IDOC=$(ROOT)/util/tydoc/idoc
idocs: $(ITCL_SRCS) $(TCL_SRCS) $(JSRCS)
	@if [ "$(TYDOC_DESC)" = "" ] ; then \
		echo "$(IDOC) -d $(ITCL_SRCS) $(TCL_SRCS) $(JSRCS)"; \
	 	$(IDOC) -d $(ITCL_SRCS) $(TCL_SRCS) $(JSRCS); \
	else \
		echo "$(IDOC) -d -t "$(TYDOC_DESC)" $(ITCL_SRCS) $(TCL_SRCS) $(JSRCS)"; \
		$(IDOC) -d -t "$(TYDOC_DESC)" $(ITCL_SRCS) $(TCL_SRCS) $(JSRCS); \
	fi

# Create tclIndex from .tcl and .itcl files
# This rule must be after the TCL_SRC and ITCL_SRC lines in the makefile
# that includes this makefile.  tclIndex should depend on the makefile
# in case we edit the makefile and move a tcl file to another location.
# We print the errorInfo stack in case there is a missing close brace
# in one of the tcl files.
tclIndex: $(TCL_SRCS) $(ITCL_SRCS) makefile
	@echo "Updating tclIndex"
	rm -f $@
	echo 'set auto_path [linsert $$auto_path 0 [info library] ]; if [catch {auto_mkindex . $(TCL_SRCS) $(ITCL_SRCS)} errMsg] {puts $$errorInfo}' | $(ITCLSH)

##############
# Rules for compiling

# Rule for installing a C++ library
$(LIBDIR)/$(LIBR):	$(LIBR) $(EXP)
	rm -f $@
	ln $(LIBR) $(LIBDIR)

# Install debug versions, currently we just do a hard link
$(LIBDIR)/$(LIBR_DEBUG):	$(LIBDIR)/$(LIBR)
	rm -f $@
	(cd $(LIBDIR); ln $(LIBR) $(LIBR_DEBUG))


# Rule for compiling C++ files
.cc.o:
	$(CPLUSPLUS) $(CC_SHAREDFLAGS) $(GPPFLAGS) $(CCFLAGS) \
	$(OTHERCCFLAGS) $(USER_CCFLAGS) \
	-I$(VPATH) $(INCL) -c $<

# Rule for compiling with cc
.c.o:
	$(CC) $(C_SHAREDFLAGS) $(CFLAGS) $(OTHERCFLAGS) $(USER_CFLAGS) \
	$(C_INCL) -c $<

##############
# Java rules

.SUFFIXES: .class .java
.java.class:
	rm -f `basename $< .java`.class
	CLASSPATH="$(CLASSPATH)$(AUXCLASSPATH)" $(JAVAC) $(JFLAGS) $<

# Build all the Java class files.
# Run in the subdirs first in case the subpackages need to be compiled first.
jclass:	$(JSRCS) subjclass $(JCLASS) 

subjclass:
	@if [ "x$(DIRS)" != "x" ]; then \
		set $(DIRS); \
		for x do \
		    if [ -w $$x ] ; then \
			( cd $$x ; \
			echo making jclass in $$x ; \
			$(MAKE) $(MFLAGS) $(MAKEVARS) jclass ;\
			) \
		    fi ; \
		done ; \
	fi

# Compile the classes that require JDK1.1 and later.
jclass1_1: $(JSRCS1_1) $(JCLASS1_1)

# Build the Java documentation.
javadocs: doc/codeDoc/tree.html
jhtml: doc/codeDoc/tree.html
doc/codeDoc/tree.html:	$(JSRCS) $(OPTIONAL_JSRCS)
	@if [ "$(JSRCS)" = "" ]; then \
		echo "No java sources, so we don't run javadoc";\
	else \
	if [ ! -d doc/codeDoc ]; then mkdir -p doc/codeDoc; fi; \
	rm -f doc/codeDoc/*.html; \
	CLASSPATH="$(CLASSPATH)$(CLASSPATHSEPARATOR)$(JAVAHOME)/lib/classes.zip$(AUXCLASSPATH)" \
	   $(JAVADOC) $(JDOCFLAGS) -d doc/codeDoc $(JSRCS) $(OPTIONAL_JSRCS); \
	for x in doc/codeDoc/*.html; do \
		echo "Fixing paths in $$x"; \
		sed -e 's|<a href="java|<a href="$(JAVAHTMLDIR)/java|g' \
		-e 's|<img src="images/|<img src="$(JAVAHTMLDIR)/images/|g' \
			$$x > $$x.bak; \
		mv $$x.bak $$x; \
	done; \
	fi

# Bring up the appletviewer on a test file.
jtest: $(JTESTHTML) $(JCLASS)
	CLASSPATH="$(CLASSPATH)" appletviewer $(TESTHTML)

htest-netscape: $(JTESTHTML) $(JCLASS)
	CLASSPATH="$(CLASSPATH)" netscape $(TESTHTML)

# Create a zip file of the .class files
# We cd up one level so that the zip file has the proper package name
tyzip: $(TYZIP)
$(TYZIP): $(JSRCS) $(JCLASS)
	(cd $(CLASSPATH); $(JAR) -c0Mf $@ $(TYPACKAGE_DIR)/*.class)

jars: $(JARFILE) 
$(JARFILE): $(JSRCS) $(JCLASS)
	@if [ "$(JBEANCLASSES)" != "" ]; then \
		echo "Creating manifest.tmp"; \
		rm -f manifest.tmp; \
		echo "Manifest-Version: 1.0" > manifest.tmp; \
		set $(JBEANCLASSES); \
		for x do \
			echo "Name: $$x" >> manifest.tmp; \
			echo "Java-Bean: True" >> manifest.tmp; \
		done; \
		(cd $(CLASSPATH); $(JAR) cfm $(TYPACKAGE_DIR)/$@ \
			$(TYPACKAGE_DIR)/manifest.tmp \
			$(TYPACKAGE_DIR)/*.class); \
	else \
		echo "Creating $@"; \
		(cd $(CLASSPATH); $(JAR) cf $(TYPACKAGE_DIR)/$@ \
			$(TYPACKAGE_DIR)/*.class); \
	fi

##############
# Rules to build Java and Itcl package distributions


# The distributions to build
TYDISTS =	$(TYDIST).tar.gz $(TYDIST).zip 

# The dists rule builds both a tar file and zip file of the sources
dists: sources install $(TYDISTS)
	@if [ "x$(SRCDIRS)" != "x" ]; then \
		set $(SRCDIRS); \
		for x do \
		    if [ -w $$x ] ; then \
			( cd $$x ; \
			echo making $@ in $$x ; \
			$(MAKE) $(MFLAGS) $(MAKEVARS) $@ ;\
			) \
		    fi ; \
		done ; \
	fi

# List of files to exclude
TYDIST_EX =	/tmp/$(TYDIST).ex
$(TYDIST_EX): $(ROOT)/mk/tycommon.mk
	@if [ "$(TYPACKAGE_DIR)x" = "x" ]; then \
		echo "TYPACKAGE_DIR is not set in the makefile, so we won't create a tar exclude file"; \
	else \
		/bin/echo "adm\nSCCS\n*.tar.gz\n*[0-9].zip\n" > $@ ; \
	fi

# Tar file distribution
$(TYDIST).tar.gz:  $(TYDIST_EX)
	if [ "$(TYPACKAGE_DIR)x" = "x" ]; then \
		echo "TYPACKAGE_DIR is not set in the makefile, so we won't create a tar file"; \
	else \
		echo "Building $@"; \
		(cd $(TYPACKAGE_ROOTDIR); \
		 gtar -zchf $(TYPACKAGE_DIR)/$@ -X $(TYDIST_EX) $(TYPACKAGE_DIR)); \
		rm -f $(TYDIST_EX); \
	fi

# Tar file distribution
$(TYDIST).zip:
	@if [ "$(TYPACKAGE_DIR)x" = "x" ]; then \
		echo "TYPACKAGE_DIR is not set in the makefile, so we won't create a zip file"; \
	else \
		echo "Building $@"; \
		(cd $(TYPACKAGE_ROOTDIR); zip -r $(TYPACKAGE_DIR)/$@ $(TYPACKAGE_DIR) -x \*/adm/\* -x \*/SCCS/\* -x \*/$(TYDIST).tar.gz -x \*/$(TYDIST).zip); \
	fi

# Build sources in a form suitable for releasing
buildjdist:
	$(MAKE) sources
	$(MAKE) realclean
	@echo "Compile any classes that require JDK1.1"
	$(MAKE) JFLAGS=-O jclass1_1 
	$(MAKE) jhtml
	@echo "We must use JDK1.0.2 to compile for use with Netscape"
	$(MAKE) JAVAHOME=/usr/sww/lang/java-1.0.2 JFLAGS=-O jclass
	$(MAKE) install
	$(MAKE) dists

# Create a distribution and install it.
# This rule is particular to our local installation
JDESTDIR = /vol/ptolemy/pt0/ftp/pub/ptolemy/www/java
installjdist:
	$(MAKE) buildjdist
	$(MAKE) updatewebsite

updatewebsite: $(TYDISTS)
	@echo "Updating website"
	(cd $(JDESTDIR); rm -rf $(TYPACKAGE); mkdir $(TYPACKAGE))
	cp $(TYDISTS) $(JDESTDIR)/$(TYPACKAGE)
	(cd $(JDESTDIR); gtar -zxf $(TYPACKAGE)/$(TYDIST).tar.gz;\
	 chmod g+ws $(TYPACKAGE))
	(cd $(JDESTDIR)/$(TYPACKAGE); chmod g+w $(TYDISTS))

##############
# Rules for testing 
# Most users will not run these rules.

# Instrument Java code for use with JavaScope.
jsinstr:
	$(JSINSTR) $(JSINTRFLAGS) $(JSRCS)
# If the jsoriginal directory does not exist, then instrument the Java files.
jsoriginal:
	@if [ ! -d jsoriginal -a "$(JSRCS)" != "" ]; then \
		echo "$(JSINSTR) $(JSINTRFLAGS) $(JSRCS)"; \
		$(JSINSTR) $(JSINTRFLAGS) $(JSRCS); \
	fi

# Back out the instrumentation.
jsrestore:
	$(JSRESTORE) $(JSRCS)
	-rm -f jsoriginal/README
	-rmdir jsoriginal
	$(MAKE) clean

# Compile the instrumented Java classes and include JavaScope.zip
jsbuild:
	$(MAKE) AUXCLASSPATH="$(CLASSPATHSEPARATOR)$(JSCLASSPATH)" jclass
# Run the test_jsimple rule with the proper classpath  
jstest_jsimple:
	$(MAKE) AUXCLASSPATH="$(CLASSPATHSEPARATOR)$(JSCLASSPATH)" \
		test_jsimple
	@echo "To view code coverage results, run javascope or jsreport"
	@echo "To get a summary, run jsreport -dbsummary" 

# Run the test_jsimple rule with the proper classpath  
jstest_jgraphical:
	$(MAKE) AUXCLASSPATH="$(CLASSPATHSEPARATOR)$(JSCLASSPATH)" \
		test_jgraphical
	@echo "To view code coverage results, run javascope or jsreport"
	@echo "To get a summary, run jsreport -dbsummary" 


# If necessary, instrument the classes, then rebuild, then run the tests
jsall: jsoriginal
	$(MAKE) clean
	$(MAKE) jsbuild
	if [ -w test ] ; then \
	   (cd test; $(MAKE) jstest_jsimple); \
	fi


# Run all the tests
tests:: makefile
	@if [ "x$(DIRS)" != "x" ]; then \
		set $(DIRS); \
		for x do \
		    if [ -w $$x ] ; then \
			( cd $$x ; \
			echo making $@ in $$x ; \
			$(MAKE) $(MFLAGS) $(MAKEVARS) $@ ;\
			) \
		    fi ; \
		done ; \
	fi


# alltests.itcl is used to source all the tests
alltests.itcl: makefile
	rm -f $@
	echo '# CAUTION: automatically generated file by a rule in tycommon.mk' > $@
	echo '# This file will source the .itcl files list in the' >> $@
	echo '# makefile SIMPLE_TESTS and GRAPHICAL_TESTS variables' >> $@ 
	echo '# This file is different from all.itcl in that all.itcl' >> $@ 
	echo '# will source all the .itcl files in the current directory' >> $@
	echo '#' >> $@
	echo '# Set the following to avoid endless calls to exit' >> $@
	echo "if {![info exists reallyExit]} {set reallyExit 0}" >> $@
	echo '# Exiting when there are no more windows is wrong' >> $@
	echo "::tycho::TopLevel::exitWhenNoMoreWindows 0" >> $@
	echo "#Do an update so that we are sure tycho is done displaying" >> $@
	echo "update" >> $@
	echo "set savedir \"[pwd]\"" >> $@
	echo "if {\"$(SIMPLE_TESTS)\" != \"\"} {foreach i [list $(SIMPLE_TESTS)] {puts \$$i; cd \"\$$savedir\"; if [ file exists \$$i ] {source \$$i}}}" >> $@
	if [ "x$(GRAPHICAL_TESTS)" != "x" ]; then \
		for x in $(GRAPHICAL_TESTS); do \
			echo "puts stderr $$x" >> $@; \
			echo "cd \"\$$savedir\"" >> $@; \
			echo "source $$x" >> $@; \
		done; \
	fi
	echo "catch {doneTests}" >> $@
	echo "# IMPORTANT: DON'T CALL exit HERE." >> $@
	echo "# If exit is present, then the Builder will exit Tycho" >> $@
	echo "# whenever the tests are run" >> $@ 
	echo "#exit" >> $@

# alljtests.tcl is used to source all the tcl files that use Java
alljtests.tcl: makefile
	rm -f $@
	echo '# CAUTION: automatically generated file by a rule in tycommon.mk' > $@
	echo '# This file will source all the Tcl files that use Java. ' >> $@ 
	echo '# This file will source the .itcl files list in the' >> $@
	echo '# makefile SIMPLE_JTESTS and GRAPHICAL_JTESTS variables' >> $@ 
	echo '# This file is different from all.itcl in that all.itcl' >> $@ 
	echo '# will source all the .itcl files in the current directory' >> $@
	echo '#' >> $@
	echo '# Set the following to avoid endless calls to exit' >> $@
	echo "if {![info exists reallyExit]} {set reallyExit 0}" >> $@
	echo '# Exiting when there are no more windows is wrong' >> $@
	echo "#::tycho::TopLevel::exitWhenNoMoreWindows 0" >> $@
	echo '# If there is no update command, define a dummy proc.  Jacl needs this' >> $@
	echo 'if {[info command update] == ""} then { ' >> $@
	echo '    proc update {} {}' >> $@
	echo '}' >> $@
	echo "#Do an update so that we are sure tycho is done displaying" >> $@
	echo "update" >> $@
	echo "set savedir \"[pwd]\"" >> $@
	echo "if {\"$(JSIMPLE_TESTS)\" != \"\"} {foreach i [list $(JSIMPLE_TESTS)] {puts \$$i; cd \"\$$savedir\"; if [ file exists \$$i ] {source \$$i}}}" >> $@
	if [ "x$(JGRAPHICAL_TESTS)" != "x" ]; then \
		for x in $(JGRAPHICAL_TESTS); do \
			echo "puts stderr $$x" >> $@; \
			echo "cd \"\$$savedir\"" >> $@; \
			echo "if [ file exists $$x ] {source $$x}" >> $@; \
		done; \
	fi
	echo "catch {doneTests}" >> $@
	echo "exit" >> $@

# all.itcl is used to source all the *.itcl files
all.itcl: makefile
	rm -f $@
	echo '# CAUTION: automatically generated file by a rule in tycommon.mk' > $@
	echo '# This file will source all the .itcl files in the current' >> $@
	echo '# directory.  This file is different from alltest.itcl' >> $@ 
	echo '# in that alltest.itcl will source only the itcl files' >> $@
	echo '# that are listed in the makefile' >> $@
	echo '#' >> $@
	echo '# Set the following to avoid endless calls to exit' >> $@
	echo 'set reallyExit 0' >> $@
	echo 'set PASSED 0' >> $@
	echo 'set FAILED 0' >> $@
	echo 'foreach file [glob *.itcl] {' >> $@
	echo '    if {$$file != "all.itcl" && $$file != "alltests.itcl"} {' >> $@
	echo '         source $$file' >> $@
	echo '    }' >> $@
	echo '}' >> $@

# Generate html files from itcl files, requires itclsh and tycho
# We use a GNU make extension here
HTMLS=$(filter %.html,  $(EXTRA_SRCS))
# weblint finds problems with html pages
# ftp://ftp.cre.canon.co.uk/pub/weblint/weblint.tar.gz
weblint:
	@if [ "$(HTMLS)" != "" ]; then \
		echo "Running weblint on $(HTMLS)"; \
		weblint -x Netscape,Java -d heading-order $(HTMLS); \
	fi
	@if [ "x$(DIRS)" != "x" ]; then \
		set $(DIRS); \
		for x do \
		    if [ -w $$x ] ; then \
			( cd $$x ; \
			echo making $@ in $$x ; \
			$(MAKE) $(MFLAGS) $(MAKEVARS) $@ ;\
			) \
		    fi ; \
		done ; \
	fi

# Check html docs for problems
# htmlchek is not shipped with tycho, see:
# 	ftp://ftp.cs.buffalo.edu/pub/htmlchek/
HTMLCHEK=/usr/tools/www/htmlchek
HTMLCHEKOUT=htmlchekout
htmlchek:
	rm -f $(HTMLCHEKOUT)*
	HTMLCHEK=$(HTMLCHEK); export HTMLCHEK; \
	sh $(HTMLCHEK)/runachek.sh `pwd` $(HTMLCHEKOUT) `pwd` \
		map=1 netscape=1 nowswarn=1 arena=1 strictpair=TCL,AUTHOR

checkjunk:
	@checkextra -v $(SRCS) $(HDRS) $(EXTRA_SRCS) $(MISC_FILES) \
		$(OPTIONAL_FILES) $(JSRCS) makefile SCCS \
		$(JCLASS) $(OBJS) $(LIBR) $(TYDISTS) $(JARFILE)
	@if [ "x$(DIRS)" != "x" ]; then \
		set $(DIRS); \
		for x do \
		    if [ -w $$x ] ; then \
			( cd $$x ; \
			echo making $@ in $$x ; \
			$(MAKE) $(MFLAGS) $(MAKEVARS) $@ ;\
			) \
		    fi ; \
		done ; \
	fi

##############
# Rules for cleaning

CRUD=*.o *.so core *~ *.bak ,* LOG* *.class \
	config.cache config.log config.status manifest.tmp \
	$(JCLASS) $(TYPACKAGE).zip $(TYPACKAGE).jar \
	$(TYDISTS) $(JARFILE) $(KRUFT)  

clean:
	rm -f $(CRUD)
	@if [ "x$(DIRS)" != "x" ]; then \
		set $(DIRS); \
		for x do \
		    if [ -w $$x ] ; then \
			( cd $$x ; \
			echo making $@ in $$x ; \
			$(MAKE) $(MFLAGS) $(MAKEVARS) $@ ;\
			) \
		    fi ; \
		done ; \
	fi

# Cleaner than 'make clean'
# Remove the stuff in the parent directory after processing
# the child directories incase something in the child depends on
# something we will be removing in the parent
# REALCLEAN_STUFF - Files to be removed by 'make realclean'
realclean:
	@if [ "x$(DIRS)" != "x" ]; then \
		set $(DIRS); \
		for x do \
		    if [ -w $$x ] ; then \
			( cd $$x ; \
			echo making $@ in $$x ; \
			$(MAKE) $(MFLAGS) $(MAKEVARS) $@ ;\
			) \
		    fi ; \
		done ; \
	fi
	rm -f $(CRUD) configure $(REALCLEAN_STUFF) 
	rm -f doc/codeDoc/* $(OPTIONAL_FILES) $(HTMLCHEKOUT)*



# Remove the sources too, so that we can get them back from sccs
extraclean:
	rm -f $(CRUD) $(REALCLEAN_STUFF) $(EXTRA_SRCS) $(JSRCS)
	rm -rf doc/codeDoc/*
	@if [ "x$(DIRS)" != "x" ]; then \
		set $(DIRS); \
		for x do \
		    if [ -w $$x ] ; then \
			( cd $$x ; \
			echo making $@ in $$x ; \
			$(MAKE) $(MFLAGS) $(MAKEVARS) $@ ;\
			) \
		    fi ; \
		done ; \
	fi
