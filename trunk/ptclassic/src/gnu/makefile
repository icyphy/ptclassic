# Makefile for $PTOLEMY/src/gnu, used to compile the GNU tools
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
#

# This makefile has only been tested with Solaris2.4, SunOS4.1.3 and
# HPUX10.20.  It may work on other platforms, but we have not tried.

# This makefile will install the GNU tools for non-hp and hp platforms.
# hp's need GNU gas for -g to work, and they don't come with lex or yacc,
# so we need some additional rules to build the GNU tools for hp.

# See the README file in this directory for details.

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

# If you change gcc versions, you should look at the defines in
# $PTOLEMY/.cshrc and $PTOLEMY/bin/g++-setup make sure that they
# refer to the right version.  Also, $PTOLEMY/src/gnu/makefile contains
# references to the current version, along with
# $PTOLEMY/src/gnu/README.
# Also, check out the COLLECTOR variables in $PTOLEMY/mk/config-*.mk
GCC_VERSION=2.7.2.1
# If you are trying to build GNU for Linux, you may want to uncomment
# the next line and comment out the following line
#LIBGPLUSPLUS_VERSION=27.1.0
LIBGPLUSPLUS_VERSION=2.7.2

# Machine dependent files
GNU_EXEC_PREFIX=$(GNU_DEST)/$(PTARCH)

# Location of gnu bin directory relative to Ptolemy bin directory
# Can't use absolute path name here, or we will create a distribution that
# will not work if $PTOLEMY != /users/ptolemy
REL_GNU_BINDIR=../gnu/$(PTARCH)/bin

# Location of Ptolemy bin dir.  We make links from here to the GNU stuff
BINDIR=$(PTOLEMY)/bin.$(PTARCH)

############################################################
# Build the GNU tools for non-HP machines
all: configure bin install
.PHONY:	configure bin install

MACH=`$(GNU_SRC)/gcc/config.guess`

SHAREDLIB_CONFIGURE_FLAGS = --enable-shared
configure: $(GNU_OBJ)
	-mkdir $(GNU_OBJ)/make
	(cd $(GNU_OBJ)/make; \
	$(GNU_SRC)/make/configure --srcdir=$(GNU_SRC)/make \
		--prefix=$(GNU_PREFIX) --exec-prefix=$(GNU_EXEC_PREFIX) \
		$(SHAREDLIB_CONFIGURE_FLAGS) $(MACH) )
	@echo "Some non-GNU makes can't deal with VPATH"
	-(cd $(GNU_OBJ)/make; ln -s $(GNU_SRC)/make/*.c .)
	-(cd $(GNU_OBJ)/make/glob; ln -s $(GNU_SRC)/make/glob/*.h .)
	-(cd $(GNU_OBJ)/make/glob; ln -s $(GNU_SRC)/make/glob/*.c .)
	@echo "Now configure gcc"
	-mkdir $(GNU_OBJ)/gcc
	(cd $(GNU_OBJ)/gcc; \
	$(GNU_SRC)/gcc/configure --srcdir=$(GNU_SRC)/gcc \
		--prefix=$(GNU_PREFIX) --exec-prefix=$(GNU_EXEC_PREFIX) \
		$(SHAREDLIB_CONFIGURE_FLAGS) \
		$(MACH) )
	@echo "Now configure libg++"
	-mkdir $(GNU_OBJ)/libg++
	(cd $(GNU_OBJ)/libg++; \
	$(GNU_SRC)/libg++/configure --srcdir=$(GNU_SRC)/libg++ \
		--prefix=$(GNU_PREFIX) --exec-prefix=$(GNU_EXEC_PREFIX) \
		$(SHAREDLIB_CONFIGURE_FLAGS) $(MACH) )

# DEC make can't seem to handle VPATH very well
#GMAKE= `if [ -f $(GNU_OBJ)/make/make ]; \
#	 then echo $(GNU_OBJ)/make/make ; \
#	 else echo make; fi`

bin: bin_make bin_gcc install_gcc bin_libg++

bin_make:
	(cd $(GNU_OBJ)/make; $(MAKE) )

# Build gcc with itself.  See src/gnu/src/gcc/INSTALL for more info.
# Use the gcc '-v ' flag to see whether your gcc was compiled with cc or gcc
# We pass CC down in case we are trying to avoid using /usr/bin/cc under
# Solaris.
bin_gcc:
	(cd $(GNU_OBJ)/gcc; $(GNU_OBJ)/make/make CC=$(CC) bootstrap)

install_gcc: $(BINDIR)
	(cd $(GNU_OBJ)/make; $(MAKE) install)
	(cd $(GNU_OBJ)/gcc; $(GNU_OBJ)/make/make install CC="stage2/xgcc -Bstage2/" CFLAGS="-g -O")
	@echo "Install c++filt, the g++ name demangler"
	-(cd $(GNU_OBJ)/gcc; cp c++filt $(BINDIR))
	@echo "Set up some links so we can move the compiler"
	-(cd $(GNU_DEST)/$(PTARCH); ln -s $(MACH) $(PTARCH) )
	-(cd $(GNU_DEST)/$(PTARCH)/lib/gcc-lib; ln -s $(MACH) $(PTARCH) )
	-(cd $(BINDIR); ln -s $(REL_GNU_BINDIR)/* .)

bin_libg++:
	(cd $(GNU_OBJ)/libg++; \
		PATH=$(GNU_OBJ)/binutils/gas:$(GNU_OBJ)/gcc:$(PATH); \
		echo $$PATH; \
		$(GNU_OBJ)/make/make \
		CC=$(GNU_OBJ)/gcc/xgcc CFLAGS=-B$(GNU_OBJ)/gcc/ \
		LDFLAGS=-B$(GNU_OBJ)/gcc/ )

install: $(BINDIR) 
	(cd $(GNU_OBJ)/libg++; \
		PATH=$(GNU_OBJ)/binutils/gas:$(GNU_OBJ)/gcc:$(PATH); \
		echo $$PATH; \
		$(GNU_OBJ)/make/make \
		CC=$(GNU_OBJ)/gcc/xgcc CFLAGS=-B$(GNU_OBJ)/gcc/ \
		LDFLAGS=-B$(GNU_OBJ)/gcc/ install)
	-(cd $(BINDIR); ln -s $(REL_GNU_BINDIR)/* .)
	-(cd $(BINDIR); \
	  rm -f gperf unprotoize protoize hp* make-* sparc-* c++ make.old)
	# Make local relative links here
	# On the hppa, we remove these later
	-(cd $(GNU_DEST)/$(PTARCH)/lib; rm -f libstdc++.so libg++.so; \
		ln -s libg++.so.$(LIBGPLUSPLUS_VERSION) libg++.so; \
		ln -s libstdc++.so.$(LIBGPLUSPLUS_VERSION) libstdc++.so)
	# Create links in libio for info files
	-(cd $(GNU_OBJ)/libg++/libio; \
		ln -s $(GNU_SRC)/libg++/libio/*.info* .)
	-(cd $(GNU_OBJ)/libg++/libg++; \
		ln -s $(GNU_SRC)/libg++/libg++/*.info* .)
	-(cd $(GNU_OBJ)/libg++/libg++/gperf; \
		ln -s $(GNU_SRC)/libg++/libg++/gperf/*.info* .)
	-(cd $(GNU_OBJ)/libg++; $(GNU_OBJ)/make/make install-info)

############################################################
# Build the GNU tools for HP machines
#
# See src/gnu/INSTALL for the details, but basically, we install a special
# GNU assembler so the the -g flag will work.
# Also, we need to install gcc before trying to build libg++



hp_all: hp_configure hp_pregcc_install hp_bin hp_postgcc_install install hp_post_install

hp_configure: $(GNU_OBJ)
	@echo "Configure binutils, as we need gas for debugging"
	-mkdir $(GNU_OBJ)/binutils
	(cd $(GNU_OBJ)/binutils; \
	$(GNU_SRC)/binutils/configure --srcdir=$(GNU_SRC)/binutils \
		--prefix=$(GNU_PREFIX) --exec-prefix=$(GNU_EXEC_PREFIX) \
		$(SHAREDLIB_CONFIGURE_FLAGS) $(MACH) )
	@echo "hpux10 does not include yacc, so we include bison"
	-mkdir $(GNU_OBJ)/bison
	(cd $(GNU_OBJ)/bison; \
	$(GNU_SRC)/bison/configure --srcdir=$(GNU_SRC)/bison \
		--prefix=$(GNU_PREFIX) --exec-prefix=$(GNU_EXEC_PREFIX) \
		$(SHAREDLIB_CONFIGURE_FLAGS) $(MACH) )
	@echo "wait to configure flex until we have gcc installed
	-mkdir $(GNU_OBJ)/make
	(cd $(GNU_OBJ)/make; \
	$(GNU_SRC)/make/configure --srcdir=$(GNU_SRC)/make \
		--prefix=$(GNU_PREFIX) --exec-prefix=$(GNU_EXEC_PREFIX) \
		$(SHAREDLIB_CONFIGURE_FLAGS) $(MACH) )
	@echo "Now configure gcc with -with-gnu-as"
	-mkdir $(GNU_OBJ)/gcc
	(cd $(GNU_OBJ)/gcc; \
	$(GNU_SRC)/gcc/configure --srcdir=$(GNU_SRC)/gcc \
		--prefix=$(GNU_PREFIX) --exec-prefix=$(GNU_EXEC_PREFIX) \
		$(SHAREDLIB_CONFIGURE_FLAGS) \
		--with-gnu-as $(MACH) )
	@echo "Now configure libg++"
	-mkdir $(GNU_OBJ)/libg++
	(cd $(GNU_OBJ)/libg++; \
	$(GNU_SRC)/libg++/configure --srcdir=$(GNU_SRC)/libg++ \
		--prefix=$(GNU_PREFIX) --exec-prefix=$(GNU_EXEC_PREFIX) \
		$(SHAREDLIB_CONFIGURE_FLAGS) $(MACH) )

hp_pregcc_install: $(GNU_DEST)/$(PTARCH)
	if [ ! -d $(GNU_OBJ)/gas ]; then mkdir $(GNU_OBJ)/gas; fi
	(cd $(GNU_OBJ)/gcc; $(MAKE)  install-dir)
	-ln -s $(GNU_OBJ)/binutils/gas/as.new $(GNU_OBJ)/binutils/gas/as
	-ln -s $(GNU_OBJ)/binutils/gas/as.new $(GNU_OBJ)/gcc/as
	-chmod a+x $(GNU_OBJ)/binutils/gas/as.new
	-ln -s $(GNU_EXEC_PREFIX)/bin/as  $(GNU_EXEC_PREFIX)/lib/gcc-lib/$(MACH)/$(GCC_VERSION)/as

hp_bin: bin_make hp_bin_binutils bin_gcc install_gcc hp_install_gas hp_bin_others bin_libg++

hp_bin_binutils:
	(cd $(GNU_OBJ)/binutils; $(GNU_OBJ)/make/make)

hp_install_gas:
	(cd $(GNU_OBJ)/binutils/gas; $(GNU_OBJ)/make/make install)

hp_bin_others:
	(cd $(GNU_OBJ)/bison; $(GNU_OBJ)/make/make)
	# Reconfigure flex so as to avoid alloca problems
	-mkdir $(GNU_OBJ)/flex
	(cd $(GNU_OBJ)/flex; \
	CC=$(BINDIR)/gcc; \
	$(GNU_SRC)/flex/configure --srcdir=$(GNU_SRC)/flex \
		--prefix=$(GNU_PREFIX) --exec-prefix=$(GNU_EXEC_PREFIX) \
		$(SHAREDLIB_CONFIGURE_FLAGS) $(MACH) )
	(cd $(GNU_OBJ)/flex; $(GNU_OBJ)/make/make \
		CC=$(BINDIR)/gcc; \
		YACC="$(GNU_OBJ)/bison/bison -y")

hp_postgcc_install:
	(cd $(GNU_OBJ)/bison; $(GNU_OBJ)/make/make install)
	(cd $(GNU_OBJ)/flex; $(GNU_OBJ)/make/make \
		YACC="$(GNU_OBJ)/bison/bison -y" install)

# Binaries to be renamed after installation
HP_BINARIES= ar nm ranlib size strings strip
hp_post_install:
	for file in $(HP_BINARIES); \
	do \
		if [ -f $(GNU_EXEC_PREFIX)/bin/$$file ]; then \
			mv $(GNU_EXEC_PREFIX)/bin/$$file $(GNU_EXEC_PREFIX)/bin/g$$file; \
		fi; \
	done
	# The hp uses .sl files, so remove the .so files we created in install
	rm -f $(GNU_DEST)/$(PTARCH)/lib/libg++.so.$(LIBGPLUSPLUS_VERSION)
	rm -f $(GNU_DEST)/$(PTARCH)/lib/libstdc++.so.$(LIBGPLUSPLUS_VERSION)

############################################################
# Build the GNU tools for SGI IRIX5 machines
# This rule has not been tested inhouse at UCB, but it should work
#
# See src/gnu/INSTALL for the details, but basically, we install a special
# GNU assembler so the the -g flag will work.
# Also, we need to install gcc before trying to build libg++

irix5_all: irix5_configure bin install
irix5_configure: $(GNU_OBJ)
	-mkdir $(GNU_OBJ)/make
	(cd $(GNU_OBJ)/make; \
	$(GNU_SRC)/make/configure --srcdir=$(GNU_SRC)/make \
		--prefix=$(GNU_PREFIX) --exec-prefix=$(GNU_EXEC_PREFIX) \
		$(SHAREDLIB_CONFIGURE_FLAGS) $(MACH) )
	@echo "Some non-GNU makes can't deal with VPATH"
	-(cd $(GNU_OBJ)/make; ln -s $(GNU_SRC)/make/*.c .)
	-(cd $(GNU_OBJ)/make/glob; ln -s $(GNU_SRC)/make/glob/*.h .)
	-(cd $(GNU_OBJ)/make/glob; ln -s $(GNU_SRC)/make/glob/*.c .)
	@echo "Now configure gcc with -with-gnu-as"
	-mkdir $(GNU_OBJ)/gcc
	(cd $(GNU_OBJ)/gcc; \
	$(GNU_SRC)/gcc/configure --srcdir=$(GNU_SRC)/gcc \
		--prefix=$(GNU_PREFIX) --exec-prefix=$(GNU_EXEC_PREFIX) \
		$(SHAREDLIB_CONFIGURE_FLAGS) \
		--with-gnu-as $(MACH) )
	@echo "Now configure libg++"
	-mkdir $(GNU_OBJ)/libg++
	(cd $(GNU_OBJ)/libg++; \
	$(GNU_SRC)/libg++/configure --srcdir=$(GNU_SRC)/libg++ \
		--prefix=$(GNU_PREFIX) --exec-prefix=$(GNU_EXEC_PREFIX) \
		$(SHAREDLIB_CONFIGURE_FLAGS) $(MACH) )

############################################################
# dejaGNU
# dejaGNU is the GNU test suite system.  We don't ship it yet.
# Note that dejaGNU uses tcl and expect, but since it uses tcl7.3, we 
# let dejaGNU install its own tcl rather than using tcl7.4 or tcl7.5

configure_dejagnu: $(GNU_OBJ)
	-mkdir $(GNU_OBJ)/dejagnu
	(cd $(GNU_OBJ)/dejagnu; \
	$(GNU_SRC)/dejagnu/configure --srcdir=$(GNU_SRC)/dejagnu \
		--prefix=$(GNU_PREFIX) --exec-prefix=$(GNU_EXEC_PREFIX) \
		$(SHAREDLIB_CONFIGURE_FLAGS) $(MACH) )
bin_dejagnu:
	(cd $(GNU_OBJ)/dejagnu; $(MAKE) )

install_dejagnu:
	(cd $(GNU_OBJ)/dejagnu; $(MAKE) install)
	(cd $(GNU_OBJ)/dejagnu/dejagnu; $(MAKE) install-info)
	@echo"Move libtcl.a to dejagnu so we don't get confused later"
	mv $(GNU_EXEC_PREFIX)/lib/libtcl.a $(GNU_EXEC_PREFIX)/lib/dejagnu
	mkdir $(GNU_PREFIX)/include/dejagnu
	mv $(GNU_PREFIX)/include/tcl.h $(GNU_PREFIX)/include/dejagnu

############################################################
# Create various directories

$(GNU_OBJ): $(OBJ_DIR) 
	-mkdir "$(GNU_OBJ)"

# If we are building GNU programs in a bare distribution, then 
# the OBJ.$(PTARCH) directory might not exist yet
$(OBJ_DIR):
	mkdir $(OBJ_DIR)

$(GNU_DEST)/$(PTARCH):
	# If $@ is a link, then the mkdir may fail, which is ok
	-if [ ! -d $@ ]; then \
		if [ ! -d $(GNU_DEST) ]; then \
			mkdir $(GNU_DEST); \
		fi; \
		mkdir $@; \
	fi

$(BINDIR):
	mkdir $(BINDIR)


############################################################
# Rules to clean by
clean:
	(cd $(GNU_OBJ); $(MAKE) clean)

realclean:
	(cd $(GNU_OBJ); $(MAKE) realclean)

distclean:
	(cd $(GNU_OBJ); $(MAKE) distclean)
