# Makefile for $PTOLEMY/src/gnu, used to compile the GNU tools
# $Id$
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
# Version:
# @(#)makefile	1.1 3/21/94

# src/gnu/Makefile
# You should not need to edit this file, merely set the
# ARCH and PTOLEMY environment variables, for example:
#	setenv ARCH sun4
#	setenv PTOLEMY /users/ptolemy

# Destination of the GNU binaries, libraries and include files.
GNU_DEST=$(PTOLEMY)/gnu

# Location of the GNU sources.
GNU_SRC=$(PTOLEMY)/src/gnu/src

# Location of the architecture specific build directory.
OBJ_DIR=$(PTOLEMY)/obj.$(ARCH)

# We don't ship makeinfo source with Ptolemy, but the GNU make install
# requires it.
MAKEINFO=$(GNU_SRC)/ptmakeinfo

# No changes should be necessary below this line
#---------------------------------------------------------
# Location GNU architecture specific build directory
GNU_OBJ=$(OBJ_DIR)/gnu

# Machine independent files
GNU_PREFIX=$(GNU_DEST)/common

GCC_VERSION=2.5.8

# Machine dependent files
GNU_EXEC_PREFIX=$(GNU_DEST)/$(ARCH)

# Location of gnu bin directory relative to Ptolemy bin directory
# Can't use absolute path name here, or we will create a distribution that
# will not work if $PTOLEMY != /users/ptolemy
REL_GNU_BINDIR=../gnu/$(ARCH)/bin

# Location of Ptolemy bin dir.  We make links from here to the GNU stuff
BINDIR=$(PTOLEMY)/bin.$(ARCH)

all: configure bin install
.PHONY:	configure bin install

# mach-vars is a simple script that returns the 'right' value for a machine
MACH=`$(GNU_SRC)/gcc/config.guess`

# Minor fixes to the configure procedure.  These changes
# are necessary if you are using SunOS make.  If you already have
# GNU make, then these changes are not necessary, but they won't
# hurt, either
configure: $(GNU_OBJ)
	(cd $(GNU_OBJ); \
	$(GNU_SRC)/configure --srcdir=$(GNU_SRC) \
		--prefix=$(GNU_PREFIX) --exec-prefix=$(GNU_EXEC_PREFIX) )
	@echo "Now reconfiguring gcc directory so we set up gcc/cp right."
	rm -rf $(GNU_OBJ)/gcc
	mkdir $(GNU_OBJ)/gcc
	(cd $(GNU_OBJ)/gcc; \
	$(GNU_SRC)/gcc/configure --srcdir=$(GNU_SRC)/gcc \
		--prefix=$(GNU_PREFIX) --exec-prefix=$(GNU_EXEC_PREFIX) )

	-(cd $(GNU_OBJ)/make; ln -s $(GNU_SRC)/make/*.c .)
	-(cd $(GNU_OBJ)/make/glob; ln -s $(GNU_SRC)/make/glob/*.h .)
	-(cd $(GNU_OBJ)/make/glob; ln -s $(GNU_SRC)/make/glob/*.c .)
	-(cd $(GNU_OBJ)/libio; ln -s $(GNU_SRC)/libio/*.c* .)
	-(cd $(GNU_OBJ)/libiberty; ln -s $(GNU_SRC)/libiberty/*.c* .)
	-(cd $(GNU_OBJ)/libg++/src; ln -s $(GNU_SRC)/libg++/src/*.c* .)
	-(cd $(GNU_OBJ)/libg++/src; ln -s $(GNU_SRC)/libg++/src/*.h .)
	-(cd $(GNU_OBJ)/libg++/gperf/src; ln -s $(GNU_SRC)/libg++/gperf/src/*.c* .)
	-(cd $(GNU_OBJ)/gcc; ln -s $(GNU_SRC)/gcc/*.c .)
	# Don't link the .h files, or cp/call.c won't compile
	#-(cd $(GNU_OBJ)/gcc; ln -s $(GNU_SRC)/gcc/*.h .)

$(GNU_OBJ): $(OBJ_DIR) 
	-mkdir "$(GNU_OBJ)"

# If we are building GNU programs in a bare distribution, then the OBJ.$(ARCH)
# directory might not exist yet
$(OBJ_DIR):
	mkdir $(OBJ_DIR)

# DEC make can't seem to handle VPATH very well
GMAKE= `if [ -f $(GNU_OBJ)/make/make ]; \
	 then echo $(GNU_OBJ)/make/make ; \
	 else echo make; fi`


bin:
	(cd $(GNU_OBJ); $(MAKE) $(MFLAGS) CC="$(CC)" MAKE=$(GMAKE) )
	#	(cd $(GNU_OBJ); $(MAKE) $(MFLAGS) CC="$(CC)" MAKE=$(GMAKE) )

$(BINDIR):
	mkdir $(BINDIR)
install: $(BINDIR) 
	(cd $(GNU_OBJ); $(MAKE) CC="$(CC)" MAKEINFO="$(MAKEINFO)" install)
	-(cd $(BINDIR); ln -s $(REL_GNU_BINDIR)/* .)
	-(cd $(BINDIR); rm -f gperf unprotoize protoize hp* dec* c++ make.old)

# The bin rule above compiles gcc with cc, which may result in a buggy
# gcc.  See src/gnu/src/gcc/INSTALL for more info.
# Use the gcc '-v ' flag to see whether your gcc was compiled with cc or gcc
bootstrap_gcc: configure
	(cd $(GNU_OBJ)/gcc; $(MAKE) MAKE=$(GMAKE) bootstrap)

install_bootstrap_gcc:
	(cd $(GNU_OBJ)/gcc; $(MAKE) install CC="stage2/xgcc -Bstage2/" CFLAGS="-g -O")

# Rules for hppa
# See src/gnu/INSTALL for the details, but basically, we install a special
# GNU assembler so the the -g flag will work.
HP_GAS_SRC=$(PTOLEMY)/src/gnu/hp/gas-2.2.u2
hp_all: hp_gas_configure hp_gas_bin hp_gas_install hp_configure \
		hp_gas_pregcc_install bin install

hp_gas_configure: $(GNU_OBJ) $(GNU_OBJ)/hp
	(cd $(GNU_OBJ)/hp; \
	$(HP_GAS_SRC)/configure $(MACH) --srcdir=$(HP_GAS_SRC) \
		--prefix=$(GNU_PREFIX) --exec-prefix=$(GNU_EXEC_PREFIX) )
	-(cd $(GNU_OBJ)/hp/gas; ln -s $(HP_GAS_SRC)/gas/*.c .)
	-(cd $(GNU_OBJ)/hp/gas; ln -s $(HP_GAS_SRC)/gas/*.h .)
	-(cd $(GNU_OBJ)/hp/libiberty; ln -s $(HP_GAS_SRC)/libiberty/*.c* .)
	-(cd $(GNU_OBJ)/hp/bfd; ln -s $(HP_GAS_SRC)/bfd/*.c .)
	-(cd $(GNU_OBJ)/hp/bfd; ln -s $(HP_GAS_SRC)/bfd/*.h .)
	-(cd $(GNU_OBJ)/hp/opcodes; ln -s $(HP_GAS_SRC)/opcodes/*.c .)
	-(cd $(GNU_OBJ)/hp/opcodes; ln -s $(HP_GAS_SRC)/opcodes/*.h .)
	-(cd $(GNU_OBJ)/hp/texinfo/makeinfo; ln -s $(HP_GAS_SRC)/texinfo/makeinfo/*.c .)
	-(cd $(GNU_OBJ)/hp/texinfo/makeinfo; ln -s $(HP_GAS_SRC)/texinfo/makeinfo/*.h .)
	-(cd $(GNU_OBJ)/hp/texinfo/info; ln -s $(HP_GAS_SRC)/texinfo/info/*.c .)
	-(cd $(GNU_OBJ)/hp/texinfo/info; ln -s $(HP_GAS_SRC)/texinfo/info/*.h .)
	-(cd $(GNU_OBJ)/hp/texinfo/util; ln -s $(HP_GAS_SRC)/texinfo/util/*.c .)
	-(cd $(GNU_OBJ)/hp/texinfo/util; ln -s $(HP_GAS_SRC)/texinfo/util/*.h .)
$(GNU_OBJ)/hp:
	mkdir $(GNU_OBJ)/hp

hp_gas_bin:
	(cd $(GNU_OBJ)/hp; $(MAKE) MAKE=$(GMAKE) )

hp_gas_install: $(BINDIR)
	(cd $(GNU_OBJ)/hp; $(MAKE) MAKE=$(GMAKE) MAKEINFO="$(MAKEINFO)" install)

HP_MACH=`$(GNU_SRC)/gcc/config.guess`
hp_configure: $(GNU_OBJ)
	@echo "Configure everything, then go back and reconfigure gcc"
	@echo "The top level configure does not pass things down properly"
	(cd $(GNU_OBJ); \
	$(GNU_SRC)/configure hppa1.1-hp-hpux --srcdir=$(GNU_SRC) \
		--prefix=$(GNU_PREFIX) --exec-prefix=$(GNU_EXEC_PREFIX) --with-gnu-as)
	@echo "Now reconfigure gcc with -gas"
	(cd $(GNU_OBJ)/gcc; \
	$(GNU_SRC)/gcc/configure --srcdir=$(GNU_SRC)/gcc \
		--prefix=$(GNU_PREFIX) --exec-prefix=$(GNU_EXEC_PREFIX) \
		--with-gnu-as hppa1.1-hp-hpux)
	@echo "Make links for the source files.  Some makes are not smart"
	@echo " enough to handle vpath"
	-(cd $(GNU_OBJ)/make; ln -s $(GNU_SRC)/make/*.c .)
	-(cd $(GNU_OBJ)/make/glob; ln -s $(GNU_SRC)/make/glob/*.h .)
	-(cd $(GNU_OBJ)/make/glob; ln -s $(GNU_SRC)/make/glob/*.c .)
	-(cd $(GNU_OBJ)/libio; ln -s $(GNU_SRC)/libio/*.c* .)
	-(cd $(GNU_OBJ)/libiberty; ln -s $(GNU_SRC)/libiberty/*.c* .)
	-(cd $(GNU_OBJ)/libg++/src; ln -s $(GNU_SRC)/libg++/src/*.c* .)
	-(cd $(GNU_OBJ)/libg++/gperf/src; ln -s $(GNU_SRC)/libg++/gperf/src/*.c* .)
	-(cd $(GNU_OBJ)/gcc; ln -s $(GNU_SRC)/gcc/*.c .)
	# Don't link the .h files, or cp/call.c won't compile
	#-(cd $(GNU_OBJ)/gcc; ln -s $(GNU_SRC)/gcc/*.h .)

hp_gas_pregcc_install:
	(cd $(GNU_OBJ)/gcc; $(MAKE)  install-dir)
	-ln -s $(GNU_EXEC_PREFIX)/bin/as  $(GNU_EXEC_PREFIX)/lib/gcc-lib/$(MACH)/$(GCC_VERSION)/as


# In gcc-2.5.8, collect2 is not automagically installed.  The symptom
# of this is that ptcl says that there is no SDF domain, and that perhaps
# constructors are not being called
install-collect2:	
	(cd $(GNU_OBJ)/gcc; $(MAKE) install-collect2)
irix5_all: all install-collect2

clean:
	(cd $(GNU_OBJ); $(MAKE) clean)

realclean:
	(cd $(GNU_OBJ); $(MAKE) realclean)

distclean:
	(cd $(GNU_OBJ); $(MAKE) distclean)
