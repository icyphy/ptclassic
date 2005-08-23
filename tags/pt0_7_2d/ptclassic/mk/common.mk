# common included makefile for all directories in which a makefile is
# built from a make.template file.
#
# Version identification:
# @(#)common.mk	1.74 02/18/99
#
# Copyright (c) 1990-1998 The Regents of the University of California.
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
# Date of creation: 6/15/92.
# Written by: J. Buck
#
# The following symbols are used by this makefile.  They should be
# defined by the parent makefile prior to including this makefile:
# SRCS
# OBJS
# PTLIB
# DOMAIN, DOMAIN_2, DOMAIN_3
# STAR_MK	Specifies the name of the star-reference file to build;
#		both a C-verion (e.g., cgcstars.c) using genStarTable and
#		a makefile version (e.g., cgcstars.mk) using genStarList 
#		is built.  The variable "STAR_MK" should be given without
#		a file suffix (e.g., just "cgcstars").
# GENSTARVARS	A variable passed to genStarTable to allow stars other
#		than those defined by PL_SRCS to be generated.

# This file is used when there are files to be compiled.  Usually a 
# make.template file is present if there are files to be compiled.
# See no-compile.mk for a similar file that is to be used when there
# are no files to be compiled.

whatToBuild:	all

.SUFFIXES:	.o .cc .h .htm .pl .chdl .is

# Include rules for compiling C and C++ code, building libraries, etc.
include $(ROOT)/mk/compile.mk

# Note that forcing the installation of ptlang might not be the best
# thing to do, it would be best if 'make sources' did not touch the
# bin.$(PTARCH) directory, so we check to see if there is a ptlang in the
# obj.$(PTARCH)/ptlang directory and use it.  This is awkward, but the
# GNU tools do something similar

# ptlang binary in the obj directory
PTLANG_OBJ_DIR=$(PTOLEMY)/obj.$(PTARCH)/ptlang
# BINARY_EXT is set to .exe for nt4
PTLANG_IN_OBJ=$(PTLANG_OBJ_DIR)/ptlang$(BINARY_EXT)
PTLANG_VPATH=../../src/ptlang

# Use either the ptlang binary in the obj directory or just use ptlang
PTLANG= `if [ ! -f $(PTLANG_IN_OBJ) ]; \
	then (cd $(PTLANG_OBJ_DIR) ; \
		  $(MAKE) ptlang VPATH=$(PTLANG_VPATH) >/dev/null 2>&1); fi; \
	 if [ ! -f $(PTLANG_IN_OBJ) ]; \
	 then echo ptlang; \
	 else echo $(PTLANG_IN_OBJ); fi`

# Build the ptlang binary if necessary
$(PTLANG_IN_OBJ):
	(cd $(PTLANG_OBJ_DIR); $(MAKE) VPATH=$(PTLANG_VPATH))

# islang binary in the obj directory
ISLANG_OBJ_DIR=$(PTOLEMY)/obj.$(PTARCH)/domains/ipus/icp/islang
ISLANG_IN_OBJ=$(ISLANG_OBJ_DIR)/islang
ISLANG_VPATH=../../../../../src/domains/ipus/icp/islang

# Use islang on the path; if it has not been installed yet, then install it
ISLANG= `if [ ! -f $(ISLANG_IN_OBJ) ]; \
	 then (cd $(ISLANG_OBJ_DIR) ; \
		  $(MAKE) $(ISLANG_IN_OBJ) VPATH=$(ISLANG_VPATH) >/dev/null 2>&1); fi; \
	 if [ ! -f $(ISLANG_IN_OBJ) ]; \
	 then echo islang; \
	 else echo $(ISLANG_IN_OBJ); fi`

# Build the islang binary if necessary
$(ISLANG_IN_OBJ):
	(cd $(ISLANG_OBJ_DIR); $(MAKE) VPATH=$(ISLANG_VPATH))

# pepp binary in the obj directory.  Pepp is used by Thor to convert
# .chdl files to .cc files.  'make sources' may need pepp
PEPP_OBJ_DIR=$(PTOLEMY)/obj.$(PTARCH)/domains/thor/pepp
PEPP_IN_OBJ=$(PEPP_OBJ_DIR)/pepp
PEPP_VPATH=../../../../src/domains/thor/pepp

# Use either the pepp binary in the obj directory or just use pepp
# I don't know why we can't use a rule like the ones for ptlang
# but it just seems to be a bug in gmake.  So instead, we run
# make by hand if necessary
PEPP= `if [ ! -f $(PEPP_IN_OBJ) ]; \
	then $(MAKE) $(PEPP_IN_OBJ) >/dev/null 2>&1; fi; \
	if [ ! -f $(PEPP_IN_OBJ) ]; \
	then echo pepp; \
	else echo $(PEPP_IN_OBJ); fi`

# Build the pepp binary if necessary
$(PEPP_IN_OBJ):
	(cd $(PEPP_OBJ_DIR); $(MAKE) VPATH=$(PEPP_VPATH) pepp)

# Rules for running the ptlang processor
# Make sure we always run the preprocessor in the source directory
# the "mv" part moves the documentation to the doc dir.
# note if there is no doc dir, the command continues despite the error.
.pl.cc: $(PTLANG_IN_OBJ)
	cd $(VPATH); $(PTLANG) $< 
	@$(STARDOCRULE)

.pl.h: $(PTLANG_IN_OBJ)
	cd $(VPATH); $(PTLANG) $< 
	@$(STARDOCRULE)


.pl.htm: $(PTLANG_IN_OBJ)
	cd $(VPATH); $(PTLANG) -htm $< 
	@$(STARDOCRULE)

# Rules for running the islang processor
# Make sure we always run the preprocessor in the source directory
# the "mv" part moves the documentation to the doc dir.
# note if there is no doc dir, the command continues despite the error.
.is.cc: $(ISLANG_IN_OBJ)
	cd $(VPATH); $(ISLANG) $<
	@$(STARDOCRULE)

.is.h: $(ISLANG_IN_OBJ)
	cd $(VPATH); $(ISLANG) $<
	@$(STARDOCRULE)

# Rule for the thor preprocessor
# Make sure we always run the preprocessor in the source directory
.chdl.cc: $(PEPP_IN_OBJ)
	cd $(VPATH); $(PEPP) $<

# Rule for making a star list for inclusion by make
$(VPATH)/$(STAR_MK).mk:	make.template
		cd $(VPATH); rm -f $(STAR_MK).mk
		cd $(VPATH); genStarList $(DOMAIN) $(DOMAIN_2) $(DOMAIN_3) > $(STAR_MK).mk

# Rule for making a star list module for pulling stars out of libraries
$(VPATH)/$(STAR_MK).c:	make.template
		cd $(VPATH); rm -f $(STAR_MK).c
		cd $(VPATH); $(ROOT)/bin/genStarTable $(MAKE) $(GENSTARVARS) $(DOMAIN) $(DOMAIN_2) $(DOMAIN_3) > $(STAR_MK).c

$(STAR_MK).o:	$(VPATH)/$(STAR_MK).c

# Standard "all" for stars directories
stars_all:	$(SRCS) $(PTLIB) $(STAR_MK).o

# Rule for installing both the star library and star list
stars_install:	all $(LIBDIR)/$(PTLIB) $(LIBDIR)/$(STAR_MK).o
		@echo Installation complete.

# Rule for installing the star list

$(LIBDIR)/$(STAR_MK).o:	$(STAR_MK).o
		rm -f $@
		ln $(STAR_MK).o $(LIBDIR)

# Matlab settings
include $(ROOT)/mk/matlab.mk

# Mathematica settings
include $(ROOT)/mk/mathematica.mk
