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
# src/tcltk/makefile
# Makefile to build and install tcl/tk

# You should not need to edit this file, merely set the
# ARCH and PTOLEMY environment variables, for example:
#	setenv ARCH sun4
#	setenv PTOLEMY /users/ptolemy

# root of source tree
ROOT	      = ../..
# Path (absolute or relative) to the object directory root
OBJDIR=..
# VPATH points to the "real" source directory
VPATH         = $(ROOT)/src/pigiRpc

# Get configuration info
# Need $(C_SHAREDFLAGS) and $(SHARED_LIBRARY_COMMAND) below
CONFIG=$(ROOT)/mk/config-$(PTARCH).mk
include $(CONFIG)

# Destination of the tcl/tk binaries, libraries and include files. 
TCLTK_DEST=$(PTOLEMY)/tcltk

# Location of the TCL sources.
TCLTK_SRC=$(PTOLEMY)/src/tcltk

# Location of the architecture specific build directory.
TCLTK_OBJ=$(PTOLEMY)/obj.$(PTARCH)/tcltk

# Location of the architecture specific build directory for shared libraries
TCLTK_SHARED_OBJ=$(PTOLEMY)/obj.$(PTARCH)/tcltk.shared

# where to install binaries
BINDIR=$(PTOLEMY)/bin.$(PTARCH)

# No changes should be necessary below this line
#---------------------------------------------------------

# Version numbers
TCL_MAJOR_VERSION = 	tcl7.4
TCL_VERSION =		tcl7.4p1
TK_MAJOR_VERSION = 	tk4.0
TK_VERSION =		tk4.0p1
ITCL_MAJOR_VERSION =	itcl-2.0
ITCL_MAJOR_VERSION_NUMBER = 2.0
ITCL_VERSION =		itcl-2.0b2


# Location of the itcl source
ITCL_SRC =	$(TCLTK_SRC)/$(ITCL_VERSION)

# Location of the itcl only object files
ITCL_OBJ =	$(TCLTK_OBJ)/itcl


# Destination of tcl only machine dependent files
TCL_DEST =	$(TCLTK_DEST)/tcl.$(PTARCH)
# Destination of the machine independent tcl files
TCL_INDEPENDENT_DEST =	$(TCLTK_DEST)/tcl

# Destination of tk only machine dependent files
TK_DEST =	$(TCLTK_DEST)/tk.$(PTARCH)
# Destination of the machine independent tk files
TK_INDEPENDENT_DEST =	$(TCLTK_DEST)/tk

# Destination of itcl only machine dependent files
ITCL_DEST =	$(TCLTK_DEST)/itcl.$(PTARCH)
# Destination of the machine independent itcl files
ITCL_INDEPENDENT_DEST =		$(TCLTK_DEST)/itcl

ITCL_DEST_LIB = $(ITCL_DEST)/lib

# Directory to make shared libraries in
ITCL_SHARED_OBJ =	$(TCLTK_OBJ)/itcl.shared
ITCL_SHARED_DEST_LIB = 	$(ITCL_DEST)/lib/shared

all: 		configure itcl
configure:	$(ITCL_OBJ)/config.status $(TCLTK_OBJ)/makefile
bin:		itcl
install:	install_itcl
clean:		clean_itcl
realclean:	clean_itcl

all_shared:	configure_shared itcl_shared
configure_shared: $(ITCL_SHARED_OBJ)/config.status
bin_shared:		itcl_shared
install_shared:		install_itcl_shared
clean_shared:		clean_itcl_shared
realclean_shared:	clean_itcl_shared

.PHONY:	tcl tk itcl configure install_tcl install_tk install_itcl \
		clean_tcl clean_tk clean_itcl realclean 

######################################################################
# Rules for itcl
# itcl is an extension for tcl that provides tcl 'classes'
# The itcl-2.0b distribution contains tcl and tk

# Configure itcl
$(ITCL_OBJ)/config.status: config_itcl
config_itcl: $(TCLTK_OBJ) $(ITCL_OBJ)
	@echo "Configuring itcl"
	(cd $(ITCL_OBJ); PWD=$(ITCL_OBJ); export PWD; $(ITCL_SRC)/configure --srcdir=$(ITCL_SRC) --prefix=$(ITCL_INDEPENDENT_DEST) --exec-prefix=$(ITCL_DEST)) 
$(TCLTK_OBJ): 
	@if [ ! -d $@ ]; then echo "Making $@"; mkdir $@; fi

$(ITCL_OBJ): $(TCLTK_OBJ)
	@if [ ! -d $@ ]; then echo "Making $@"; mkdir $@; fi

itcl: $(ITCL_OBJ)/iwidgets/init.iwidgets.in $(ITCL_OBJ)/libitcl.a 
$(ITCL_OBJ)/libitcl.a: $(ITCL_OBJ) 
	@echo "Making all in itcl"
	(cd  $(ITCL_OBJ); $(MAKE) \
		CFLAGS="$(CFLAGS) $(WRITABLE_STRINGS_CFLAGS)" \
		X11_INCLUDES="$(X11_INCSPEC)" \
		X11_LIB_SWITCHES="$(X11_LIBSPEC)" \
		all)

# Bug fix for itcl-2.0b
$(ITCL_OBJ)/iwidgets/init.iwidgets.in:
	ln -s $(ITCL_SRC)/iwidgets/init.iwidgets.in $@	

# Install itcl
install_itcl: $(ITCL_DEST_LIB)/libitcl.a

$(ITCL_DEST_LIB)/libitcl.a: \
		$(ITCL_OBJ)/libitcl.a $(TCLTK_DEST) $(ITCL_DEST) \
		$(ITCL_DEST_LIB) $(ITCL_INDEPENDENT_DEST) \
		$(ITCL_DEST_LIB)/libtk.a \
		$(ITCL_DEST_LIB)/libitcl.a $(ITCL_DEST_LIB)/libitk.a \
		$(TCL_DEST) $(TCL_INDEPENDENT_DEST) \
		$(TK_DEST) $(TCL_INDEPENDENT_DEST) 
	(cd  $(ITCL_OBJ); $(MAKE) \
		CFLAGS="$(CFLAGS) $(WRITABLE_STRINGS_CFLAGS)" \
		X11_INCLUDES="$(X11_INCSPEC)" \
		X11_LIB_SWITCHES="$(X11_LIBSPEC)" \
		INCLUDE_DIR=$(ITCL_INDEPENDENT_DEST)/include \
		install)
	-(cd $(ITCL_DEST_LIB); \
		ln -s libitcl$(ITCL_MAJOR_VERSION_NUMBER).a libitcl.a)

$(ITCL_DEST):
	mkdir $@
$(ITCL_DEST_LIB):
	mkdir $@
$(ITCL_INDEPENDENT_DEST):
	mkdir $@

# Links so that we don't have to change version numbers in lots of makefiles.
$(ITCL_DEST_LIB)/libtcl.a:
	(cd $(ITCL_DEST_LIB); ln -s lib$(TCL_MAJOR_VERSION).a libtcl.a)
$(ITCL_DEST_LIB)/libtk.a:
	(cd $(ITCL_DEST_LIB); ln -s lib$(TK_MAJOR_VERSION).a libtk.a)
$(ITCL_DEST_LIB)/libitk.a:
	(cd $(ITCL_DEST_LIB); \
		ln -s libitk$(ITCL_MAJOR_VERSION_NUMBER).a libitk.a)

# Links so that older programs can find libs and includes.
$(TCL_DEST):
	(cd $(TCLTK_DEST); ln -s itcl.$(PTARCH) tcl.$(PTARCH))
$(TCL_INDEPENDENT_DEST):
	(cd $(TCLTK_DEST); ln -s itcl tcl)
$(TK_DEST):
	(cd $(TCLTK_DEST); ln -s itcl.$(PTARCH) tk.$(PTARCH))
$(TK_INDEPENDENT_DEST):
	(cd $(TCLTK_DEST); ln -s itcl kt)

clean_itcl:
	(cd $(ITCL_OBJ); $(MAKE) clean)

#######################
$(TCLTK_OBJ)/makefile:
	ln -s $(TCLTK_SRC)/makefile $@

#######################
# Produce shared libraries
$(ITCL_SHARED_OBJ)/config.status: config_itcl_shared
config_itcl_shared: $(TCLTK_OBJ) $(ITCL_SHARED_OBJ)
	@echo "Configuring itcl"
	(cd $(ITCL_SHARED_OBJ); PWD=$(ITCL_SHARED_OBJ); export PWD; $(ITCL_SRC)/configure --srcdir=$(ITCL_SRC) --prefix=$(ITCL_INDEPENDENT_DEST) --exec-prefix=$(ITCL_DEST)) 

$(ITCL_SHARED_OBJ): $(TCLTK_OBJ)
	@if [ ! -d $@ ]; then echo "Making $@"; mkdir $@; fi

itcl_shared: $(ITCL_SHARED_OBJ) \
		$(ITCL_SHARED_OBJ)/iwidgets/init.iwidgets.in \
		$(ITCL_SHARED_OBJ)/iwidgets/init.iwidgets \
		$(ITCL_SHARED_OBJ)/tcl/libtcl.so \
		$(ITCL_SHARED_OBJ)/tk/libtk.so \
		$(ITCL_SHARED_OBJ)/itcl/libitcl.so \
		$(ITCL_SHARED_OBJ)/itk/libitk.so
	@echo "Done making all shared libraries in itcl"

# Bug fix for itcl-2.0b
$(ITCL_SHARED_OBJ)/iwidgets/init.iwidgets.in:
	ln -s $(ITCL_SRC)/iwidgets/init.iwidgets.in $@	

$(ITCL_SHARED_OBJ)/iwidgets/init.iwidgets:
	(cd  $(ITCL_SHARED_OBJ)/iwidgets; $(MAKE) all)

$(ITCL_SHARED_OBJ)/tcl/libtcl.so: 
	(cd  $(ITCL_SHARED_OBJ)/tcl; $(MAKE) \
	       CFLAGS="$(CFLAGS) $(WRITABLE_STRINGS_CFLAGS) $(C_SHAREDFLAGS)" \
		X11_INCLUDES="$(X11_INCSPEC)" \
		X11_LIB_SWITCHES="$(X11_LIBSPEC)" \
		all)
	(cd  $(ITCL_SHARED_OBJ)/tcl; $(SHARED_LIBRARY_COMMAND) libtcl.so *.o)

$(ITCL_SHARED_OBJ)/tk/libtk.so: 
	(cd  $(ITCL_SHARED_OBJ)/tk; $(MAKE) \
	       CFLAGS="$(CFLAGS) $(WRITABLE_STRINGS_CFLAGS) $(C_SHAREDFLAGS)" \
		X11_INCLUDES="$(X11_INCSPEC)" \
		X11_LIB_SWITCHES="$(X11_LIBSPEC)" \
		all)
	(cd  $(ITCL_SHARED_OBJ)/tk; $(SHARED_LIBRARY_COMMAND) libtk.so *.o)

$(ITCL_SHARED_OBJ)/itcl/libitcl.so: 
	(cd  $(ITCL_SHARED_OBJ)/itcl; $(MAKE) \
	       CFLAGS="$(CFLAGS) $(WRITABLE_STRINGS_CFLAGS) $(C_SHAREDFLAGS)" \
		X11_INCLUDES="$(X11_INCSPEC)" \
		X11_LIB_SWITCHES="$(X11_LIBSPEC)" \
		shared)
$(ITCL_SHARED_OBJ)/itk/libitk.so: 
	(cd  $(ITCL_SHARED_OBJ)/itk; $(MAKE) \
	       CFLAGS="$(CFLAGS) $(WRITABLE_STRINGS_CFLAGS) $(C_SHAREDFLAGS)" \
		X11_INCLUDES="$(X11_INCSPEC)" \
		X11_LIB_SWITCHES="$(X11_LIBSPEC)" \
		shared)

# Install itcl
install_itcl_shared: $(ITCL_SHARED_DEST_LIB)/libitcl.so

$(ITCL_SHARED_DEST_LIB)/libitcl.so: \
		$(ITCL_DEST) $(ITCL_DEST_LIB) \
		$(TCLTK_DEST) \
		$(ITCL_SHARED_DEST_LIB) $(ITCL_INDEPENDENT_DEST) \
		$(ITCL_SHARED_OBJ)/itcl/libitcl.so \
		$(ITCL_SHARED_DEST_LIB)/libtcl.so \
		$(ITCL_SHARED_DEST_LIB)/libtk.so \
		$(ITCL_SHARED_DEST_LIB)/libitk.so \
		$(TCL_DEST) $(TCL_INDEPENDENT_DEST) \
		$(TK_DEST) $(TCL_INDEPENDENT_DEST) 
	(cd  $(ITCL_SHARED_OBJ); $(MAKE) \
		CFLAGS="$(CFLAGS) $(WRITABLE_STRINGS_CFLAGS)" \
		X11_INCLUDES="$(X11_INCSPEC)" \
		X11_LIB_SWITCHES="$(X11_LIBSPEC)" \
		INCLUDE_DIR=$(ITCL_INDEPENDENT_DEST)/include \
		install)
	mv $(ITCL_DEST_LIB)/libitcl.so.2 $(ITCL_SHARED_DEST_LIB)/libitcl.so


$(ITCL_SHARED_DEST_LIB):
	mkdir $@

# Links so that we don't have to change version numbers in lots of makefiles.
$(ITCL_SHARED_DEST_LIB)/libtcl.so: $(ITCL_SHARED_OBJ)/tcl/libtcl.so
	mv $(TCLTK_OBJ)/itcl.shared/tcl/libtcl.so $@
$(ITCL_SHARED_DEST_LIB)/libtk.so: $(ITCL_SHARED_OBJ)/tk/libtk.so 
	mv $(TCLTK_OBJ)/itcl.shared/tk/libtk.so $@
$(ITCL_SHARED_DEST_LIB)/libitk.so: $(ITCL_SHARED_OBJ)/itk/libitk.so 
	mv $(ITCL_DEST_LIB)/libitk.so.2 $(ITCL_SHARED_DEST_LIB)/libitk.so


clean_shared_itcl:
	(cd $(ITCL_SHARED_OBJ); $(MAKE) clean)
