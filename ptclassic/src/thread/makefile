# Copyright (C) 1990-%Q% The Regents of the University of California.
#                       All Rights Reserved.
#		       
# Author:	T. M. Parks
# $Id$
#
# Important!  Do not run this makefile while in ~ptolemy/src/thread.  Instead
# run in from ~ptolemy/obj.$ARCH/thread (through the symbolic link) -- that
# way a separate directory containing, say, Sun-3 and Sun-4 objects can be
# maintained.


ROOT = ../..
OBJ = $(ROOT)/obj.$(ARCH)

DIRS =

ifneq (,$(filter sun%,$(ARCH)))
DIRS = Awesime awe lwp
endif

ifeq (cfront,$(ARCH))
DIRS = lwp
endif

ifeq (mips,$(ARCH))
DIRS = Awesime awe
endif

MAKEVARS = "ARCH=$(ARCH)"

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
	@echo "No need to do anything in thread directory for this ARCH."
endif


#
# Rules for the gthreads library.
#

THREAD_OBJ = $(OBJ)/thread
THREAD_INSTALL = $(ROOT)/thread
THREAD_INCLUDE = $(THREAD_INSTALL)/include.$(ARCH)
THREAD_LIB = $(THREAD_INSTALL)/lib.$(ARCH)

GT_OBJ = $(THREAD_OBJ)/gthreads
GT_SRC = ../$(ROOT)/src/thread/gthreads
GT_LIBS = libgthreads.a
GT_CC = gcc
GT_CFLAGS = -DASM_SETJMP -DNOERR_CHECK -DSTACK_CHECK -DSIGNAL_STACK \
	-DSRP -DC_INTERFACE
GT_COMP_ASM = $$(CC) $$(INC1) $$(INC2) $$(CFLAGS) -c $$*.S

#
# Build required directories if necessary.
#

$(THREAD_INSTALL):
	-mkdir $@

$(THREAD_INCLUDE) \
$(THREAD_LIB): $(THREAD_INSTALL)
	-mkdir $@

$(OBJ):
	-mkdir $@

$(THREAD_OBJ): $(OBJ)
	-mkdir $@

$(GT_OBJ): $(THREAD_OBJ)
	-mkdir $@

$(GT_OBJ)/include $(GT_OBJ)/src: $(GT_OBJ)
	-mkdir $@

$(GT_OBJ)/include/pthread: $(GT_OBJ)/include
	-mkdir $@

# Clean up.
gthreads_clean: $(GT_OBJ)/src
	(cd $(GT_OBJ)/src; $(MAKE) \
		LIBS=$(GT_LIBS) \
		clean; \
	)

# Make symbolic links for all source and header files.
gthreads_sources: $(GT_OBJ)/src $(GT_OBJ)/include \
	$(GT_OBJ)/include/pthread
	-(cd $(GT_OBJ)/include; ln -s ../$(GT_SRC)/include/*.h .)
	-(cd $(GT_OBJ)/include/pthread; ln -s ../../$(GT_SRC)/include/pthread/*.h .)
	-(cd $(GT_OBJ)/src; ln -s ../$(GT_SRC)/src/* .)

# Build and install everything.
gthreads_install: gthreads_$(ARCH) $(THREAD_INCLUDE) $(THREAD_LIB)
	-cp -r $(GT_OBJ)/include/* $(THREAD_INCLUDE)
	-cp $(GT_OBJ)/src/$(GT_LIBS) $(THREAD_LIB)

# Build objects for sol2.
gthreads_sol2: gthreads_sources 
	(cd $(GT_OBJ)/src; $(MAKE) $(MFLAGS) $(MAKEVARS) \
		CC=$(GT_CC) \
		CFLAGS='$(GT_CFLAGS) -DSOLARIS -DSVR4' \
		COMP_ASM='$(GT_COMP_ASM) -traditional' \
		LIBS=$(GT_LIBS) \
		all ; \
	)

# Build objects for sol2 using Sun's compiler.
gthreads_sol2.cfront: gthreads_sources
	(cd $(GT_OBJ)/src; $(MAKE) $(MFLAGS) $(MAKEVARS) \
		CC=cc \
		CFLAGS='$(GT_CFLAGS) -DSOLARIS -DSVR4' \
		CCFLAGS='-xO3 $$(CFLAGS)' \
		LIBS=$(GT_LIBS) \
		all ; \
	)

# Build objects for sun4.
gthreads_sun4: gthreads_sources
	(cd $(GT_OBJ)/src; $(MAKE) $(MFLAGS) $(MAKEVARS) \
		CC=$(GT_CC) \
		CFLAGS='$(GT_CFLAGS)' \
		COMP_ASM='$(GT_COMP_ASM)' \
		LIBS=$(GT_LIBS) \
		all ; \
	)
