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
# If you build Itcl with shared libraries on sun4, then	
# incremental linking of stars will fail with a message from ld
# about how it cannot find -litk22
ENABLE_SHARED =		`$(PTOLEMY)/src/tcltk/tcl_shared`

# Used to find the itcl source directory
ITCL_VERSION =		itcl2.2
ITCL_TCL_MAJOR_VERSION_NUMBER =	7.6
ITCL_TK_MAJOR_VERSION_NUMBER =	4.2
ITCL_MAJOR_VERSION_NUMBER =	2.2
IWIDGETS_MAJOR_VERSION_NUMBER =	2.2.0

#ITCL_VERSION =		itcl3.0a1
#ITCL_TCL_MAJOR_VERSION_NUMBER =	8.0
#ITCL_TK_MAJOR_VERSION_NUMBER =	8.0
#ITCL_MAJOR_VERSION_NUMBER =	3.0
#IWIDGETS_MAJOR_VERSION_NUMBER =	3.0.0

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
configure:	$(PTOLEMY)/src/tcltk/tcl_shared \
			$(ITCL_OBJ)/config.status $(TCLTK_OBJ)/makefile
bin:		itcl
install:	install_itcl
clean:		clean_itcl
realclean:	clean_itcl

######################################################################
# Rules for itcl
# itcl is an extension for tcl that provides tcl 'classes'
# The itcl2.1 distribution contains tcl and tk

# Configure itcl
FORCE:
$(PTOLEMY)/src/tcltk/tcl_shared: FORCE
	-chmod a+x $@

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
		ln -s libitcl$(ITCL_VERSION_NUM).$$LIBSUFFIX$$LIBVERSION libitcl.$$LIBSUFFIX; \
		ln -s libitk$(ITCL_VERSION_NUM).$$LIBSUFFIX$$LIBVERSION libitk.$$LIBSUFFIX; \
	)
	@echo "Create a links for itclsh to ish"
	-(cd $(ITCL_DEST)/bin; rm -f ish tclsh itkwish itclsh)
	-(cd $(ITCL_DEST)/bin; ln -s ish$(ITCL_TCL_MAJOR_VERSION_NUMBER) ish)
	-(cd $(ITCL_DEST)/bin; ln -s ish$(ITCL_TCL_MAJOR_VERSION_NUMBER) tclsh)
	-(cd $(ITCL_DEST)/bin; \
		ln -s itkwish$(ITCL_MAJOR_VERSION_NUMBER) itkwish)
	-(cd $(ITCL_DEST)/bin; \
		ln -s itclsh$(ITCL_MAJOR_VERSION_NUMBER) itclsh)
	@echo "Create links so Ptolemy can find tcl.h and tk.h easily"
	-(cd $(TCLTK_DEST); rm -rf tcl tk; mkdir tcl tk; \
		cd tcl; ln -s ../itcl/include/itcl include; \
		cd ../tk; ln -s ../itcl/include/itcl include; \
		cd ../itcl/include; \
		rm -f `cd itcl && echo *.h`; ln -s itcl/*.h .)

######################################################
# Tcl8/Tk8
# This is experimental, and should go away when Itcl3.0 is out

TCLTK8_VERSION = 8.0
TCL8_SRC =	$(TCLTK_SRC)/tcl$(TCLTK8_VERSION)/unix
TK8_SRC =	$(TCLTK_SRC)/tk$(TCLTK8_VERSION)/unix
TCL8_OBJ =	$(TCLTK_OBJ)/tcl$(TCLTK8_VERSION)
TK8_OBJ =	$(TCLTK_OBJ)/tk$(TCLTK8_VERSION)
TCL8_DEST_BASE =	tcl$(TCLTK8_VERSION).$(PTARCH)
TCL8_DEST =	$(TCLTK_DEST)/$(TCL8_DEST_BASE)
TCL8_INDEPENDENT_DEST =	$(TCLTK_DEST)/tcl$(TCLTK8_VERSION)

all_tcltk8: config_tcltk8 tcltk8 install_tcltk8
config_tcltk8: config_tcl8 config_tk8
config_tcl8: $(TCL8_OBJ)
	@echo "Configuring tcl8"
	(cd $(TCL8_OBJ); PWD=$(TCL8_OBJ); export PWD; \
	$(TCL8_SRC)/configure \
		--srcdir=$(TCL8_SRC) \
		--prefix=$(TCL8_INDEPENDENT_DEST) \
		--exec-prefix=$(TCL8_DEST) \
		$(ENABLE_SHARED) $(ENABLE_GCC))

config_tk8: $(TK8_OBJ)
	@echo "Configuring tk8"
	(cd $(TK8_OBJ); PWD=$(TK8_OBJ); export PWD; \
	$(TK8_SRC)/configure \
		--srcdir=$(TK8_SRC) \
		--with-tcl=$(TCL8_OBJ) \
		--prefix=$(TCL8_INDEPENDENT_DEST) \
		--exec-prefix=$(TCL8_DEST) \
		$(ENABLE_SHARED) $(ENABLE_GCC))

tcltk8:
	(cd $(TCL8_OBJ); PWD=$(TCL8_OBJ); export PWD; $(MAKE))
	(cd $(TK8_OBJ); PWD=$(TK8_OBJ); export PWD; $(MAKE))

install_tcltk8: $(TCL8_DEST) $(TCL8_INDEPENDENT_DEST)
	(cd $(TCL8_OBJ); PWD=$(TCL8_OBJ); export PWD; $(MAKE) install)
	(cd $(TK8_OBJ); PWD=$(TK8_OBJ); export PWD; $(MAKE) install)

$(TCL8_OBJ): $(TCLTK_OBJ)
	@if [ ! -d $@ ]; then echo "Making $@"; mkdir $@; fi
$(TK8_OBJ): $(TCLTK_OBJ)
	@if [ ! -d $@ ]; then echo "Making $@"; mkdir $@; fi
$(TCL8_DEST): $(TCLTK_DEST)
	@if [ ! -d $@ ]; then echo "Making $@"; mkdir $@; fi
$(TCL8_INDEPENDENT_DEST): $(TCLTK_DEST)
	@if [ ! -d $@ ]; then echo "Making $@"; mkdir $@; fi


######################################################
# TclBlend

TCLBLEND_MAJOR_VERSION = 1.0
TCLBLEND_MINOR_VERSION = up2
TCLBLEND_VERSION = $(TCLBLEND_MAJOR_VERSION)$(TCLBLEND_MINOR_VERSION)

TCLBLEND_SRC =	$(TCLTK_SRC)/tclBlend$(TCLBLEND_VERSION)/unix
TCLBLEND_OBJ =	$(TCLTK_OBJ)/tclBlend$(TCLBLEND_VERSION)
TCLBLEND_DEST =	$(TCL8_DEST)
TCLBLEND_INDEPENDENT_DEST =	$(TCL8_INDEPENDENT_DEST)

all_tclblend: config_tclblend tclblend install_tclblend
config_tclblend: $(TCLBLEND_OBJ)
	@echo "Configuring tclblend"
	(cd $(TCLBLEND_OBJ); PWD=$(TCLBLEND_OBJ); export PWD; \
		$(TCLBLEND_SRC)/configure \
		--srcdir=$(TCLBLEND_SRC) \
		--with-tcl=$(TCL8_OBJ) \
		--prefix=$(TCLBLEND_INDEPENDENT_DEST) \
		--exec-prefix=$(TCLBLEND_DEST) \
		$(ENABLE_SHARED) $(ENABLE_GCC))

tclblend:
	(cd $(TCLBLEND_OBJ); PWD=$(TCLBLEND_OBJ); export PWD; $(MAKE))

install_tclblend:
	(cd $(TCLBLEND_OBJ); PWD=$(TCLBLEND_OBJ); export PWD; $(MAKE) install)

test_tclblend:
	(cd $(TCLBLEND_OBJ); PWD=$(TCLBLEND_OBJ); export PWD; $(MAKE) test)

time_tclblend:
	@echo "Be sure to run 'make test_jacl' first"
	(cd $(JACL_OBJ); \
		(echo lappend auto_path [pwd]\; \
		cd $(JACL_SRC)/../tests\; \
		source $(TCLBLEND_SRC)/../tests/time\; printStats\; exit\; )| \
	CLASSPATH=$(JACL_OBJ)/jacl:$(JACL_OBJ)/jacl/tests:$(JACL_OBJ)/jacl/tests/javaload:$(TCLBLEND_OBJ)/tclblend \
	time jtclsh)

$(TCLBLEND_OBJ): $(TCLTK_OBJ)
	@if [ ! -d $@ ]; then echo "Making $@"; mkdir $@; fi

######################################################
# Jacl

JACL_MAJOR_VERSION = 1.0
JACL_MINOR_VERSION =
JACL_VERSION = $(JACL_MAJOR_VERSION)$(JACL_MINOR_VERSION)

JACL_SRC =	$(TCLTK_SRC)/jacl$(JACL_VERSION)/unix
JACL_OBJ =	$(TCLTK_OBJ)/jacl$(JACL_VERSION)
JACL_DEST =	$(TCL8_DEST)
JACL_INDEPENDENT_DEST =	$(TCL8_INDEPENDENT_DEST)

all_jacl: config_jacl jacl install_jacl
config_jacl: $(JACL_OBJ)
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
	@if [ ! -d $@ ]; then echo "Making $@"; mkdir $@; fi

######################################################
# Tcldp

TCLDP_MAJOR_VERSION = 4.0
TCLDP_MINOR_VERSION = 
TCLDP_VERSION = $(TCLDP_MAJOR_VERSION)$(TCLDP_MINOR_VERSION)

TCLDP_SRC =	$(TCLTK_SRC)/tcl-dp/unix
TCLDP_OBJ =	$(TCLTK_OBJ)/tcl-dp$(TCLDP_VERSION)
TCLDP_DEST =	$(TCLTK_DEST)/dp.$(PTARCH)
TCLDP_INDEPENDENT_DEST =	$(TCLTK_DEST)/dp

all_tcldp: config_tcldp tcldp install_tcldp
config_tcldp: $(TCLDP_OBJ)
	@echo "Configuring tcldp"
	(cd $(TCLDP_OBJ); PWD=$(TCLDP_OBJ); export PWD; \
		$(TCLDP_SRC)/configure \
		--srcdir=$(TCLDP_SRC) \
		--with-tcl=$(TCL_OBJ)/unix \
		--prefix=$(TCLDP_DEST) \
		$(ENABLE_SHARED) $(ENABLE_GCC))

tcldp:
	(cd $(TCLDP_OBJ); PWD=$(TCLDP_OBJ); export PWD; $(MAKE))

# We are not handling separate prefix and exec_prefix directories here.
install_tcldp: $(TCLDP_DEST) $(TCLDP_DEST)/pkgIndex.tcl \
		$(TCLDP_DEST)/init.tcl
	# We should use install-sh or something here.
	cp $(TCLDP_OBJ)/libdp40.so $<
	cp $(TCLDP_SRC)/../library/*.tcl $<


$(TCLDP_DEST)/pkgIndex.tcl: $(TCLDP_DEST)
	echo "package ifneeded dp 4.0 [list source [file join \$$dir init.tcl]]" > $@"

$(TCLDP_DEST)/init.tcl: $(TCLDP_DEST)
	echo "set dir $(TCLDP_DEST)" > $@
	echo "load [file join \$$dir libdp40[info sharedlibextension]]" >> $@
	echo "source [file join \$$dir acl.tcl]" >> $@
	echo "source [file join \$$dir distribObj.tcl]" >> $@
	echo "source [file join \$$dir dp_atclose.tcl]" >> $@
	echo "source [file join \$$dir dp_atexit.tcl]" >> $@
	echo "source [file join \$$dir ldelete.tcl]" >> $@
	echo "source [file join \$$dir oo.tcl]" >> $@
	echo "source [file join \$$dir rpc.tcl]" >> $@

$(TCLDP_OBJ): $(TCLTK_OBJ)
	@if [ ! -d $@ ]; then echo "Making $@"; mkdir $@; fi
$(TCLDP_DEST): $(TCLTK_DEST)
	@if [ ! -d $@ ]; then echo "Making $@"; mkdir $@; fi

######################################################
# Tcl8dp
# Tcl-DP for tcl8
TCL8DP_MAJOR_VERSION = 4.0
TCL8DP_MINOR_VERSION = 
TCL8DP_VERSION = $(TCL8DP_MAJOR_VERSION)$(TCL8DP_MINOR_VERSION)

TCL8DP_SRC =	$(TCLTK_SRC)/tcl-dp/unix
TCL8DP_OBJ =	$(TCLTK_OBJ)/tcl8-dp$(TCL8DP_VERSION)
TCL8DP_DEST =	$(TCLTK_DEST)/dptcl8.$(PTARCH)
TCL8DP_INDEPENDENT_DEST =	$(TCLTK_DEST)/dp

all_tcl8dp: config_tcl8dp tcl8dp install_tcl8dp
config_tcl8dp: $(TCL8DP_OBJ)
	@echo "Configuring tcl8dp"
	(cd $(TCL8DP_OBJ); PWD=$(TCL8DP_OBJ); export PWD; \
		$(TCL8DP_SRC)/configure \
		--srcdir=$(TCL8DP_SRC) \
		--with-tcl=$(TCL8_OBJ) \
		--prefix=$(TCL8DP_DEST) \
		$(ENABLE_SHARED) $(ENABLE_GCC))

tcl8dp:
	(cd $(TCL8DP_OBJ); PWD=$(TCL8DP_OBJ); export PWD; $(MAKE))

# We are not handling separate prefix and exec_prefix directories here.
install_tcl8dp: $(TCL8DP_DEST) $(TCL8DP_DEST)/pkgIndex.tcl \
		$(TCL8DP_DEST)/init.tcl
	# We should use install-sh or something here.
	cp $(TCL8DP_OBJ)/libdp40.so $<
	cp $(TCL8DP_SRC)/../library/*.tcl $<


$(TCL8DP_DEST)/pkgIndex.tcl: $(TCL8DP_DEST)
	echo "package ifneeded dp 4.0 [list source [file join \$$dir init.tcl]]" > $@"

$(TCL8DP_DEST)/init.tcl: $(TCL8DP_DEST)
	echo "set dir $(TCL8DP_DEST)" > $@
	echo "load [file join \$$dir libdp40[info sharedlibextension]]" >> $@
	echo "source [file join \$$dir acl.tcl]" >> $@
	echo "source [file join \$$dir distribObj.tcl]" >> $@
	echo "source [file join \$$dir dp_atclose.tcl]" >> $@
	echo "source [file join \$$dir dp_atexit.tcl]" >> $@
	echo "source [file join \$$dir ldelete.tcl]" >> $@
	echo "source [file join \$$dir oo.tcl]" >> $@
	echo "source [file join \$$dir rpc.tcl]" >> $@

$(TCL8DP_OBJ): $(TCLTK_OBJ)
	@if [ ! -d $@ ]; then echo "Making $@"; mkdir $@; fi
$(TCL8DP_DEST): $(TCLTK_DEST)
	@if [ ! -d $@ ]; then echo "Making $@"; mkdir $@; fi


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




