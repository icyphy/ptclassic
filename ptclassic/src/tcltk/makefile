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

# The default action is to build itcl2.1 with shared libraries

# You should not need to edit this file, merely set the
# PTARCH and PTOLEMY environment variables, for example:
#	setenv PTARCH sun4
#	setenv PTOLEMY /users/ptolemy

# root of source tree
ROOT	      = ../..
# Path (absolute or relative) to the object directory root
OBJDIR=..
# VPATH points to the "real" source directory
VPATH         = $(ROOT)/src/pigiRpc

# Get configuration info
# Need $(C_SHAREDFLAGS) and $(CSHARED_LIBRARY_COMMAND) below
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
ENABLE_SHARED =		--enable-shared

# Used to find the itcl source directory
ITCL_VERSION =		itcl2.2
ITCL_TCL_MAJOR_VERSION_NUMBER =	7.6
ITCL_TK_MAJOR_VERSION_NUMBER =	4.2
ITCL_MAJOR_VERSION_NUMBER =	2.2
IWIDGETS_MAJOR_VERSION_NUMBER =	2.2.0

# TclX
TCLX_VERSION =		tclX7.5.3-a1
TCLX_SRC =		$(TCLTK_SRC)/$(ITCL_VERSION).$(TCLX_VERSION)/$(TCLX_VERSION)
TCLX_OBJ =		$(TCLTK_OBJ)/$(ITCL_VERSION)/$(TCLX_VERSION)/unix

# BLT
BLT_VERSION =		blt2.0
BLT_SRC =		$(TCLTK_SRC)/$(ITCL_VERSION).$(TCLX_VERSION)/$(BLT_VERSION)
BLT_OBJ =		$(TCLTK_OBJ)/$(ITCL_VERSION)/$(BLT_VERSION)

# TCLDEBUG
TCLDEBUG_VERSION =	tcl-debug-1.5
TCLDEBUG_SRC =		$(TCLTK_SRC)/$(ITCL_VERSION).$(TCLX_VERSION)/$(TCLDEBUG_VERSION)
TCLDEBUG_OBJ =		$(TCLTK_OBJ)/$(ITCL_VERSION)/$(TCLDEBUG_VERSION)

# Location of the itcl source
#ITCL_SRC =	$(TCLTK_SRC)/$(ITCL_VERSION).$(TCLX_VERSION)
ITCL_SRC =	$(TCLTK_SRC)/$(ITCL_VERSION)

# Location of the itcl only object files
ITCL_OBJ =	$(TCLTK_OBJ)/$(ITCL_VERSION)
TCL_OBJ =	$(ITCL_OBJ)/tcl$(ITCL_TCL_MAJOR_VERSION_NUMBER)
TK_OBJ =	$(ITCL_OBJ)/tk$(ITCL_TK_MAJOR_VERSION_NUMBER)
IWIDGETS_OBJ =	$(ITCL_OBJ)/iwidgets$(IWIDGETS_MAJOR_VERSION_NUMBER)

# Destination of itcl only machine dependent files
ITCL_DEST_BASE =	itcl.$(PTARCH)
ITCL_DEST =	$(TCLTK_DEST)/$(ITCL_DEST_BASE)

# Location of itcl bin directory relative to Ptolemy bin directory
# Can't use absolute path name here, or we will create a distribution that
# will not work if $PTOLEMY != /users/ptolemy
# Since itcl2.0 places the tcl, tk, and itcl binaries in one directory,
# we get everything in one swoop here.
REL_ITCL_BINDIR =	../tcltk/$(ITCL_DEST_BASE)/bin

# Destination of the machine independent itcl files
ITCL_INDEPENDENT_DEST =		$(TCLTK_DEST)/itcl

ITCL_DEST_LIB = $(ITCL_DEST)/lib

# itcl2.1 contains its own custom tcltk
all: 		configure itcl
configure:	$(ITCL_OBJ)/config.status $(TCLTK_OBJ)/makefile
bin:		itcl
install:	install_itcl
clean:		clean_itcl
realclean:	clean_itcl

######################################################################
# Rules for itcl
# itcl is an extension for tcl that provides tcl 'classes'
# The itcl2.1 distribution contains tcl and tk

# Configure itcl
$(ITCL_OBJ)/config.status: config_itcl

config_itcl: $(TCLTK_OBJ) $(ITCL_OBJ)  $(TCL_OBJ) $(TK_OBJ) $(ITCL_OBJ)/itcl $(ITCL_OBJ)/itk $(IWIDGETS_OBJ)
	@echo "Configuring itcl"
	(cd $(ITCL_OBJ); PWD=$(ITCL_OBJ); export PWD; \
	$(ITCL_SRC)/configure \
		--srcdir=$(ITCL_SRC) \
		--prefix=$(ITCL_INDEPENDENT_DEST) \
		--exec-prefix=$(ITCL_DEST) \
		$(ENABLE_SHARED) $(ENABLE_GCC))

$(TCLTK_OBJ): 
	@if [ ! -d $@ ]; then echo "Making $@"; mkdir $@; fi

$(ITCL_OBJ): $(TCLTK_OBJ)
	@if [ ! -d $@ ]; then echo "Making $@"; mkdir $@; fi

$(TCL_OBJ): $(ITCL_OBJ)
	@if [ ! -d $@ ]; then echo "Making $@"; mkdir $@; fi

$(TK_OBJ): $(ITCL_OBJ)
	@if [ ! -d $@ ]; then echo "Making $@"; mkdir $@; fi

$(ITCL_OBJ)/itcl: $(ITCL_OBJ)
	@if [ ! -d $@ ]; then echo "Making $@"; mkdir $@; fi

$(ITCL_OBJ)/itk: $(ITCL_OBJ)
	@if [ ! -d $@ ]; then echo "Making $@"; mkdir $@; fi

$(IWIDGETS_OBJ): $(ITCL_OBJ)
	@if [ ! -d $@ ]; then echo "Making $@"; mkdir $@; fi

itcl: $(ITCL_OBJ) $(ITCL_OBJ)/libitcl.a 
$(ITCL_OBJ)/libitcl.a: $(ITCL_OBJ) 
	@echo "Making all in itcl"
	(cd  $(ITCL_OBJ); $(MAKE) \
		CFLAGS="$(CFLAGS) $(WRITABLE_STRINGS_CFLAGS)" \
		X11_INCLUDES="$(X11_INCSPEC)" \
		X11_LIB_SWITCHES="$(X11_LIBSPEC)" \
		all)

# Install itcl
install_itcl: $(ITCL_DEST_LIB)/libitcl.a fixlinks
	-(cd $(BINDIR); ln -s $(REL_ITCL_BINDIR)/* .)

$(ITCL_DEST_LIB)/libitcl.a:
	@echo "Making install in itcl"
	(cd  $(ITCL_OBJ); $(MAKE) \
		CFLAGS="$(CFLAGS) $(WRITABLE_STRINGS_CFLAGS)" \
		X11_INCLUDES="$(X11_INCSPEC)" \
		X11_LIB_SWITCHES="$(X11_LIBSPEC)" \
		install)
fixlinks:
	@echo "Create unversioned links for arch independent libraries"
	-(cd $(ITCL_INDEPENDENT_DEST)/lib/itcl; \
		rm -f tcl tk itcl itk iwidgets; \
		ln -s tcl$(ITCL_TCL_MAJOR_VERSION_NUMBER) tcl; \
		ln -s tk$(ITCL_TK_MAJOR_VERSION_NUMBER) tk; \
		ln -s itcl$(ITCL_MAJOR_VERSION_NUMBER) itcl; \
		ln -s itk$(ITCL_MAJOR_VERSION_NUMBER) itk; \
		ln -s iwidgets$(IWIDGETS_MAJOR_VERSION_NUMBER) iwidgets )
	#@echo "Create links for the itcl2.1 lib files in the directory"
	#@echo " where itcl2.0 lib files were"
	#-(cd $(ITCL_DEST)/lib; ln -s itcl/* .)
	@echo "Create links for systems that require shared lib versioning"
	@echo " such as sun4 and freebsd"
	-(cd $(ITCL_DEST)/lib/itcl; \
		if [ -f $(ITCL_DEST)/lib/itcl/libtcl$(TCL_VERSION_NUM).so.1.0 ]; then \
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
		ln -s libitcl$(ITCL_VERSION_NUM).$$LIBSUFFIX$$LIBVERSION libitcl.$$LIBVERSION; \
		ln -s libitk$(ITCL_VERSION_NUM).$$LIBSUFFIX$$LIBVERSION libitk.$$LIBVERSION; \
	)
	@echo "Create a links for itclsh to ish"
	-(cd $(ITCL_DEST)/bin; rm -f tclsh itkwish itclsh)
	-(cd $(ITCL_DEST)/bin; ln -s ish$(ITCL_TCL_MAJOR_VERSION_NUMBER) tclsh)
	-(cd $(ITCL_DEST)/bin; \
		ln -s itkwish$(ITCL_MAJOR_VERSION_NUMBER) itkwish)
	-(cd $(ITCL_DEST)/bin; \
		ln -s itclsh$(ITCL_MAJOR_VERSION_NUMBER) itclsh)
	@echo "Create links so Ptolemy can find tcl.h and tk.h easily"
	-(cd $(TCLTK_DEST); mkdir tcl tk; \
		cd tcl; ln -s ../itcl/include/itcl include; \
		cd ../tk; ln -s ../itcl/include/itcl include; \
		cd ../itcl/include; ln -s itcl/*.h .)

######################################################
# TclX rules
# we use TclX for the profile command
all_tclx: all config_tclx tclx

# FIXME: configure fails with GNU make!  try set path=(`pwd` $path)
config_tclx: $(TCLX_OBJ)	
	( cd $(TCLX_OBJ); \
		$(TCLX_SRC)/unix/configure \
		--srcdir=$(TCLX_SRC)/unix \
		--prefix=$(ITCL_INDEPENDENT_DEST) \
		--exec-prefix=$(ITCL_DEST) \
		$(ENABLE_SHARED) $(ENABLE_GCC))

# TclX demands that we run the dynamic loading tests in the tcl directory
# so we do that before actually building
# The tcl dl test:
DLTEST=$(ITCL_OBJ)/tcl$(ITCL_TCL_MAJOR_VERSION_NUMBER)/unix/dltest/pkgIndex.tcl
$(DLTEST):
	(cd $(ITCL_OBJ)/tcl$(ITCL_TCL_MAJOR_VERSION_NUMBER)/unix; \
		$(MAKE) dltest/pkgIndex.tcl)

tclx: $(DLTEST)
	@echo "Making all in tclx"
	rm -f $(TCLX_OBJ)/tcl/unix/dltest
	(cd  $(TCLX_OBJ); \
		$(MAKE) \
		CFLAGS="$(CFLAGS) $(WRITABLE_STRINGS_CFLAGS)" \
		X11_INCLUDES="$(X11_INCSPEC)" \
		X11_LIB_SWITCHES="$(X11_LIBSPEC)" \
		all)

install_tclx:
	@echo "Making install in itcl"
	(cd  $(TCLX_OBJ); $(MAKE) \
		CFLAGS="$(CFLAGS) $(WRITABLE_STRINGS_CFLAGS)" \
		X11_INCLUDES="$(X11_INCSPEC)" \
		X11_LIB_SWITCHES="$(X11_LIBSPEC)" \
		install)
	-(cd $(BINDIR); ln -s $(REL_ITCL_BINDIR)/* .)

$(TCLX_OBJ):
	mkdir -p $@

######################################################
# Blt rules
all_blt: all config_blt blt

config_blt: $(BLT_OBJ)	
	( cd $(BLT_OBJ); PWD=$(ITCL_OBJ); export PWD; \
		TKLIBDIR=$(ITCL_DEST_LIB)/itcl; export TKLIBDIR; \
		TCLLIBDIR=$(ITCL_DEST_LIB)/itcl; export TCLLIBDIR; \
		$(BLT_SRC)/configure \
		--srcdir=$(BLT_SRC) \
		--prefix=$(ITCL_INDEPENDENT_DEST) \
		--exec-prefix=$(ITCL_DEST) \
		$(ENABLE_SHARED) --with-cc=gcc)

blt: 
	@echo "Making all in blt"
	(cd  $(BLT_OBJ); \
		$(MAKE) \
		CC="$(CC)" \
		CFLAGS="$(CFLAGS) $(WRITABLE_STRINGS_CFLAGS)" \
		X11_INCLUDES="$(X11_INCSPEC)" \
		X11_LIB_SWITCHES="$(X11_LIBSPEC)" \
		all)

install_blt:
	@echo "Making install in itcl"
	(cd  $(BLT_OBJ); $(MAKE) \
		CFLAGS="$(CFLAGS) $(WRITABLE_STRINGS_CFLAGS)" \
		X11_INCLUDES="$(X11_INCSPEC)" \
		X11_LIB_SWITCHES="$(X11_LIBSPEC)" \
		install)
	-(cd $(BINDIR); ln -s $(REL_ITCL_BINDIR)/* .)
$(BLT_OBJ):
	mkdir -p $@

######################################################
# Tcldebug rules
all_tcldebug: all config_tcldebug tcldebug

config_tcldebug: $(TCLDEBUG_OBJ)	
	( cd $(TCLDEBUG_OBJ); PWD=$(ITCL_OBJ); export PWD; \
		$(TCLDEBUG_SRC)/configure \
		--srcdir=$(TCLDEBUG_SRC) \
		--prefix=$(ITCL_INDEPENDENT_DEST) \
		--exec-prefix=$(ITCL_DEST) \
		$(ENABLE_GCC) $(ENABLE_SHARED) )

tcldebug: 
	@echo "Making all in tcldebug"
	# Don't pass CFLAGS down or you will end up overriding
	# -fPIC in tcl-debug-1.5/Makefile
	(cd  $(TCLDEBUG_OBJ); \
		$(MAKE) \
		CC="$(CC)" \
		X11_INCLUDES="$(X11_INCSPEC)" \
		X11_LIB_SWITCHES="$(X11_LIBSPEC)" \
		all)

install_tcldebug:
	@echo "Making install in itcl"
	(cd  $(TCLDEBUG_OBJ); $(MAKE) \
		CFLAGS="$(CFLAGS) $(WRITABLE_STRINGS_CFLAGS)" \
		X11_INCLUDES="$(X11_INCSPEC)" \
		X11_LIB_SWITCHES="$(X11_LIBSPEC)" \
		install)
	-(cd $(BINDIR); ln -s $(REL_ITCL_BINDIR)/* .)
$(TCLDEBUG_OBJ):
	mkdir -p $@

clean_tcldebug:
	(cd  $(TCLDEBUG_OBJ); $(MAKE) clean)

# Links so that older programs can find libs and includes.
# Note that itcl contains its own tcl/tk so we have to make links
$(TCL_DEST):
	(cd $(TCLTK_DEST); ln -s itcl.$(PTARCH) tcl.$(PTARCH))
	#mkdir $@
$(TCL_INDEPENDENT_DEST):
	(cd $(TCLTK_DEST); ln -s itcl tcl)
	#mkdir $@
$(TCL_DEST)/lib:
	mkdir $@
$(TK_DEST):
	(cd $(TCLTK_DEST); ln -s itcl.$(PTARCH) tk.$(PTARCH))
	#mkdir $@
$(TK_INDEPENDENT_DEST):
	(cd $(TCLTK_DEST); ln -s itcl tk)
	#mkdir $@

clean_itcl:
	(cd $(ITCL_OBJ); $(MAKE) clean)

#######################
$(TCLTK_OBJ)/makefile:
	ln -s $(TCLTK_SRC)/makefile $@
