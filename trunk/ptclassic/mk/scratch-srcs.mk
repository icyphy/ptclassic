# Version Identification:
# $Id$
# Build GNU binaries and tcl/tk from scratch for Ptolemy
#
# Copyright (c) 1994 The Regents of the University of California.
# All rights reserved.
# 
# Permission is hereby granted, without written agreement and without
# license or royalty fees, to use, copy, modify, and distribute this
# software and its documentation for any purpose, provided that the above
# copyright notice and the following two paragraphs appear in all copies
# of this software.
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

# This script builds GNU binaries and tcl/tk from scratch.  This
# file and the GNU and tcl/tk sources are part of an optional Ptolemy
# tar file.
# You probably don't need to build from scratch, you can use the binaries
# from the binary only distribution
#
# If you are going to build from scratch, you should have the PTOLEMY and
# ARCH variables set, and you should have $PTOLEMY/bin.$ARCH
# in your path. For example:
#	setenv PTOLEMY /users/ptolemy
#	setenv ARCH sun4
#	set path = ( $PTOLEMY/bin.$ARCH $PTOLEMY/bin $path)
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
GNU_DEST=	$(PTOLEMY)/gnu
TCLTK_DEST=	$(PTOLEMY)/tcltk
XV_DEST= 	$(PTOLEMY)
OBJARCH=	$(PTOLEMY)/obj.$(ARCH)
ROOT=		.
XV_CC=		cc

include $(ROOT)/mk/config-$(ARCH).mk

all: gnu_all tcltk_all xv_all
install: gnu_all tcltk_all xv_all
clean: gnu_clean tcltk_clean xv_clean

.PHONY: gnu_all tcltk_all

#
# Build and install gnu tools
#
gnu_all: gnu_configure gnu_bin gnu_install

.PHONY: gnu_configure gnu_bin gnu_install

gnu_configure: stats $(OBJARCH)/gnu
$(OBJARCH)/gnu: $(OBJARCH) 
	(cd $(PTOLEMY)/src/gnu; $(MAKE) $(MFLAGS) GNU_DEST=$(GNU_DEST) configure)

gnu_bin: stats $(OBJARCH)/gnu
	(cd $(PTOLEMY)/src/gnu; $(MAKE) $(MFLAGS) GNU_DEST=$(GNU_DEST) bin)

gnu_install: stats $(OBJARCH)/gnu
	(cd $(PTOLEMY)/src/gnu; $(MAKE) $(MFLAGS) ARCH=$(ARCH) PTOLEMY=$(PTOLEMY) GNU_DEST=$(GNU_DEST) install)

gnu_clean:
	(cd $(PTOLEMY)/src/gnu; $(MAKE) clean)

# For hppa
hpgnu_all: stats  $(OBJARCH)/gnu
	(cd $(PTOLEMY)/src/gnu; $(MAKE) $(MFLAGS) ARCH=$(ARCH) PTOLEMY=$(PTOLEMY) GNU_DEST=$(GNU_DEST) hp_all)

	
#
# Build and install tcl tools
#
tcltk_all: tcltk_configure tcltk_bin tcltk_install

.PHONY: tcltk_configure tcltk_bin tcltk_install

tcltk_configure: $(OBJARCH)/tcltk
$(OBJARCH)/tcltk: stats $(OBJARCH) 
	(cd $(PTOLEMY)/src/tcltk; $(MAKE) $(MFLAGS) TCLTK_DEST=$(TCLTK_DEST) configure)

tcltk_bin: stats $(OBJARCH)/tcltk
	(cd $(PTOLEMY)/src/tcltk; $(MAKE) $(MFLAGS) TCLTK_DEST=$(TCLTK_DEST) bin)

tcltk_install: stats $(OBJARCH)/tcltk
	(cd $(PTOLEMY)/src/tcltk; $(MAKE) $(MFLAGS) TCLTK_DEST=$(TCLTK_DEST) install)
tcltk_clean: 
	(cd $(PTOLEMY)/src/tcltk; $(MAKE) clean)

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
	(cd $(OBJARCH)/xv; xmkmf)

$(OBJARCH)/xv:
	if [ ! -d $(OBJARCH) ]; then mkdir $(OBJARCH); fi
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
		$(MAKE) \
		CC=$(XV_CC) \
		EXTRA_LDOPTIONS=$(CC_STATIC) \
		AR=ar\
		RANLIB=ranlib \
		RAND=$(XV_RAND) \
		BINDIR=$(XV_DEST)/bin.$(ARCH) all)

xv_install: $(OBJARCH)/xv
	(cd $(OBJARCH)/xv; \
		$(MAKE) \
		CC=$(XV_CC) \
		EXTRA_LDOPTIONS=$(CC_STATIC) \
		AR=ar\
		RANLIB=ranlib \
		INSTALL=$(XV_INSTALL) \
		BINDIR=$(XV_DEST)/bin.$(ARCH)  install)
	strip $(PTOLEMY)/bin.$(ARCH)/xv

xv_install.man:
	(cd $(OBJARCH)/xv; \
		$(MAKE) \
		INSTALL=$(XV_INSTALL) \
		MANDIR=$(XV_DEST)/man/man1  install.man)

xv_clean:
	(cd $(OBJARCH)/xv; $(MAKE) clean)



	

$(OBJARCH):
	mkdir $@ 

# Print out information during the build
stats:
	@echo "========================================================"
	@date
	-df $(PTOLEMY)


