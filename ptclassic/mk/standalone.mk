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

ROOT=$(PTOLEMY)

ifndef NOPTOLEMY
	include $(ROOT)/mk/stars.mk
	INCL= $(foreach dir,$(CUSTOM_DIRS),-I$(ROOT)$(dir))
endif

include $(ROOT)/mk/config-$(PTARCH).mk

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

%.bin: %.o $(PT_DEPEND)
	$(PURELINK) $(LINKER) $(LINKFLAGS) $< $(LIBS) -o $(@F)

%.debug: %.o  $(PT_DEPEND)
	$(PURELINK) $(LINKER) $(LINKFLAGS_D) $< $(LIBS) -o $(@F)

%.purify: %.o $(PT_DEPEND)
	$(PURIFY) $(LINKER) $(LINKFLAGS_D)  $< $(LIBS) -o $(@F)

%.quantify: %.o $(PT_DEPEND)
	$(QUANTIFY) $(LINKER) $(LINKFLAGS) $< $(LIBS) -o $(@F)

$.purecov: %.o $(PT_DEPEND)
	$(PURECOV) $(LINKER) $(LINKFLAGS) $< $(LIBS) -o $(@F)






