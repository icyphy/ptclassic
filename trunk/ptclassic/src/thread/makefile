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
# Authors:	T. M. Parks, Christopher Hylands
# $Id$
#
# Important!  Do not run this makefile while in ~ptolemy/src/thread.  Instead
# run in from ~ptolemy/obj.$PTARCH/thread (through the symbolic link) -- that
# way a separate directory containing, say, Sun-3 and Sun-4 objects can be
# maintained.

ROOT = ../..

# get configuration info (Need INCLUDE_GTHREADS)
CONFIG=$(ROOT)/mk/config-$(PTARCH).mk
include $(CONFIG)

DIRS =

# sol2 and sol2.cfront
ifneq (,$(filter sol%,$(PTARCH)))
DIRS = posix
endif

# sun4
ifneq (,$(filter sun%,$(PTARCH)))
DIRS = Awesime awe lwp posix
endif

# cfront
ifeq (cfront,$(PTARCH))
DIRS = lwp posix
endif

# mips
ifeq (mips,$(PTARCH))
DIRS = Awesime awe
endif

MAKEVARS = "PTARCH=$(PTARCH)"

makefiles all install clean sources depend realclean checkjunk sccsinfo:
ifneq (,$(DIRS)) 
	@for x in $(DIRS); do \
	    if [ -w $$x ] ; then \
		( cd $$x ; \
		  echo making $@ in thread/$$x ; \
		  $(MAKE) $(MFLAGS) $(MAKEVARS) \
		      VPATH=../../../src/thread/$$x $@ ; \
		) \
	    fi ; \
	done
else
	@echo "No need to do anything in thread directory for this PTARCH."
endif

############################################################
# Directories for installing include files and libraries
# for thread packages.
THREAD_ROOT =		$(ROOT)/thread
THREAD_INCLUDE = 	$(THREAD_ROOT)/include.$(PTARCH)
THREAD_LIB = 		$(THREAD_ROOT)/lib.$(PTARCH)

$(THREAD_ROOT) $(THREAD_INCLUDE) $(THREAD_LIB):
	mkdir $@

#
# Rules for the gthreads package.
#

# Source and object directories for the gthreads package.
GT_SRC = $(ROOT)/src/thread/gthreads
GT_OBJ = $(ROOT)/obj.$(PTARCH)/thread/gthreads

# Redefined make variables.
GT_CFLAGS =	-DASM_SETJMP -DNOERR_CHECK -DSTACK_CHECK -DSIGNAL_STACK \
		-DC_INTERFACE
GT_COMP_ASM =	$$(CC) $$(INC1) $$(INC2) $$(CFLAGS) -c $$*.S

# Only the gthreads_install and gthreads_clean rules are externally visible.
# We build gthreads only on sol2 sol2.cfront and sun4

ifeq ($(INCLUDE_GTHREADS),yes)
# Install gthreads library and include files.
gthreads_install: $(THREAD_ROOT) $(THREAD_LIB) $(THREAD_INCLUDE) \
			gthreads_$(PTARCH)

# Clean up.
gthreads_clean: $(GT_OBJ)
	rm -rf $(GT_OBJ)

# Uninstall.
gthreads_uninstall:
	rm -rf $(THREAD_INCLUDE)/pthread.h $(THREAD_INCLUDE)/pthread
	rm -f $(THREAD_LIB)/libgthreads.a

else
gthreads_install:
	@echo "gthreads_install need not do anything for this architecture"
gthreads_clean:
	@echo "gthreads_clean need not do anything for this architecture"
gthreads_uninstall:
	@echo "gthreads_uninstall need not do anything for this architecture"
endif

# Build a shadow tree to hold object files.
GT_OBJ_SUBDIRS =	include lib src
GT_OBJ_SUBSUBDIRS =	include/pthread
$(GT_OBJ): $(GT_SRC)
	-mkdir $@
	-(cd $@; mkdir $(GT_OBJ_SUBDIRS) $(GT_OBJ_SUBSUBDIRS))
	-for mydir in $(GT_OBJ_SUBDIRS); \
	do \
		(cd $(GT_OBJ)/$$mydir; \
			ln -s ../../$(GT_SRC)/$$mydir/* .); \
	done
	-for mydir in $(GT_OBJ_SUBSUBDIRS); \
	do \
		(cd $(GT_OBJ)/$$mydir; \
			ln -s ../../../$(GT_SRC)/$$mydir/* .); \
	done
	# There is no default Makefile in the distribution.
	-cd $(GT_OBJ)/src; ln -s Makefile.Solaris Makefile

# Build library for sol2 using Gnu compiler.
gthreads_sol2: $(GT_OBJ)
	$(MAKE) -C $(GT_OBJ)/src $(MFLAGS) $(MAKEVARS) \
		INSTALL_INCLUDE='../../$(THREAD_INCLUDE)' \
		INSTALL_LIBS='../../$(THREAD_LIB)' \
		CC=gcc \
		CFLAGS='$(GT_CFLAGS) -DSOLARIS -DSVR4' \
		COMP_ASM='$(GT_COMP_ASM) -traditional' \
		all install;

# Solaris2.5, with gcc
gthreads_sol2.5: $(GT_OBJ)
	$(MAKE) -C $(GT_OBJ)/src $(MFLAGS) $(MAKEVARS) \
		INSTALL_INCLUDE='../../$(THREAD_INCLUDE)' \
		INSTALL_LIBS='../../$(THREAD_LIB)' \
		CC=gcc \
		CFLAGS='$(GT_CFLAGS) -DSOLARIS -DSVR4' \
		COMP_ASM='$(GT_COMP_ASM) -traditional' \
		all install;

# Build library for sol2 using Sun's compiler.
gthreads_sol2.cfront: $(GT_OBJ)
	$(MAKE) -C $(GT_OBJ)/src $(MFLAGS) $(MAKEVARS) \
		INSTALL_INCLUDE='../../$(THREAD_INCLUDE)' \
		INSTALL_LIBS='../../$(THREAD_LIB)' \
		CC=cc \
		CFLAGS='$(GT_CFLAGS) -DSOLARIS -DSVR4' \
		CCFLAGS='-xO3 $$(CFLAGS)' \
		CCFLAGS1='-xO1 $$(CFLAGS)' \
		CCFLAGS2='-xO2 $$(CFLAGS)' \
	    COMP_ASM='$$(AS) $$(INC1) $$(INC2) -P $$(CFLAGS) $$*.S -o $$*.o' \
		all install;

# Build library for sun4 using Gnu compiler.
gthreads_sun4: $(GT_OBJ)
	$(MAKE) -C $(GT_OBJ)/src $(MFLAGS) $(MAKEVARS) \
		INSTALL_INCLUDE='../../$(THREAD_INCLUDE)' \
		INSTALL_LIBS='../../$(THREAD_LIB)' \
		CC=gcc \
		CFLAGS='$(GT_CFLAGS)' \
		COMP_ASM='$(GT_COMP_ASM)' \
		all install;
