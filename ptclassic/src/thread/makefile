# Copyright (C) 1990-1995 The Regents of the University of California.
#                       All Rights Reserved.
#		       
# Author:	T. M. Parks
# $Id$
#
# Important!  Do not run this makefile while in ~ptolemy/src/thread.  Instead
# run in from ~ptolemy/obj.$PTARCH/thread (through the symbolic link) -- that
# way a separate directory containing, say, Sun-3 and Sun-4 objects can be
# maintained.

ROOT = ../..

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
THREAD_INCLUDE = $(ROOT)/thread/include.$(PTARCH)
THREAD_LIB = $(ROOT)/thread/lib.$(PTARCH)

$(THREAD_INCLUDE) $(THREAD_LIB) $(THREAD_INCLUDE)/pthread :
	mkdir $@

#
# Rules for the gthreads package.
#

# Source and object directories for the gthreads package.
GT_SRC = $(ROOT)/src/thread/gthreads
GT_OBJ = $(ROOT)/obj.$(PTARCH)/thread/gthreads

# Include files that need to be installed.
GT_INCLUDE= $(THREAD_INCLUDE)/pthread.h \
 $(THREAD_INCLUDE)/pthread/config.h \
 $(THREAD_INCLUDE)/pthread/errno.h \
 $(THREAD_INCLUDE)/pthread/limits.h \
 $(THREAD_INCLUDE)/pthread/pthread_asm.h \
 $(THREAD_INCLUDE)/pthread/signal.h \
 $(THREAD_INCLUDE)/pthread/stdtypes.h \
 $(THREAD_INCLUDE)/pthread/unistd.h

# Redefined make variables.
GT_CFLAGS = 	-DASM_SETJMP -DNOERR_CHECK -DSTACK_CHECK -DC_CONTEXT_SWITCH \
		-DSRP -DC_INTERFACE
GT_COMP_ASM =	$$(CC) $$(INC1) $$(INC2) $$(CFLAGS) -c $$*.S

# Install gthreads library and include files.
gthreads_install: $(THREAD_LIB) $(THREAD_INCLUDE) $(THREAD_INCLUDE)/pthread \
	$(THREAD_LIB)/libgthreads.a $(GT_INCLUDE)

# Clean up.
gthreads_clean: $(GT_OBJ)
	$(MAKE) -C $(GT_OBJ)/src clean;

# Install gthreads include files.
$(THREAD_INCLUDE)/pthread/config.h:	$(GT_OBJ)/include/pthread/config.h
	cp $< $@

$(THREAD_INCLUDE)/pthread/%.h: $(GT_SRC)/pthread/%.h
	cp $< $@

$(THREAD_INCLUDE)/%.h: $(GT_SRC)/include/%.h
	cp $< $@

# Install gthreads library.
$(THREAD_LIB)/libgthreads.a: $(GT_OBJ)/lib/libgthreads.a
	cp $< $@

# Build a shadow tree to hold object files.
$(GT_OBJ): $(GT_SRC)
	mkdir $@
	lndir ../$< $@

# Build gthreads library.
$(GT_OBJ)/lib/libgthreads.a: gthreads_$(PTARCH)

# Build library for sol2 using Gnu compiler.
gthreads_sol2: $(GT_OBJ)
	$(MAKE) -C $(GT_OBJ)/src $(MFLAGS) $(MAKEVARS) \
		CC=gcc \
		CFLAGS='$(GT_CFLAGS) -DSOLARIS -DSVR4' \
		CCFLAGS='$$(CFLAGS)' \
		CCFLAGS1='$$(CFLAGS)' \
		CCFLAGS2='$$(CFLAGS)' \
		COMP_ASM='$(GT_COMP_ASM) -traditional' \
		all;

# Build library for sol2 using Sun's compiler.
gthreads_sol2.cfront: $(GT_OBJ)
	$(MAKE) -C $(GT_OBJ)/src $(MFLAGS) $(MAKEVARS) \
		CC=cc \
		CFLAGS='$(GT_CFLAGS) -DSOLARIS -DSVR4' \
		CCFLAGS='$$(CFLAGS)' \
		CCFLAGS1='$$(CFLAGS)' \
		CCFLAGS2='$$(CFLAGS)' \
		all;

# Build library for sun4 using Gnu compiler.
gthreads_sun4: $(GT_OBJ)
	$(MAKE) -C $(GT_OBJ)/src $(MFLAGS) $(MAKEVARS) \
		CC=gcc \
		CFLAGS='$(GT_CFLAGS)' \
		CCFLAGS='$$(CFLAGS)' \
		CCFLAGS1='$$(CFLAGS)' \
		CCFLAGS2='$$(CFLAGS)' \
		COMP_ASM='$(GT_COMP_ASM)' \
		all;
