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
# JCLASS	.class files
# OBJS		.o files
# LIB		The name of the library being created.
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
# TYDOC		The Tycho Tydoc script, see $TYCHO/lib/tydoc.
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
# JFLAGS	Flags to pass to javac.
# JAVADOC	The 'javadoc' program
# JDOCFLAGS	Flags to pass to javadoc.
# JTESTHTML	Test html file for a java class.

##############
# Under no circumstances should this makefile include 'all', 'install'
# or 'depend' rules.  These rules should go in the makefile that 
# includes this makefile, or into no-compile.mk
# The reason is that we want to avoid duplicate 'all', 'install' 
# and 'depend' rules without using the possibly unportable double-colon
# makefile convention.

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
			sccs check || true ; \
			$(MAKE) $(MFLAGS) $(MAKEVARS) $@ ;\
			) \
		    fi ; \
		done ; \
	fi
	@sccs check || true

# Remove everytying that can be retrieved from SCCS, except files that
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
# Note that $(ROOT) here is relative to the tycho directory, not
# the Ptolemy directory.
TYDOC=$(ROOT)/lib/tydoc/tydoc
itcldocs: $(ITCL_SRCS) $(TCL_SRCS)
	@if [ "$(TYDOC_DESC)" = "" ] ; then \
		echo "$(TYDOC) -d $(ITCL_SRCS) $(TCL_SRCS)"; \
	 	$(TYDOC) -d $(ITCL_SRCS) $(TCL_SRCS); \
	else \
		echo "$(TYDOC) -d -t "$(TYDOC_DESC)" $(ITCL_SRCS) $(TCL_SRCS)"; \
		$(TYDOC) -d -t "$(TYDOC_DESC)" $(ITCL_SRCS) $(TCL_SRCS); \
	fi

# Generate idoc files from itcl files, requires itclsh and tycho
# Note that $(ROOT) here is relative to the tycho directory, not
# the Ptolemy directory.
IDOC=$(ROOT)/lib/tydoc/idoc
idocs: $(ITCL_SRCS) $(TCL_SRCS)
	@if [ "$(TYDOC_DESC)" = "" ] ; then \
		echo "$(IDOC) -d $(ITCL_SRCS) $(TCL_SRCS)"; \
	 	$(IDOC) -d $(ITCL_SRCS) $(TCL_SRCS); \
	else \
		echo "$(IDOC) -d -t "$(TYDOC_DESC)" $(ITCL_SRCS) $(TCL_SRCS)"; \
		$(IDOC) -d -t "$(TYDOC_DESC)" $(ITCL_SRCS) $(TCL_SRCS); \
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
$(LIBDIR)/$(LIB):	$(LIB) $(EXP)
	rm -f $@
	ln $(LIB) $(LIBDIR)

# Install debug versions, currently we just do a hard link
$(LIBDIR)/$(LIB_DEBUG):	$(LIBDIR)/$(LIB)
	rm -f $@
	(cd $(LIBDIR); ln $(LIB) $(LIB_DEBUG))


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
	rm -f `basename .java`.class
	CLASSPATH=$(CLASSPATH) $(JAVAC) $(JFLAGS) $<

# Build all the Java class files.
jclass:	$(JSRCS) $(JCLASS) 

# Build the Java documentation.
javadocs: doc/codeDoc/tree.html
jhtml: doc/codeDoc/tree.html
doc/codeDoc/tree.html:	$(JSRCS) 
	@if [ "$(JSRCS)" = "" ]; then \
		echo "No java sources, so we don't run javadoc";\
	else \
	if [ ! -d doc/codeDoc ]; then mkdir -p doc/codeDoc; fi; \
	rm -f doc/codeDoc/*.html; \
	CLASSPATH=$(CLASSPATH):$(JAVAHOME)/lib/classes.zip $(JAVADOC) $(JDOCFLAGS) -d doc/codeDoc $(JSRCS); \
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
	CLASSPATH=$(CLASSPATH) appletviewer $(TESTHTML)

htest-netscape: $(JTESTHTML) $(JCLASS)
	CLASSPATH=$(CLASSPATH) netscape $(TESTHTML)

##############
# Rules for testing 
# Most users will not run these rules.

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
	echo "doneTests" >> $@

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
		weblint -x netscape -d heading-order $(HTMLS); \
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
		$(JCLASS) $(OBJS) $(LIB) 
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
	config.cache config.log config.status $(JCLASS) $(KRUFT)  

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
	rm -rf doc/codeDoc/* $(OPTIONAL_FILES) $(HTMLCHEKOUT)*



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
