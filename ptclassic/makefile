# Top-level Ptolemy makefile.
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
# Programmers:  J. T. Buck, Christopher Hylands
#
# The PTARCH environment variable must be set.
# $Id$

defaultTarget:		install
	@echo Installation for $(PTARCH) is complete.

makefiles:		obj.$(PTARCH)
	@echo Updating makefiles.  Ignore messages about needing to rerun make --
	@echo the top-level makefile will make sure it is done right.
	cd obj.$(PTARCH); $(MAKE) $(MFLAGS) makefiles

all install:	obj.$(PTARCH) sources 
	cd obj.$(PTARCH); $(MAKE) $(MFLAGS) $@

clean realclean:
	cd obj.$(PTARCH); $(MAKE) $(MFLAGS) $@

sources depend TAGS: obj.$(PTARCH)
	cd src; $(MAKE) $(MFLAGS) makefiles $@

obj.$(PTARCH) obj.$(PTARCH)/octtools: MAKEARCH
	csh -f MAKEARCH

# This rule builds and installs everything but the gnu tools.
# tcltk, octtools, and gthreads source are distributed
# as a separate tar file.
everything: obj.$(PTARCH) install_tcltk install_octtools install_xv \
	install_gthreads install

everything_clean: clean_tcltk clean_octtools clean_xv clean_gthreads clean

# This rule builds and installs gcc and tcl/tk from scratch, then octtools,
# then everything else.
# gcc source is distributed as a separate tar file
# tcltk and octtools source are distributed as a separate tar file
bootstrap: obj.$(PTARCH) install_gnu install_tcltk install_octtools install_xv \
	install_gthreads install

bootstrap_clean: clean_gnu clean_tcltk clean_octtools clean_xv \
	clean_gthreads clean

.PHONY:	install_gnu install_tcltk install_octtools install_gthreads install

# The gcc INSTALL file says:
#     If you are building with a previous GNU C compiler, do not use
#     `CC=gcc' on the make command or by editing the Makefile.  Instead,
#     use a full pathname to specify the compiler, such as
#     `CC=/usr/local/bin/gcc'.  This is because make might execute the
#     `gcc' in the current directory before all of the compiler
#     components have been built.
# If you already have gcc, you will need to change this variable, or
# building gcc/main.c may fail
#GNU_BUILD_CC=/usr/tools/gnu/bin/gcc
# Use for NetBSD-1.0
# GNU_BUILD_CC=/usr/bin/gcc

GNU_BUILD_CC=cc

# Special CC for irix5.  If you are using gcc to recompile, use this:
#IRIX5_GNU_BUILD_CC=/users/ptolemy/gnu/irix5/bin/gcc
# If you are using cc to compile gcc-2.5.8 under irix5, use this:
#IRIX5_GNU_BUILD_CC=cc -Wf,-XNh1500,-XNg1500 -Olimit 3000 -cckr -O1
# If you are using cc to compile gcc-2.6.2 under irix5, use this:
IRIX5_GNU_BUILD_CC=cc -Wf,-XNh2000,-XNg1500 -Olimit 3000 -cckr -O1

# irix5 /bin/make seems to need this
SHELL=/bin/sh

install_gnu:
	@echo "Building gnu for $(PTARCH)";
	case $(PTARCH) in \
	hppa) 	$(MAKE) -f mk/scratch-srcs.mk CC=$(GNU_BUILD_CC) \
		$(MFLAGS) hpgnu_all;; \
	irix5)	$(MAKE) -f mk/scratch-srcs.mk CC="$(IRIX5_GNU_BUILD_CC)" \
		$(MFLAGS) irix5gnu_all;; \
	*)	$(MAKE) -f mk/scratch-srcs.mk CC=$(GNU_BUILD_CC) \
		$(MFLAGS) gnu_all;; \
	esac

install_tcltk:
	$(MAKE) -f mk/scratch-srcs.mk $(MFLAGS) tcltk_all
clean_tcltk:
	$(MAKE) -f mk/scratch-srcs.mk $(MFLAGS) tcltk_clean

# Not all architectures support shared libraries.  sol2 w/ gcc-2.7.0 does.
install_tcltk_shared:
	$(MAKE) -f mk/scratch-srcs.mk $(MFLAGS) tcltk_all_shared
clean_tcltk_shared:
	$(MAKE) -f mk/scratch-srcs.mk $(MFLAGS) tcltk_clean_shared

# Usually rebuilding vem and the octtools library is not done everytime.
# Note that the octtools source is distributed as a separate tar file.

octtools: obj.$(PTARCH)/octtools
	cd obj.$(PTARCH)/octtools; $(MAKE) $(MFLAGS) all
install_octtools: obj.$(PTARCH)/octtools
	cd obj.$(PTARCH)/octtools; $(MAKE) $(MFLAGS) install
clean_octtools:
	cd obj.$(PTARCH)/octtools; $(MAKE) $(MFLAGS) clean

install_xv:
	$(MAKE) -f mk/scratch-srcs.mk $(MFLAGS) xv_all
clean_xv:
	$(MAKE) -f mk/scratch-srcs.mk $(MFLAGS) xv_clean

install_gthreads:
	cd obj.$(PTARCH)/thread; $(MAKE) $(MFLAGS) gthreads_install

clean_gthreads:
	cd obj.$(PTARCH)/thread; $(MAKE) $(MFLAGS) gthreads_clean
