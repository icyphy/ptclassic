# Makefile for $PTOLEMY/src/gnu, used to compile the GNU tools
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
#

# Destination of the GNU binaries, libraries and include files.
GNU_DEST=$(PTOLEMY)/gnu

# Location of the GNU sources.
GNU_SRC=$(PTOLEMY)/src/gnu/src

# Location of the architecture specific build directory.
OBJ_DIR=$(PTOLEMY)/obj.$(PTARCH)

# We don't ship makeinfo source with Ptolemy, but the GNU make install
# requires it.
MAKEINFO=$(GNU_SRC)/ptmakeinfo

# No changes should be necessary below this line
#---------------------------------------------------------
# Location GNU architecture specific build directory
GNU_OBJ=$(OBJ_DIR)/gnu

# Machine independent files
GNU_PREFIX=$(GNU_DEST)/common

GCC_VERSION=2.6.3

# Machine dependent files
GNU_EXEC_PREFIX=$(GNU_DEST)/$(PTARCH)

# Location of gnu bin directory relative to Ptolemy bin directory
# Can't use absolute path name here, or we will create a distribution that
# will not work if $PTOLEMY != /users/ptolemy
REL_GNU_BINDIR=../gnu/$(PTARCH)/bin

# Location of Ptolemy bin dir.  We make links from here to the GNU stuff
BINDIR=$(PTOLEMY)/bin.$(PTARCH)

all: configure bin install
.PHONY:	configure bin install

MACH=`$(GNU_SRC)/gcc/config.guess`

# Minor fixes to the configure procedure.  These changes
# are necessary if you are using SunOS make.  If you already have
# GNU make, then these changes are not necessary, but they won't
# hurt, either
configure: $(GNU_OBJ)
	(cd $(GNU_OBJ); \
	$(GNU_SRC)/configure -v --srcdir=$(GNU_SRC) \
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
	-(cd $(GNU_OBJ)/gcc; ln -s $(GNU_SRC)/gcc/*.c .)
	# Don't link the .h files, or cp/call.c won't compile
	#-(cd $(GNU_OBJ)/gcc; ln -s $(GNU_SRC)/gcc/*.h .)
	-(cd $(GNU_OBJ)/libg++/src; ln -s $(GNU_SRC)/libg++/src/*.c* .)
	-(cd $(GNU_OBJ)/libg++/src; ln -s $(GNU_SRC)/libg++/src/*.h .)
	-(cd $(GNU_OBJ)/libg++/gperf/src; ln -s $(GNU_SRC)/libg++/gperf/src/*.c* .)
	-mkdir $(GNU_OBJ)/gcc/objc
	-(cd $(GNU_OBJ)/gcc/objc; ln -s $(GNU_SRC)/gcc/objc/*.c .)
	-(cd $(GNU_OBJ)/gcc/objc; ln -s $(GNU_SRC)/gcc/objc/*.h .)
	-(cd $(GNU_OBJ)/gcc/objc; ln -s $(GNU_SRC)/gcc/objc/*.m .)
	-(cd $(GNU_OBJ)/gcc/cp; ln -s $(GNU_SRC)/gcc/cp/*.c .)
	-(cd $(GNU_OBJ)/librx; ln -s $(GNU_SRC)/librx/*.c .)
	-(cd $(GNU_OBJ)/libstdc++; ln -s $(GNU_SRC)/libstdc++/*.cc .)
	-(cd $(GNU_OBJ)/libstdc++/stl; ln -s $(GNU_SRC)/libstdc++/stl/*.cc .)


$(GNU_OBJ): $(OBJ_DIR) 
	-mkdir "$(GNU_OBJ)"

# If we are building GNU programs in a bare distribution, then the OBJ.$(PTARCH)
# directory might not exist yet
$(OBJ_DIR):
	mkdir $(OBJ_DIR)

# DEC make can't seem to handle VPATH very well
GMAKE= `if [ -f $(GNU_OBJ)/make/make ]; \
	 then echo $(GNU_OBJ)/make/make ; \
	 else echo make; fi`


bin:
	echo $$PATH
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
HP_GNU_SRC=$(PTOLEMY)/src/gnu/src.hp

# See $PTOLEMY/src/gnu/src.hp/gcc/INSTALL for more info
#HP_MACH=`$(HP_GNU_SRC)/gcc/config.guess`
HP_MACH=hppa1.1-hp-hpux

hp_all: hp_configure hp_pregcc_install hp_bin install hp_all_gdb

# Binaries to be renamed after installation
HP_BINARIES= ar nm ranlib size strings strip
hp_post_install:
	for file in $(HP_BINARIES); \
	do \
		if [ -f $(GNU_EXEC_PREFIX)/bin/$$file ]; then \
			mv $(GNU_EXEC_PREFIX)/bin/$$file $(GNU_EXEC_PREFIX)/bin/g$$file; \
		fi; \
	done

hp_configure: $(GNU_OBJ)
	@echo "Configure everything, then go back and reconfigure gcc"
	@echo "The top level configure does not pass things down properly"
	(cd $(GNU_OBJ); \
	$(HP_GNU_SRC)/configure $(HP_MACH) --srcdir=$(HP_GNU_SRC) \
		--prefix=$(GNU_PREFIX) --exec-prefix=$(GNU_EXEC_PREFIX) \
		--with-gnu-as)
	@echo "Now reconfigure gcc with -gas"
	(cd $(GNU_OBJ)/gcc; \
	$(HP_GNU_SRC)/gcc/configure --srcdir=$(HP_GNU_SRC)/gcc \
		--prefix=$(GNU_PREFIX) --exec-prefix=$(GNU_EXEC_PREFIX) \
		--with-gnu-as hppa1.1-hp-hpux)
	@echo "Make links for the source files.  Some makes are not smart"
	@echo " enough to handle vpath"
	@echo "We could try installing GNU make first, but HPs hate it when"
	@echo "try to move a running binary, so installing make a second"
	@echo "time could fail."
	-(cd $(GNU_OBJ)/bfd; ln -s $(HP_GNU_SRC)/bfd/*.c .)
	-(cd $(GNU_OBJ)/opcodes; ln -s $(HP_GNU_SRC)/opcodes/*.c .)
	-(cd $(GNU_OBJ)/binutils; ln -s $(HP_GNU_SRC)/binutils/*.c .)
	-(cd $(GNU_OBJ)/gas; ln -s $(HP_GNU_SRC)/gas/*.c .)
	-(cd $(GNU_OBJ)/make; ln -s $(HP_GNU_SRC)/make/*.c .)
	-(cd $(GNU_OBJ)/make/glob; ln -s $(HP_GNU_SRC)/make/glob/*.h .)
	-(cd $(GNU_OBJ)/make/glob; ln -s $(HP_GNU_SRC)/make/glob/*.c .)
	-(cd $(GNU_OBJ)/libio; ln -s $(HP_GNU_SRC)/libio/*.c* .)
	-(cd $(GNU_OBJ)/libiberty; ln -s $(HP_GNU_SRC)/libiberty/*.c* .)
	-(cd $(GNU_OBJ)/libg++/src; ln -s $(HP_GNU_SRC)/libg++/src/*.c* .)
	-(cd $(GNU_OBJ)/libg++/src; ln -s $(HP_GNU_SRC)/libg++/src/*.h .)
	-(cd $(GNU_OBJ)/libg++/gperf/src; ln -s $(HP_GNU_SRC)/libg++/gperf/src/*.c* .)
	# Don't need gprof for Ptolemy.  BTW - gprof on hppa needs -lm
	#-(cd $(GNU_OBJ)/libg++/gprof/src; \
	#	ln -s $(HP_GNU_SRC)/libg++/gprof/src/*.c* .)
	-(cd $(GNU_OBJ)/gcc; ln -s $(HP_GNU_SRC)/gcc/*.c .)
	# Don't link the .h files, or cp/call.c won't compile
	#-(cd $(GNU_OBJ)/gcc; ln -s $(HP_GNU_SRC)/gcc/*.h .)
	-mkdir $(GNU_OBJ)/gcc/objc
	-(cd $(GNU_OBJ)/gcc/objc; ln -s $(HP_GNU_SRC)/gcc/objc/*.c .)
	-(cd $(GNU_OBJ)/gcc/objc; ln -s $(HP_GNU_SRC)/gcc/objc/*.h .)
	-(cd $(GNU_OBJ)/gcc/objc; ln -s $(HP_GNU_SRC)/gcc/objc/*.m .)
	-(cd $(GNU_OBJ)/gcc/cp; ln -s $(HP_GNU_SRC)/gcc/cp/*.c .)
	# We build gdb as a separate step, after we have GNU make
	#-(cd $(GNU_OBJ)/gcc/gdb; ln -s $(HP_GNU_SRC)/gcc/gdb/*.c .)
	#-(cd $(GNU_OBJ)/gcc/gdb; ln -s $(HP_GNU_SRC)/gcc/gdb/*.h .)
	# Building libg++ requires as to bin in gcc so -B arg will work
	-(cd $(GNU_OBJ)/gcc; ln -s ../gas/as.new as)
	-(cd $(GNU_OBJ)/librx; ln -s $(HP_GNU_SRC)/librx/*.c .)
	-(cd $(GNU_OBJ)/libstdc++; ln -s $(HP_GNU_SRC)/libstdc++/*.cc .)
	-(cd $(GNU_OBJ)/libstdc++/stl; ln -s $(HP_GNU_SRC)/libstdc++/stl/*.cc .)

# Set up links for GNU as.
hp_pregcc_install: $(GNU_DEST)/$(PTARCH)
	(cd $(GNU_OBJ)/gcc; $(MAKE)  install-dir)
	-ln -s $(GNU_OBJ)/gas/as.new $(GNU_OBJ)/gas/as
	-chmod a+x $(GNU_OBJ)/gas/as.new
	-ln -s $(GNU_EXEC_PREFIX)/bin/as  $(GNU_EXEC_PREFIX)/lib/gcc-lib/$(HP_MACH)/$(GCC_VERSION)/as

$(GNU_DEST)/$(PTARCH):
	if [ ! -d $@ ]; then \
		mkdir $@; \
	fi

# We must use gas with hp, so we change the path so the GNU as is first
hp_bin:
	#(cd $(GNU_OBJ); $(MAKE) -f $(PTOLEMY)/src/gnu/makefile bin)
	(PATH=$(GNU_OBJ)/gas:$(PATH) ;echo $$PATH; cd $(GNU_OBJ); $(MAKE) $(MFLAGS) CC="$(CC)" )

# Install HP GDB
# We have a separate directory in part because the bfd directories 
# from gdb and binutils are not easily shareable.
hp_all_gdb: hp_configure_gdb hp_bin_gdb hp_install_gdb

HP_GDB_SRC=$(HP_GNU_SRC)/gdb-4.13.u4

hp_configure_gdb: $(GNU_OBJ)/gdb
	(cd $(GNU_OBJ)/gdb; \
	$(HP_GDB_SRC)/configure $(HP_MACH) --srcdir=$(HP_GDB_SRC) \
		--prefix=$(GNU_PREFIX) --exec-prefix=$(GNU_EXEC_PREFIX) \
		--with-gnu-as)
$(GNU_OBJ)/gdb:
	if [ ! -d $@ ]; then \
		mkdir $@; \
	fi

# We assume you have GNU make installed here, otherwise you won't find alloca.c
hp_bin_gdb:
	cd $(GNU_OBJ)/gdb; $(GMAKE) $(MFLAGS) CC="$(CC)"

hp_install_gdb:
	cd $(GNU_OBJ)/gdb; $(GMAKE) $(MFLAGS) CC="$(CC)" install

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
