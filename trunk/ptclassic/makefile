# Top-level Ptolemy makefile.
# Copyright (c) 1990-1994 The Regents of the University of California.
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
# Programmer:  J. T. Buck
#
# The ARCH environment variable must be set.
# $Id$

defaultTarget:		install
	@echo Installation for $(ARCH) is complete.

makefiles:		obj.$(ARCH)
	@echo Updating makefiles.  Ignore messages about needing to rerun make --
	@echo the top-level makefile will make sure it is done right.
	cd obj.$(ARCH); $(MAKE) $(MFLAGS) makefiles

all install:	sources obj.$(ARCH)
	cd obj.$(ARCH); $(MAKE) $(MFLAGS) $@

clean realclean:
	cd obj.$(ARCH); $(MAKE) $(MFLAGS) $@

sources depend:
	cd src; $(MAKE) $(MFLAGS) makefiles $@

obj.$(ARCH) obj.$(ARCH)/octtools: MAKEARCH
	csh -f MAKEARCH

# This rule builds and installs gcc and tcl/tk from scratch, then octtools,
# then everything else.
# gcc source is distributed as a separate tar file
# tcltk and octtools source are distributed as a separate tar file
bootstrap: obj.$(ARCH) install_gnu install_tcltk install_octtools install

.PHONY:	install_gnu install_tcltk install_octtools install

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
GNU_BUILD_CC=cc
install_gnu:
	$(MAKE) -f mk/scratch-srcs.mk CC=$(GNU_BUILD_CC) $(MFLAGS) gnu_all
install_tcltk:
	$(MAKE) -f mk/scratch-srcs.mk $(MFLAGS) tcltk_all

# Usually rebuilding vem and the octtools library is not done everytime.
# Currently, vem will not run properly if it is built with gcc.
OCT_CC=cc
octtools: obj.$(ARCH)
	cd obj.$(ARCH)/octtools; $(MAKE) CC=$(OCT_CC) $(MFLAGS) all
install_octtools: obj.$(ARCH)
	cd obj.$(ARCH)/octtools; $(MAKE) CC=$(OCT_CC) $(MFLAGS) install

