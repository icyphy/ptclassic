# Version Identification:
# $Id$
# Copyright (c) 1990-1995 The Regents of the University of California.
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

# Version numbers
#TCL_VERSION =		tcl7.3
#TK_MAJOR_VERSION = 	tk3.6
#TK_VERSION =		tk3.6p1

TCL_MAJOR_VERSION = 	tcl7.4
TCL_VERSION =		tcl7.4p1
TK_MAJOR_VERSION = 	tk4.0
TK_VERSION =		tk4.0p1

ITCL_VERSION =	itcl-1.5


# Location of the tcl source
TCL_SRC =	$(TCLTK_SRC)/$(TCL_VERSION)
# Location of the tk source
TK_SRC =	$(TCLTK_SRC)/$(TK_VERSION)
# Location of the itcl source
ITCL_SRC =	$(TCLTK_SRC)/$(ITCL_VERSION)


# Location of the tcl only object files
TCL_OBJ =	$(TCLTK_OBJ)/tcl
#TCL_OBJ =	$(TCLTK_OBJ)/$(TCL_VERSION)
# Location of the tk only object files
TK_OBJ =	$(TCLTK_OBJ)/tk
#TK_OBJ =	$(TCLTK_OBJ)/$(TK_VERSION)
# Location of the tk only object files
ITCL_OBJ =	$(TCLTK_OBJ)/itcl


# Destination of tcl only machine dependent files
#TCL_DEST =	$(TCLTK_DEST)/tcl.$(PTARCH)
TCL_DEST =	$(TCLTK_DEST)/$(TCL_VERSION).$(PTARCH)
# Destination of the machine independent tcl files
#TCL_INDEPENDENT_DEST =	$(TCLTK_DEST)/tcl
TCL_INDEPENDENT_DEST =	$(TCLTK_DEST)/$(TCL_VERSION)

# Destination of tk only machine dependent files
#TK_DEST =	$(TCLTK_DEST)/tk.$(PTARCH)
TK_DEST =	$(TCLTK_DEST)/$(TK_VERSION).$(PTARCH)
# Destination of the machine independent tk files
#TK_INDEPENDENT_DEST =	$(TCLTK_DEST)/tk
TK_INDEPENDENT_DEST =	$(TCLTK_DEST)/$(TK_VERSION)

# Destination of itcl only machine dependent files
ITCL_DEST =	$(TCLTK_DEST)/itcl.$(PTARCH)
# Destination of the machine independent itcl files
ITCL_INDEPENDENT_DEST =	$(TCLTK_DEST)/itcl

all: 		configure tcl tk itcl
configure:	$(TCL_OBJ)/config.status $(TK_OBJ)/config.status \
			$(ITCL_OBJ)/config.status \
			$(TCLTK_OBJ)/$(TCL_MAJOR_VERSION) $(TCLTK_OBJ)/makefile
bin:		tcl tk itcl
install:	install_tcl install_tk install_itcl
clean:		clean_tcl clean_tk clean_itcl
realclean:	clean_tcl clean_tk clean_itcl

.PHONY:	tcl tk itcl configure install_tcl install_tk install_itcl \
		clean_tcl clean_tk clean_itcl realclean 


###########################################################################
# Rules for tcl
#

# Configure tcl
$(TCL_OBJ)/config.status: $(TCLTK_OBJ) $(TCL_OBJ)
	@echo "Configuring tcl"
	(cd $(TCL_OBJ); $(TCL_SRC)/configure -v --srcdir=$(TCL_SRC) --prefix=$(TCL_INDEPENDENT_DEST) --exec-prefix=$(TCL_DEST) )
	-(cd $(TCL_OBJ); ln -s $(TCL_SRC)/install-sh .)

$(TCL_OBJ): 
	mkdir $(TCL_OBJ)

$(TCLTK_OBJ):
	mkdir $(TCLTK_OBJ)

$(BINDIR)/tclsh:
	ln -s $(TCL_DEST)/bin/tclsh $@ 

# Build tcl
tcl: $(TCL_OBJ)/libtcl.a
$(TCL_OBJ)/libtcl.a: $(TCL_OBJ)
	@echo "Making all in tcl"
	(cd  $(TCL_OBJ); $(MAKE) all)

# Install tcl
install_tcl: $(TCL_DEST)/lib/libtcl.a $(BINDIR)/tclsh
$(TCL_DEST)/lib/libtcl.a: $(TCL_OBJ)/libtcl.a $(TCLTK_DEST) $(TCL_DEST) $(TCL_DEST)/lib $(TCL_INDEPENDENT_DEST)
	(cd  $(TCL_OBJ); $(MAKE) install)
	if [ ! -f $(TCL_DEST)/lib/libtcl.a ]; then \
		cd $(TCL_DEST)/lib; ln -s lib$(TCL_VERSION).a libtcl.a; \
	fi

$(TCLTK_DEST):
	mkdir $(TCLTK_DEST)

$(TCL_INDEPENDENT_DEST):
	mkdir $(TCL_INDEPENDENT_DEST)

$(TCL_DEST):
	mkdir $(TCL_DEST)

$(TCL_DEST)/lib:
	mkdir $(TCL_DEST)/lib

clean_tcl:
	(cd $(TCL_OBJ); $(MAKE) clean)

######################################################################
# Rules for tk
#

# Configure tk
#	Need to make a link for tk.h so that we are sure it is installed
#	Theoretically, GNU make should do the right thing, but . . .
$(TK_OBJ)/config.status: $(TCLTK_OBJ) $(TK_OBJ) \
			$(TCLTK_OBJ)/$(TCL_MAJOR_VERSION) \
			$(TCLTK_OBJ)/$(TK_MAJOR_VERSION) #$(TK_OBJ)/xpm
	@echo "Configuring tk"
	(cd $(TK_OBJ); $(TK_SRC)/configure -v --srcdir=$(TK_SRC) --prefix=$(TK_INDEPENDENT_DEST) --exec-prefix=$(TK_DEST) )
	-(cd $(TK_OBJ); ln -s $(TK_SRC)/tk.h tk.h)
	-(cd $(TK_OBJ); ln -s $(TK_SRC)/install-sh .)
$(TK_OBJ):
	mkdir $(TK_OBJ)

# Need the xpm directory for TkPixmap
# TkPixmap is not needed for Ptolemy, but it might be needed for xpole
$(TK_OBJ)/xpm:
	mkdir $(TK_OBJ)/xpm
	(cd $(TK_OBJ)/xpm; ln -s $(TK_SRC)/xpm/* .)

# This link is necessary for the tk build to find libtcl.a
$(TCLTK_OBJ)/$(TCL_MAJOR_VERSION): $(TCL_OBJ)
	(cd $(TCLTK_OBJ); ln -s $(TCL_OBJ) $(TCLTK_OBJ)/$(TCL_MAJOR_VERSION))

# This link is necessary for the tk build to find libtk.a
$(TCLTK_OBJ)/$(TK_MAJOR_VERSION): $(TK_OBJ)
	(cd $(TCLTK_OBJ); ln -s $(TK_OBJ) $(TCLTK_OBJ)/$(TK_MAJOR_VERSION))

# Build tk
tk: $(TK_OBJ)/libtk.a 
$(TK_OBJ)/libtk.a: $(TK_OBJ) #$(TK_OBJ)/xpm/libxpm.a
	@echo "Making all in tk"
	(cd  $(TK_OBJ); $(MAKE) \
		X11_INCLUDES="$(X11_INCSPEC)" \
		X11_LIB_SWITCHES="$(X11_LIBSPEC)" \
		all)

# Build libxpm for TkPixMap
# Pick up the X11 includes on machines like Solaris2
# If your system does not have strdup(), use the following (Ultrix4.3a)
XPM_DEFINES=-DZPIPE -DNEED_STRDUP $(X11_INCSPEC)
# Otherwise:
#XPM_DEFINES=-DZPIPE $(X11_INCSPEC)

# If necessary override values for each independent architecture
# This line needs to be after the XPM_DEFINES line
ROOT =	../..
include $(ROOT)/mk/config-$(PTARCH).mk

all_xpm: $(TK_OBJ)/xpm/libxpm.a
$(TK_OBJ)/xpm/libxpm.a: $(TK_OBJ)/xpm
	(cd $(TK_OBJ)/xpm; $(MAKE) -f Makefile.noXtree \
		DESTBINDIR=$(TK_DEST)/bin \
		DESTLIBDIR=$(TK_DEST)/lib \
		DESTINCLUDEDIR=$(TK_INDEPENDENT_DEST)/include \
		CC=$(CC) \
		RANLIB=$(RANLIB) \
		DEFINES="$(XPM_DEFINES)" \
	all)

# Install tk
install_tk: $(TK_DEST)/lib/libtk.a #$(TK_DEST)/lib/libxpm.a
$(TK_DEST)/lib/libtk.a: $(TK_OBJ)/libtk.a $(TCLTK_DEST) $(TK_DEST) $(TK_DEST)/lib $(TK_INDEPENDENT_DEST)
	(cd  $(TK_OBJ); $(MAKE) \
		X11_INCLUDES="$(X11_INCSPEC)" \
		X11_LIB_SWITCHES="$(X11_LIBSPEC)" \
		install)
	if [ ! -f $(TK_DEST)/lib/libtk.a ]; then \
		cd $(TK_DEST)/lib; ln -s lib$(TK_VERSION).a libtk.a; \
	fi
install_xpm:$(TK_DEST)/lib/libxpm.a
$(TK_DEST)/lib/libxpm.a: $(TK_OBJ)/xpm/libxpm.a
	(cd $(TK_OBJ)/xpm; $(MAKE) -f Makefile.noXtree \
		DESTBINDIR=$(TK_DEST)/bin \
		DESTLIBDIR=$(TK_DEST)/lib \
		DESTINCLUDEDIR=$(TK_INDEPENDENT_DEST)/include \
		RANLIB=$(RANLIB) \
		DEFINES="$(XPM_DEFINES)" \
	install)

$(TK_DEST):
	mkdir $(TK_DEST)
$(TK_DEST)/lib:
	mkdir $(TK_DEST)/lib
$(TK_INDEPENDENT_DEST):
	mkdir $(TK_INDEPENDENT_DEST)

clean_tk:
	(cd $(TK_OBJ); $(MAKE) clean)

######################################################################
# Rules for itcl
# itcl is an extension for tcl that provides tcl 'classes'
#

# Configure itcl
$(ITCL_OBJ)/config.status: config_itcl
config_itcl: $(TCLTK_OBJ) $(ITCL_OBJ) \
		$(TCL_OBJ)/libtcl.a $(TK_OBJ)/libtk.a \
		$(TCL_OBJ)/tclInt.h $(TCL_OBJ)/tcl.h \
		$(TCL_OBJ)/tclRegexp.h $(TK_OBJ)/tk.h
	@echo "Configuring itcl"
	(cd $(ITCL_OBJ); PWD=$(ITCL_OBJ); export PWD; $(ITCL_SRC)/configure --srcdir=$(ITCL_SRC) --prefix=$(ITCL_INDEPENDENT_DEST) --exec-prefix=$(ITCL_DEST)) 
	@echo "Create link for itcl.h installation"
	-(cd $(ITCL_OBJ)/src; ln -s $(ITCL_SRC)/src/itcl.h itcl.h)
	-(cd $(ITCL_OBJ)/library; ln -s $(ITCL_SRC)/library/*.tcl .)

$(ITCL_OBJ): $(TCLTK_OBJ)
	if [ ! -d $@ ]; then \
		echo "Making $@"; \
		mkdir $@; \
	fi

$(TCL_OBJ)/tclInt.h:  $(TCL_OBJ)
	if [ ! -f $@ ]; then \
		(cd $<; ln -s $(TCL_SRC)/tclInt.h .); \
	fi

$(TCL_OBJ)/tclRegexp.h:  $(TCL_OBJ)
	if [ ! -f $@ ]; then \
		(cd $<; ln -s $(TCL_SRC)/tclRegexp.h .); \
	fi

$(TCL_OBJ)/tcl.h:  $(TCL_OBJ)
	if [ ! -f $@ ]; then \
		(cd $<; ln -s $(TCL_SRC)/tcl.h .); \
	fi

$(TK_OBJ)/tk.h:  $(TK_OBJ)
	if [ ! -f $@ ]; then \
		(cd $<; ln -s $(TK_SRC)/tk.h .); \
	fi

# This link is necessary for the itcl build to find libtcl.a
#$(TCLTK_OBJ)/$(TCL_VERSION): $(TCL_OBJ)
#	(cd $(TCLTK_OBJ); ln -s $(TCL_OBJ) $(TCLTK_OBJ)/$(TCL_VERSION))

# This link is necessary for the itcl build to find libtk.a
#$(TCLTK_OBJ)/$(TK_MAJOR_VERSION): $(TK_OBJ)
#	(cd $(TCLTK_OBJ); ln -s $(ITCL_OBJ) $(TCLTK_OBJ)/$(TK_MAJOR_VERSION))

# Build itcl
itcl: $(ITCL_OBJ)/libitcl.a 
$(ITCL_OBJ)/libitcl.a: $(ITCL_OBJ) 
	@echo "Making all in itcl"
	(cd  $(ITCL_OBJ); $(MAKE) \
		X11_INCLUDES="$(X11_INCSPEC)" \
		X11_LIB_SWITCHES="$(X11_LIBSPEC)" \
		all)

# Install itcl
install_itcl: $(ITCL_DEST)/lib/libitcl.a
$(ITCL_DEST)/lib/libitcl.a: $(ITCL_OBJ)/libitcl.a $(TCLTK_DEST) \
		$(ITCL_DEST) $(ITCL_DEST)/lib $(ITCL_INDEPENDENT_DEST)
	(cd  $(ITCL_OBJ); $(MAKE) \
		X11_INCLUDES="$(X11_INCSPEC)" \
		X11_LIB_SWITCHES="$(X11_LIBSPEC)" \
		INCLUDE_DIR=$(ITCL_INDEPENDENT_DEST)/include \
		install)

$(ITCL_DEST):
	mkdir $(ITCL_DEST)
$(ITCL_DEST)/lib:
	mkdir $(ITCL_DEST)/lib
$(ITCL_INDEPENDENT_DEST):
	mkdir $(ITCL_INDEPENDENT_DEST)

clean_itcl:
	(cd $(ITCL_OBJ); $(MAKE) clean)

##################################################################
$(TCLTK_OBJ)/makefile:
	ln -s $(TCLTK_SRC)/makefile $@
