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
# This makefile is to be used to create small, stand-alone testing programs
# for the Ptolemy kernel.
#
# To use this, you need to construct a <filename>.cc file that has a main 
# function and accomplishes the test.  This file should be located in the 
# src directory of the library to test.  Then, just cd into the obj directory 
# and execute the make command:
# make -f $(ROOT)/mk/standalone.mk <stars.mk variable defs> <filename>.<suffix>
#
# where the suffix is one of: .bin, .debug, .purify, .quantify, .purecov
#
include makefile

include $(ROOT)/mk/stars.mk

%.bin: %.o $(PT_DEPEND)
	$(PURELINK) $(LINKER) $(LINKFLAGS) $< $(LIBS) -o $(@F)

%.debug: %.o 
	$(PURELINK) $(LINKER) $(LINKFLAGS_D) $< $(LIBS) -o $(@F)

%.purify: %.o
	$(PURIFY) $(LINKER) $(LINKFLAGS_D)  $< $(LIBS) -o $(@F)

%.quantify: %.o
	$(QUANTIFY) $(LINKER) $(LINKFLAGS) $< $(LIBS) -o $(@F)

$.purecov: %.o
	$(PURECOV) $(LINKER) $(LINKFLAGS) $< $(LIBS) -o $(@F)
