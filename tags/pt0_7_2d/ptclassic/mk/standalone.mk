# @(#)standalone.mk	1.8 07/19/96

# Copyright (c) 1990-1996 The Regents of the University of California.
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
# Programmer:  Jose Luis Pino
#
# This makefile is to be used to create small, stand-alone programs
# that use parts of Ptolemy kernel or just the pure mk rule definitions
#
# To use this, you need to construct a single <filename>.cc file 
# that defines a main function. The usage of this mk file is:
#
# make -f $(ROOT)/mk/standalone.mk <stars.mk variable defs> <filename>.<suffix>
#
# where the suffix is one of: .bin, .debug, .purify, .quantify, .purecov
#
# matlab.mk and mathematica.mk check these vars before traversing the path
# 

NEED_MATLABDIR =        1
NEED_MATHEMATICADIR =   1

ROOT=$(PTOLEMY)
OBJDIR=$(ROOT)/obj.$(PTARCH)

include $(ROOT)/mk/config-$(PTARCH).mk

ifndef NOPTOLEMY
	include $(ROOT)/mk/stars.mk
	INCL= $(foreach dir,$(CUSTOM_DIRS),-I$(ROOT)$(dir))
	# unfortunately, if we are building a stand-alone program and
	# link in libPar we must also get gantt function definitions
	ifdef CGPAR
		VERSION=version.o
	endif
endif

#if we define NOPTOLEMY variable, we just want to use the pure make commands &
#none of the PTOLEMY libs
ifdef NOPTOLEMY
	#Remove rpath which may have been defined
	SHARED_LIBRARY_R_LIST=
endif

VPATH=.

ifndef LIB
	#This definition is needed so that make won't complain w/ common.mk
	LIB=dummy
endif

include $(ROOT)/mk/common.mk

version.o:
	echo "char DEFAULT_DOMAIN[] = \"SDF\";" >> version.c
	$(CC) -c version.c

%.bin: %.o $(PT_DEPEND) $(VERSION)
	$(PURELINK) $(LINKER) $(LINKFLAGS) $< $(STARS) $(TARGETS) $(LIBS) -o $(@F)

%.debug: %.o  $(PT_DEPEND) $(VERSION)
	$(PURELINK) $(LINKER) $(LINKFLAGS_D) $< $(STARS) $(TARGETS) $(LIBS) -o $(@F)

%.purify: %.o $(PT_DEPEND) $(VERSION)
	$(PURIFY) $(LINKER) $(LINKFLAGS_D)  $<  $(STARS) $(TARGETS) $(LIBS) -o $(@F)

%.quantify: %.o $(PT_DEPEND) $(VERSION)
	$(QUANTIFY) $(LINKER) $(LINKFLAGS) $<  $(STARS) $(TARGETS) $(LIBS) -o $(@F)

$.purecov: %.o $(PT_DEPEND) $(VERSION)
	$(PURECOV) $(LINKER) $(LINKFLAGS) $<  $(STARS) $(TARGETS) $(LIBS) -o $(@F)






