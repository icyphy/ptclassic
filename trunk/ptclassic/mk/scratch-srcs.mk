# Version Identification:
# $Id$
# Build GNU binaries and tcl/tk from scratch for Ptolemy
#
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
# 

# This script builds GNU binaries and tcl/tk from scratch.  This
# file and the GNU and tcl/tk sources are part of an optional Ptolemy
# tar file.
# You probably don't need to build from scratch, you can use the binaries
# from the binary only distribution
#
# If you are going to build from scratch, you should have the PTOLEMY and
# PTARCH variables set, and you should have $PTOLEMY/bin.$PTARCH
# in your path. For example:
#	setenv PTOLEMY /users/ptolemy
#	setenv PTARCH sun4
#	set path = ( $PTOLEMY/bin.$PTARCH $PTOLEMY/bin $path)
#
# Then type:
#	make -f $PTOLEMY/mk/scratch-srcs.mk

# If your system does not have strdup(), use the following (Ultrix4.3a)
# to build the Pixmap extension to tk.
#XPM_DEFINES="-DZPIPE -DNEED_STRDUP"
# Otherwise:
XPM_DEFINES="-DZPIPE"

#PTOLEMY=	/users/ptdesign
#ARCH=		sun4
#########################################
# Should not have to change below here
SHELL=		/bin/sh
GNU_DEST=	$(PTOLEMY)/gnu
TCLTK_DEST=	$(PTOLEMY)/tcltk
XV_DEST= 	$(PTOLEMY)
OBJARCH=	$(PTOLEMY)/obj.$(PTARCH)
ROOT=		.
XV_CC=		cc
XMKMF=		/usr/bin/X11/xmkmf
MKDIRHIER=	/usr/bin/X11/mkdirhier
include $(ROOT)/mk/config-$(PTARCH).mk

all: gnu_all tcltk_all xv_all
install: gnu_all tcltk_all xv_all
clean: gnu_clean tcltk_clean xv_clean

.PHONY: gnu_all tcltk_all

#
# Build and install gnu tools
#
gnu_all: gnu_configure gnu_bin gnu_install

.PHONY: gnu_configure gnu_bin gnu_install

gnu_configure: $(OBJARCH)/gnu
$(OBJARCH)/gnu: $(OBJARCH) 
	(cd $(PTOLEMY)/src/gnu; $(MAKE) $(MFLAGS) GNU_DEST=$(GNU_DEST) configure)

gnu_bin: $(OBJARCH)/gnu
	(cd $(PTOLEMY)/src/gnu; $(MAKE) $(MFLAGS) GNU_DEST=$(GNU_DEST) bin)
	@echo "Now we rebuild gcc, bootstrapping gcc with itself"
	(cd $(PTOLEMY)/obj.$(PTARCH)/gnu/gcc; $(MAKE) $(MFLAGS) GNU_DEST=$(GNU_DEST) bootstrap)

gnu_install: $(OBJARCH)/gnu
	(cd $(PTOLEMY)/src/gnu; $(MAKE) $(MFLAGS) PTARCH=$(PTARCH) PTOLEMY=$(PTOLEMY) GNU_DEST=$(GNU_DEST) install)

gnu_clean:
	(cd $(PTOLEMY)/src/gnu; $(MAKE) clean)

# For hppa
# No need to depend on  $(OBJARCH)/gnu, as hp_all will run configure
# for us
hpgnu_all: 
	(cd $(PTOLEMY)/src/gnu; $(MAKE) $(MFLAGS) PTARCH=$(PTARCH) PTOLEMY=$(PTOLEMY) GNU_DEST=$(GNU_DEST) hp_all)
	@echo "Now we rebuild gcc, bootstrapping gcc with itself"
	(cd $(PTOLEMY)/obj.$(PTARCH)/gnu/gcc; $(MAKE) $(MFLAGS) GNU_DEST=$(GNU_DEST) bootstrap)

# For irix5
# No need to depend on  $(OBJARCH)/gnu, as irix5_all will run configure
# for us
irix5gnu_all:
	(cd $(PTOLEMY)/src/gnu; $(MAKE) $(MFLAGS) PTARCH=$(PTARCH) PTOLEMY=$(PTOLEMY) GNU_DEST=$(GNU_DEST) irix5_all)
	@echo "Now we rebuild gcc, bootstrapping gcc with itself"
	(cd $(PTOLEMY)/obj.$(PTARCH)/gnu/gcc; $(MAKE) $(MFLAGS) GNU_DEST=$(GNU_DEST) bootstrap)


#
# Build and install tcl tools
#
tcltk_all: tcltk_configure tcltk_bin tcltk_install

.PHONY: tcltk_configure tcltk_bin tcltk_install

tcltk_configure: $(OBJARCH)/tcltk
$(OBJARCH)/tcltk: $(OBJARCH) 
	(cd $(PTOLEMY)/src/tcltk; $(MAKE) $(MFLAGS) \
		CC=$(CC) \
		CFLAGS="$(CFLAGS) $(WRITABLE_STRINGS_CFLAGS)" \
		RANLIB=$(RANLIB) \
		TCLTK_DEST=$(TCLTK_DEST) \
		configure)

tcltk_bin: $(OBJARCH)/tcltk
	(cd $(PTOLEMY)/src/tcltk; $(MAKE) $(MFLAGS) \
		CC=$(CC) \
		CFLAGS="$(CFLAGS) $(WRITABLE_STRINGS_CFLAGS)" \
		RANLIB=$(RANLIB) \
		TCLTK_DEST=$(TCLTK_DEST) \
		bin)

tcltk_install: $(OBJARCH)/tcltk
	(cd $(PTOLEMY)/src/tcltk; $(MAKE) $(MFLAGS) \
		CC=$(CC) \
		CFLAGS="$(CFLAGS) $(WRITABLE_STRINGS_CFLAGS)" \
		RANLIB=$(RANLIB) \
		TCLTK_DEST=$(TCLTK_DEST) \
		install)
tcltk_clean: 
	(cd $(PTOLEMY)/src/tcltk; $(MAKE) clean)

#
# Build and install tcl tools with shared libraries
#
tcltk_all_shared: tcltk_configure_shared tcltk_bin_shared tcltk_install_shared

#.PHONY: tcltk_configure_shared tcltk_bin_shared tcltk_install_shared

tcltk_configure_shared: $(OBJARCH)/tcltk.shared
$(OBJARCH)/tcltk.shared: $(OBJARCH) 
	(cd $(PTOLEMY)/src/tcltk; $(MAKE) $(MFLAGS) \
		CC=$(CC) \
		CFLAGS="$(CFLAGS) $(C_SHARED_FLAGS)" \
		RANLIB=$(RANLIB) \
		TCLTK_DEST=$(TCLTK_DEST) \
		configure_shared)

tcltk_bin_shared: $(OBJARCH)/tcltk.shared
	(cd $(PTOLEMY)/src/tcltk; $(MAKE) $(MFLAGS) \
		CC=$(CC) \
		CFLAGS="$(CFLAGS)" \
		RANLIB=$(RANLIB) \
		TCLTK_DEST=$(TCLTK_DEST) \
		bin_shared)

tcltk_install_shared: $(OBJARCH)/tcltk.shared
	(cd $(PTOLEMY)/src/tcltk; $(MAKE) $(MFLAGS) \
		CC=$(CC) \
		CFLAGS="$(CFLAGS)" \
		RANLIB=$(RANLIB) \
		TCLTK_DEST=$(TCLTK_DEST) \
		install_shared)
tcltk_clean_shared: 
	(cd $(PTOLEMY)/src/tcltk; $(MAKE) clean_shared)

#
# Build and install xv
# 	These rules might not work on architectures other than sun4 w/ cc
#	
xv_all: xv_configure xv_bin xv_install xv_install.man

.PHONY: xv_configure xv_bin xv_install xv_install.man

xv_configure: $(OBJARCH)/xv \
		$(OBJARCH)/xv/jpeg $(OBJARCH)/xv/jpeg/Makefile \
		$(OBJARCH)/xv/tiff $(OBJARCH)/xv/tiff/Makefile	\
		$(OBJARCH)/xv/Imakefile
	# xmkmf causes problems under solaris2 with gcc, so we don't use it
	(cd $(OBJARCH)/xv; $(XMKMF) )

$(OBJARCH)/xv: $(OBJARCH)
	mkdir $(OBJARCH)/xv

$(OBJARCH)/xv/Imakefile:
	-(cd $(OBJARCH)/xv; ln -s ../../src/xv/* .)	

$(OBJARCH)/xv/jpeg:
	mkdir $(OBJARCH)/xv/jpeg
$(OBJARCH)/xv/jpeg/Makefile:
	(cd $(OBJARCH)/xv/jpeg; ln -s ../../../src/xv/jpeg/* .)
$(OBJARCH)/xv/tiff:
	mkdir $(OBJARCH)/xv/tiff
$(OBJARCH)/xv/tiff/Makefile:
	(cd $(OBJARCH)/xv/tiff; ln -s ../../../src/xv/tiff/* .)

xv_bin: $(OBJARCH)/xv
	(cd $(OBJARCH)/xv; \
		$(MAKE) -f Makefile.std \
		CC="$(XV_CC)" \
		EXTRA_LDOPTIONS=$(CC_STATIC) \
		AR=ar \
		RANLIB=$(RANLIB) \
		RAND=$(XV_RAND) \
		MKDIRHIER=$(MKDIRHIER) \
		BINDIR=$(XV_DEST)/bin.$(PTARCH) all)

xv_install: $(OBJARCH)/xv $(XV_DEST)/bin.$(PTARCH)
	(cd $(OBJARCH)/xv; cp xv $(XV_DEST)/bin.$(PTARCH))
	chmod a+rx $(PTOLEMY)/bin.$(PTARCH)/xv
	strip $(PTOLEMY)/bin.$(PTARCH)/xv

$(XV_DEST)/bin.$(PTARCH):
	if [ ! -d $(XV_DEST)/bin.$(PTARCH) ]; then \
		mkdir $(XV_DEST)/bin.$(PTARCH); \
	fi

xv_install.man: $(XV_DEST)/man/man1
	rm -f $(XV_DEST)/man/man1/xv.1
	cp $(PTOLEMY)/src/xv/docs/xv.man $(XV_DEST)/man/man1/xv.1

$(XV_DEST)/man/man1:
	if [ ! -d $(XV_DEST)/man ]; then \
		mkdir $(XV_DEST)/man; \
	fi
	if [ ! -d $(XV_DEST)/man/man1 ]; then \
		mkdir $(XV_DEST)/man/man1; \
	else true; fi

xv_clean:
	(cd $(OBJARCH)/xv; $(MAKE) clean)

$(OBJARCH):
	mkdir $@ 
