# Version Idenitfication:
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
# src/tcltk/makefile
# Makefile to build and install tcl/tk

# The default action is to build Itcl with shared libraries

# You should not need to edit this file, merely set the
# PTARCH and PTOLEMY environment variables, for example:
#	setenv PTARCH sol2.5
#	setenv PTOLEMY /users/ptolemy

# root of source tree
ROOT	      = ../..
# Path (absolute or relative) to the object directory root
OBJDIR=..
# VPATH points to the "real" source directory
VPATH         = $(ROOT)/src/tcltk

# Get configuration info, like X11_INCLUDES and X11_LIB_SWITCHES
CONFIG=$(ROOT)/mk/config-$(PTARCH).mk
include $(CONFIG)

# Destination of the tcl/tk binaries, libraries and include files. 
TCLTK_DEST=$(PTOLEMY)/tcltk

# Location of the TCL sources.
TCLTK_SRC=$(PTOLEMY)/src/tcltk

# Location of the architecture specific build directory.
TCLTK_OBJ=$(PTOLEMY)/obj.$(PTARCH)/tcltk

# where to install binaries
BINDIR=$(PTOLEMY)/bin.$(PTARCH)

# No changes should be necessary below this line
#---------------------------------------------------------
ENABLE_GCC =		--enable-gcc
# If you build Itcl2.2 with shared libraries on sun4, then	
# incremental linking of stars will fail with a message from ld
# about how it cannot find -litk22
# This file has not been tested with Itcl3.0 under SunOS4.x
ENABLE_SHARED =		`$(PTOLEMY)/src/tcltk/tcl_shared`

ITCL_MAJOR_VERSION_NUMBER =	3.0
ITCL_VERSION =		itcl$(ITCL_MAJOR_VERSION_NUMBER).1
IWIDGETS_MAJOR_VERSION_NUMBER =	3.0.0
TCLTK8_VERSION = 		8.0.5
TCLTK_MAJOR_VERSION_NUMBER = 	8.0 
WINDOWSTCLTK_MAJOR_VERSION_NUMBER = 80

TCL8_SRC =	$(TCLTK_SRC)/tcl$(TCLTK8_VERSION)/unix
TK8_SRC =	$(TCLTK_SRC)/tk$(TCLTK8_VERSION)/unix
TCL8_OBJ =	$(TCLTK_OBJ)/tcl$(TCLTK8_VERSION)/unix
TK8_OBJ =	$(TCLTK_OBJ)/tk$(TCLTK8_VERSION)/unix
TCL8_DEST_BASE =	tcl.$(PTARCH)
TCL8_DEST =	$(TCLTK_DEST)/$(TCL8_DEST_BASE)
TCL8_INDEPENDENT_DEST =	$(TCLTK_DEST)/tcl

#ITCL_SRC =	$(TCLTK_SRC)/$(ITCL_VERSION).$(TCLX_VERSION)
ITCL_SRC =	$(TCLTK_SRC)/$(ITCL_VERSION)

# Location of the itcl only object files
ITCL_OBJ =	$(TCLTK_OBJ)/$(ITCL_VERSION)
#TCL_OBJ =	$(ITCL_OBJ)/tcl$(ITCL_TCL_MAJOR_VERSION_NUMBER)
#TK_OBJ =	$(ITCL_OBJ)/tk$(ITCL_TK_MAJOR_VERSION_NUMBER)
IWIDGETS_OBJ =	$(ITCL_OBJ)/iwidgets$(IWIDGETS_MAJOR_VERSION_NUMBER)

# Location of itcl bin directory relative to Ptolemy bin directory
# Can't use absolute path name here, or we will create a distribution that
# will not work if $PTOLEMY != /users/ptolemy
REL_ITCL_BINDIR =	../tcltk/$(TCL8_DEST_BASE)/bin

ITCL_DEST_LIB = $(TCL8_DEST)/lib


all: 		configure tcltk8 itcl

configure:	configure_tcltk8 $(PTOLEMY)/src/tcltk/tcl_shared \
			$(ITCL_OBJ)/config.status $(TCLTK_OBJ)/makefile

#configure:	configure_tcltk8 tcltk8 install_tcltk8 \
#			$(PTOLEMY)/src/tcltk/tcl_shared \
#			$(ITCL_OBJ)/config.status $(TCLTK_OBJ)/makefile
bin:		tcltk8 itcl
install:	install_tcltk8 install_itcl

clean:		clean_itcl
realclean:	clean_itcl

######################################################
# Rules for Tcl


all_tcltk8: 	configure_tcltk8 tcltk8 install_tcltk8
configure_tcltk8: configure_tcl8 configure_tk8
configure_tcl8: $(TCL8_OBJ) $(TCL8_SRC)/configure
	@echo "Configuring tcl8"
	(cd $(TCL8_OBJ); PWD=$(TCL8_OBJ); export PWD; \
	$(TCL8_SRC)/configure \
		--srcdir=$(TCL8_SRC) \
		--prefix=$(TCL8_INDEPENDENT_DEST) \
		--exec-prefix=$(TCL8_DEST) \
		$(ENABLE_SHARED) $(ENABLE_GCC))

# We may need to run configure, esp. if we checked out from cvs
$(TCL8_SRC)/configure: $(TCL8_SRC)/configure.in
	(cd $(TCL8_SRC); autoconf)

configure_tk8: $(TK8_SRC)/configure $(TK8_OBJ)
	@echo "Configuring tk8"
	(cd $(TK8_OBJ); PWD=$(TK8_OBJ); export PWD; \
	$(TK8_SRC)/configure \
		--srcdir=$(TK8_SRC) \
		--with-tcl=$(TCL8_OBJ) \
		--prefix=$(TCL8_INDEPENDENT_DEST) \
		--exec-prefix=$(TCL8_DEST) \
		$(ENABLE_SHARED) $(ENABLE_GCC))

# We may need to run configure, esp. if we checked out from cvs
$(TK8_SRC)/configure: $(TK8_SRC)/configure.in
	(cd $(TK8_SRC); autoconf)

tcltk8:
	(cd $(TCL8_OBJ); PWD=$(TCL8_OBJ); export PWD; $(MAKE))
	(cd $(TK8_OBJ); PWD=$(TK8_OBJ); export PWD; $(MAKE))

install_tcltk8: $(TCL8_DEST) $(TCL8_INDEPENDENT_DEST)
	-chmod a+x $(TCL8_SRC)/install-sh $(TK8_SRC)/install-sh
	-chmod a+x $(TCL8_SRC)/mkLinks $(TK8_SRC)/mkLinks
	(cd $(TCL8_OBJ); PWD=$(TCL8_OBJ); export PWD; $(MAKE) install)
	(cd $(TK8_OBJ); PWD=$(TK8_OBJ); export PWD; $(MAKE) install)

$(TCL8_OBJ): $(TCLTK_OBJ)
	@if [ ! -d $@ ]; then echo "Making directory $@"; mkdir -p $@; fi
$(TK8_OBJ): $(TCLTK_OBJ)
	@if [ ! -d $@ ]; then echo "Making directory $@"; mkdir -p $@; fi
$(TCL8_DEST): $(TCLTK_DEST)
	@if [ ! -d $@ ]; then echo "Making directory $@"; mkdir -p $@; fi
$(TCL8_INDEPENDENT_DEST): $(TCLTK_DEST)
	@if [ ! -d $@ ]; then echo "Making directory $@"; mkdir -p $@; fi
$(TCLTK_DEST):
	@if [ ! -d $@ ]; then echo "Making directory $@"; mkdir -p $@; fi

######################################################################
# Rules for Itcl
# Itcl is an extension for tcl that provides tcl 'classes'
# The Itcl distribution contains tcl and tk

# Configure itcl
FORCE:
$(PTOLEMY)/src/tcltk/tcl_shared: FORCE
	-chmod a+x $@

$(ITCL_OBJ)/config.status: configure_itcl

configure_itcl: $(ITCL_SRC)/configure \
		$(ITCL_SRC)/itcl/unix/configure \
		$(ITCL_SRC)/itk/unix/configure \
		$(ITCL_SRC)/iwidgets$(IWIDGETS_MAJOR_VERSION_NUMBER)/unix/configure \
		$(ITCL_SRC)/iwidgets2.2.0/unix/configure \
		$(TCLTK_OBJ) $(ITCL_OBJ) \
		$(ITCL_OBJ)/itcl $(ITCL_OBJ)/itk $(IWIDGETS_OBJ) \
		$(ITCL_OBJ)/iwidgets2.2.0
	@echo "Configuring itcl"
	(cd $(ITCL_OBJ); PWD=$(ITCL_OBJ); export PWD; \
	$(ITCL_SRC)/configure \
		--srcdir=$(ITCL_SRC) \
		--prefix=$(TCL8_INDEPENDENT_DEST) \
		--exec-prefix=$(TCL8_DEST) \
		$(ENABLE_SHARED) $(ENABLE_GCC))

# We may need to run configure, esp. if we checked out from cvs
$(ITCL_SRC)/configure: $(ITCL_SRC)/configure.in
	(cd $(ITCL_SRC); autoconf)
$(ITCL_SRC)/itcl/unix/configure: $(ITCL_SRC)/itcl/unix/configure.in
	(cd $(ITCL_SRC)/itcl/unix; autoconf)
$(ITCL_SRC)/itk/unix/configure: $(ITCL_SRC)/itk/unix/configure.in
	(cd $(ITCL_SRC)/itk/unix; autoconf)
$(ITCL_SRC)/iwidgets$(IWIDGETS_MAJOR_VERSION_NUMBER)/unix/configure: \
	$(ITCL_SRC)/iwidgets$(IWIDGETS_MAJOR_VERSION_NUMBER)/unix/configure.in
	(cd $(ITCL_SRC)/iwidgets$(IWIDGETS_MAJOR_VERSION_NUMBER)/unix; \
		autoconf);
$(ITCL_SRC)/iwidgets2.2.0/unix/configure: \
	$(ITCL_SRC)/iwidgets2.2.0/unix/configure.in
	(cd $(ITCL_SRC)/iwidgets2.2.0/unix; \
		autoconf);

$(TCLTK_OBJ): 
	@if [ ! -d $@ ]; then echo "Making directory $@"; mkdir $@; fi
$(ITCL_OBJ): $(TCLTK_OBJ)
	@if [ ! -d $@ ]; then echo "Making directory $@"; mkdir $@; fi
$(ITCL_OBJ)/itcl: $(ITCL_OBJ)
	@if [ ! -d $@ ]; then echo "Making directory $@"; mkdir $@; fi
$(ITCL_OBJ)/itk: $(ITCL_OBJ)
	@if [ ! -d $@ ]; then echo "Making directory $@"; mkdir $@; fi
$(IWIDGETS_OBJ): $(ITCL_OBJ)
	@if [ ! -d $@ ]; then echo "Making directory $@"; mkdir $@; fi
$(ITCL_OBJ)/iwidgets2.2.0:  $(ITCL_OBJ)
	@if [ ! -d $@ ]; then echo "Making directory $@"; mkdir $@; fi

$(TCLTK_OBJ)/makefile:
	ln -s $(TCLTK_SRC)/makefile $@

itcl: $(ITCL_OBJ) $(ITCL_OBJ)/libitcl.a 
$(ITCL_OBJ)/libitcl.a: $(ITCL_OBJ) 
	@echo "Making all in itcl"
	(cd  $(ITCL_OBJ); $(MAKE) \
		CFLAGS="$(CFLAGS) $(WRITABLE_STRINGS_CFLAGS)" \
		X11_INCLUDES="$(X11_INCSPEC)" \
		X11_LIB_SWITCHES="$(X11_LIBSPEC)" \
		all)

# Install itcl
# In Itcl3.0, itk.h includes itclInt.h which includes 
# tclInt.h and tclRegexp.h
install_itcl: $(ITCL_DEST_LIB)/libitcl.a fixlinks \
			$(TCL8_INDEPENDENT_DEST)/include/itclInt.h \
			$(TCL8_INDEPENDENT_DEST)/include/tclInt.h \
			$(TCL8_INDEPENDENT_DEST)/include/tclRegexp.h
	-(cd $(BINDIR); ln -s $(REL_ITCL_BINDIR)/* .)

$(TCL8_INDEPENDENT_DEST)/include/itclInt.h:
	if [ ! -f $@ ]; then \
		echo "Copying itclInt.h"; \
		cp $(ITCL_SRC)/itcl/generic/itclInt.h $@; \
	fi

$(TCL8_INDEPENDENT_DEST)/include/tclInt.h:
	if [ ! -f $@ ]; then \
		echo "Copying tclInt.h"; \
		cp $(TCL8_SRC)/../generic/tclInt.h $@; \
	fi

$(TCL8_INDEPENDENT_DEST)/include/tclRegexp.h:
	if [ ! -f $@ ]; then \
		echo "Copying tclRegexp.h"; \
		cp $(TCL8_SRC)/../generic/tclRegexp.h $@; \
	fi

$(ITCL_DEST_LIB): $(TCL8_DEST)
	@if [ ! -d $@ ]; then echo "Making directory $@"; mkdir $@; fi

$(ITCL_DEST_LIB)/libitcl.a: $(ITCL_DEST_LIB)
	@echo "Making install in itcl"
	-chmod a+x $(ITCL_SRC)/config/install-sh \
		$(ITCL_SRC)/config/mkinstalldirs
	(cd  $(ITCL_OBJ); $(MAKE) \
		CFLAGS="$(CFLAGS) $(WRITABLE_STRINGS_CFLAGS)" \
		X11_INCLUDES="$(X11_INCSPEC)" \
		X11_LIB_SWITCHES="$(X11_LIBSPEC)" \
		install)
fixlinks:
	@echo "Create unversioned links for arch independent libraries"
	-(cd $(TCL8_INDEPENDENT_DEST)/lib; \
		rm -f tcl tk itcl itk iwidgets; \
		ln -s tcl$(TCLTK_MAJOR_VERSION_NUMBER) tcl; \
		ln -s tk$(TCLTK_MAJOR_VERSION_NUMBER) tk; \
		ln -s itcl$(ITCL_MAJOR_VERSION_NUMBER) itcl; \
		ln -s itk$(ITCL_MAJOR_VERSION_NUMBER) itk; \
		ln -s iwidgets$(IWIDGETS_MAJOR_VERSION_NUMBER) iwidgets )
	@echo "Create links for systems that require shared lib versioning"
	@echo " such as sun4 and freebsd"
	-(cd $(ITCL_DEST_LIB); \
		if [ -f $(ITCL_DEST_LIB)/itcl/libtcl$(TCL_VERSION_NUM).so.1.0 ]; then \
			echo "Hack for sun4 versioned shared libs"; \
			LIBVERSION=.1.0; \
			LIBSUFFIX=so; \
		else \
			LIBVERSION=""; \
			LIBSUFFIX=$(LIBSUFFIX); \
		fi; \
		rm -f libtcl.$$LIBSUFFIX libtk.$$LIBSUFFIX libitcl.$$LIBSUFFIX libitk.$$LIBSUFFIX; \
		ln -s libtcl$(TCL_VERSION_NUM).$$LIBSUFFIX$$LIBVERSION libtcl.$$LIBSUFFIX; \
		ln -s libtk$(TK_VERSION_NUM).$$LIBSUFFIX$$LIBVERSION libtk.$$LIBSUFFIX; \
		ln -s libitcl$(ITCL_VERSION_NUM).$$LIBSUFFIX$$LIBVERSION libitcl.$$LIBSUFFIX; \
		ln -s libitk$(ITCL_VERSION_NUM).$$LIBSUFFIX$$LIBVERSION libitk.$$LIBSUFFIX; \
	)
	@echo "Create a links for itclsh"
	-(cd $(TCL8_DEST)/bin; rm -f tclsh itkwish itclsh wish \
		tclsh$(ITCL_WINDOWS_TCL_MAJOR_VERSION_NUMBER) \
		wish$(ITCL_WINDOWS_TK_MAJOR_VERSION_NUMBER))
	-(cd $(TCL8_DEST)/bin; \
		ln -s itkwish$(ITCL_MAJOR_VERSION_NUMBER) itkwish)
	-(cd $(TCL8_DEST)/bin; \
		ln -s itclsh$(ITCL_MAJOR_VERSION_NUMBER) itclsh)
	-(cd $(TCL8_DEST)/bin; \
		ln -s tclsh$(TCLTK_MAJOR_VERSION_NUMBER) tclsh)
	-(cd $(TCL8_DEST)/bin; \
		ln -s tclsh$(TCLTK_MAJOR_VERSION_NUMBER) \
			tclsh$(WINDOWS_TCLTK_MAJOR_VERSION_NUMBER))
	-(cd $(TCL8_DEST)/bin; \
		ln -s wish$(TCLTK_MAJOR_VERSION_NUMBER) wish)
	-(cd $(TCL8_DEST)/bin; \
		ln -s wish$(TCLTK_MAJOR_VERSION_NUMBER) \
			wish$(WINDOWS_TCLTK_MAJOR_VERSION_NUMBER))

######################################################
# TclBlend

TCLBLEND_MAJOR_VERSION = 1.0
TCLBLEND_MINOR_VERSION =
TCLBLEND_VERSION = $(TCLBLEND_MAJOR_VERSION)$(TCLBLEND_MINOR_VERSION)

TCLBLEND_SRC =	$(TCLTK_SRC)/tclBlend$(TCLBLEND_VERSION)/unix
TCLBLEND_OBJ =	$(TCLTK_OBJ)/tclBlend$(TCLBLEND_VERSION)
TCLBLEND_DEST =	$(TCL8_DEST)
TCLBLEND_INDEPENDENT_DEST =	$(TCL8_INDEPENDENT_DEST)

# Script that checks for Java Studio
JAVA_STUDIO_CONFIG=`$(PTOLEMY)/src/tcltk/java_studio`

all_tclblend: configure_tclblend tclblend \
		install_tclblend fixlinks_tclblend
configure_tclblend: $(TCLBLEND_OBJ)
	@echo "Configuring tclblend"
	(cd $(TCLBLEND_OBJ); PWD=$(TCLBLEND_OBJ); export PWD; \
		$(TCLBLEND_SRC)/configure \
		--srcdir=$(TCLBLEND_SRC) \
		--with-tcl=$(TCL8_OBJ) \
		--prefix=$(TCLBLEND_INDEPENDENT_DEST) \
		--exec-prefix=$(TCLBLEND_DEST) \
		$(JAVA_STUDIO_CONFIG) \
		$(ENABLE_SHARED) $(ENABLE_GCC))

tclblend: #$(TCLBLEND_OBJ)/pkgIndex.tcl
	(cd $(TCLBLEND_OBJ); PWD=$(TCLBLEND_OBJ); export PWD; $(MAKE) all)

#$(TCLBLEND_OBJ)/pkgIndex.tcl: $(TCLBLEND_SRC)/pkgIndex.tcl
#	rm -f $@
#	cp $^ $@

install_tclblend:
	(cd $(TCLBLEND_OBJ); PWD=$(TCLBLEND_OBJ); export PWD; $(MAKE) install)

fixlinks_tclblend:
	-(cd $(BINDIR); ln -s $(REL_ITCL_BINDIR)/* .)

clean_tclblend:
	(cd $(TCLBLEND_OBJ); PWD=$(TCLBLEND_OBJ); export PWD; $(MAKE) clean)

test_tclblend:
	(cd $(TCLBLEND_OBJ); PWD=$(TCLBLEND_OBJ); export PWD; $(MAKE) test)

bean_tclblend:
	(cd $(TCLBLEND_OBJ); PWD=$(TCLBLEND_OBJ); export PWD; $(MAKE) bean)
tcljavaSrcDist_tclblend:
	(cd $(TCLBLEND_OBJ); PWD=$(TCLBLEND_OBJ); export PWD; \
		$(MAKE) tcljavaSrcDist DISTDIR=$(TCLBLEND_OBJ)/dist)
# Run JavaScope, the Java Code Coverage tool.
# See http://www.suntest.com/JavaScope
jsall_tclblend:
	(cd $(TCLBLEND_OBJ); PWD=$(TCLBLEND_OBJ); export PWD; $(MAKE) jsall)
jstest_tclblend:
	(cd $(TCLBLEND_OBJ); PWD=$(TCLBLEND_OBJ); export PWD; $(MAKE) jstclblendTest)
jsrestore_tclblend:
	(cd $(TCLBLEND_OBJ); PWD=$(TCLBLEND_OBJ); export PWD; $(MAKE) jsrestore)


time_tclblend:
	@echo "Be sure to run 'make test_jacl' first"
	(cd $(JACL_OBJ); \
		(echo lappend auto_path [pwd]\; \
		cd $(JACL_SRC)/../tests\; \
		source $(TCLBLEND_SRC)/../tests/time\; printStats\; exit\; )| \
	CLASSPATH=$(JACL_OBJ)/jacl:$(JACL_OBJ)/jacl/tests:$(JACL_OBJ)/jacl/tests/javaload:$(TCLBLEND_OBJ)/tclblend \
	time jtclsh)

$(TCLBLEND_OBJ): $(TCLTK_OBJ)
	@if [ ! -d $@ ]; then echo "Making directory $@"; mkdir $@; fi

######################################################
# Jacl

JACL_MAJOR_VERSION = 1.1
JACL_MINOR_VERSION = b1
JACL_VERSION = $(JACL_MAJOR_VERSION)$(JACL_MINOR_VERSION)

JACL_SRC =	$(TCLTK_SRC)/jacl$(JACL_VERSION)/unix
JACL_OBJ =	$(TCLTK_OBJ)/jacl$(JACL_VERSION)
JACL_DEST =	$(TCL8_DEST)
JACL_INDEPENDENT_DEST =	$(TCL8_INDEPENDENT_DEST)

all_jacl: configure_jacl jacl install_jacl
configure_jacl: $(JACL_OBJ)
	@echo "Configuring jacl"
	(cd $(JACL_OBJ); PWD=$(JACL_OBJ); export PWD; \
	$(JACL_SRC)/configure \
		--srcdir=$(JACL_SRC) \
		--prefix=$(JACL_INDEPENDENT_DEST))


jacl:
	(cd $(JACL_OBJ); PWD=$(JACL_OBJ); export PWD; $(MAKE))
	(cd $(JACL_OBJ); PWD=$(JACL_OBJ); export PWD; $(MAKE) jacl.jar)
install_jacl:
	(cd $(JACL_OBJ); PWD=$(JACL_OBJ); export PWD; $(MAKE) install)

test_jacl:
	(cd $(JACL_OBJ); PWD=$(JACL_OBJ); export PWD; $(MAKE) test)
time_jacl:
	@echo "Be sure to run 'make test_jacl' first"
	(cd $(JACL_OBJ); \
		(echo lappend auto_path [pwd]\; \
		cd $(JACL_SRC)/../tests\; \
		source $(TCLBLEND_SRC)/../tests/time\; printStats\; exit\; )| \
	CLASSPATH=$(JACL_OBJ)/jacl:$(JACL_OBJ)/jacl/tests:$(JACL_OBJ)/jacl/tests/javaload:$(TCLBLEND_OBJ)/tclblend \
	time java tcl.lang.Shell)

$(JACL_OBJ): $(TCLTK_OBJ)
	@if [ ! -d $@ ]; then echo "Making directory $@"; mkdir $@; fi



######################################################
# Tcljava
# Tcljava is the combined Tcl Blend and Jacl source tree which is used
# to create the individual Tcl Blend and Jacl tar files.
# You will only need to run these rules if you are building 
# Tcl Blend and Jacl releases.

TCLJAVA_SRC =	$(TCLTK_SRC)/tcljava$(TCLBLEND_VERSION)/unix
TCLJAVA_OBJ =	$(TCLTK_OBJ)/tcljava$(TCLBLEND_VERSION)

# Script that checks for Java Studio
JAVA_STUDIO_CONFIG=`$(PTOLEMY)/src/tcltk/java_studio`

all_tcljava: configure_tcljava tcljava \
		install_tcljava fixlinks_tcljava
configure_tcljava: $(TCLJAVA_OBJ)
	@echo "Configuring tcljava"
	(cd $(TCLJAVA_OBJ); PWD=$(TCLJAVA_OBJ); export PWD; \
		$(TCLJAVA_SRC)/configure \
		--srcdir=$(TCLJAVA_SRC) \
		--with-tcl=$(TCL8_OBJ) \
		--prefix=$(TCLBLEND_INDEPENDENT_DEST) \
		--exec-prefix=$(TCLBLEND_DEST) \
		$(JAVA_STUDIO_CONFIG) \
		$(ENABLE_SHARED) $(ENABLE_GCC))

tcljava:
	(cd $(TCLJAVA_OBJ); PWD=$(TCLJAVA_OBJ); export PWD; $(MAKE) all)

install_tcljava:
	(cd $(TCLJAVA_OBJ); PWD=$(TCLJAVA_OBJ); export PWD; $(MAKE) install)

fixlinks_tcljava:
	-(cd $(BINDIR); ln -s $(REL_ITCL_BINDIR)/* .)

clean_tcljava:
	(cd $(TCLJAVA_OBJ); PWD=$(TCLJAVA_OBJ); export PWD; $(MAKE) clean)

test_tcljava:
	(cd $(TCLJAVA_OBJ); PWD=$(TCLJAVA_OBJ); export PWD; $(MAKE) test)

dist_tcljava:
	(cd $(TCLJAVA_OBJ); PWD=$(TCLJAVA_OBJ); export PWD; $(MAKE) dist)


$(TCLJAVA_OBJ): $(TCLTK_OBJ)
	@if [ ! -d $@ ]; then echo "Making directory $@"; mkdir $@; fi

#######################
clean_itcl:
	(cd $(ITCL_OBJ); $(MAKE) clean)



