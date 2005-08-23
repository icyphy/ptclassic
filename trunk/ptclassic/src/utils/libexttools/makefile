# Makefile for external tool interface library libexttools
# Version identification:
# @(#)make.template	1.14	08/25/97
# Copyright (c) 1990-1997 The Regents of the University of California.
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
# Date of creation: 11/6/95
#
# If this file's name is "make.template" and there is no makefile in
# this directory, do
#	cp make.template makefile
#	make depend
# This will add the dependencies to the makefile.

# Root of Ptolemy directory
ROOT=../../..
# Where we are
VPATH = ../../../src/utils/libexttools
# get configuration info
CONFIG=$(ROOT)/mk/config-$(PTARCH).mk
include $(CONFIG)

# Library name
PTLIB=libexttools.$(LIBSUFFIX)

# Used to create .dlls under nt4.vc
DLLDEPS = ..\..\kernel\libptolemy.lib \
	..\libptmathematica\libptmathematica.lib

# Put the Ptolemy kernel directory in the include path
INCL = -I$(KERNDIR) $(PTCOMPAT_INCSPEC) \
	$(MATLAB_INCSPEC) -I$(PTMATLABLIB) \
	$(MATHEMATICA_INCSPEC) -I$(PTMATHEMATICALIB)

# Library sources.  CCS_WITH_H_FILES includes all .cc files with a
# corresponding .h file with the same root.
# CCS_WITH_H_FILES = InstanceManager.cc MathematicaIfc.cc MatlabIfc.cc
CCS_WITH_H_FILES = \
InstanceManager.cc MathematicaIfc.cc MatlabIfc.cc MatlabPtIfc.cc

# The complete list of .cc files used in the library
SRCS= $(CCS_WITH_H_FILES)

# Header files.  When adding a new header, add it here only if there is
# no corresponding .cc file; otherwise add the .cc file to CCS_WITH_H_FILES.
HDRS= $(CCS_WITH_H_FILES:.cc=.h) MatlabIfcFuns.h

# Library objects
OBJS= $(SRCS:.cc=.o)

all:	makefile $(PTLIB)

# "make install" puts things in the official place
install:	makefile $(LIBDIR)/$(PTLIB)

# matlab.mk and mathematica.mk check these vars before traversing the path
NEED_MATLABDIR = 	1
NEED_MATHEMATICADIR = 	1

# Automatically generated files
MATLABAUTO = MatlabHook.cc.Auto MatlabIfc.cc.Auto1 MatlabIfc.cc.Auto2 \
	     MatlabIfc.h.Auto
REALCLEAN_STUFF = $(MATLABAUTO)
EXTRA_SRCS = MatlabIfcGen.tcl MatlabIfcGen.dat
$(MATLABAUTO): MatlabIfcGen.tcl MatlabIfcGen.dat
	cd $(VPATH) && itclsh MatlabIfcGen.tcl

depend: $(MATLABAUTO)

# Include common make definitions
include $(ROOT)/mk/common.mk

# MatlabIfc.cc needs to know if Matlab libraries are shared, static,
# or non-existent.
ifneq ($(strip $(MATLABEXT_LIB)),)
GPPFLAGS += -DMATLABSTATIC
endif
ifneq ($(strip $(MATLABEXT2_LIB)),)
GPPFLAGS += -DMATLABSHARED
endif

# Don't add anything after the next line; makedepend will zap it.
# DO NOT DELETE THIS LINE -- make depend depends on it.

InstanceManager.o: InstanceManager.cc ../../../src/kernel/miscFuncs.h \
 ../../../src/kernel/logNew.h ../../../src/kernel/Error.h \
 ../../../src/kernel/StringList.h ../../../src/kernel/DataStruct.h \
 ../../../src/kernel/type.h InstanceManager.h
MathematicaIfc.o: MathematicaIfc.cc MathematicaIfc.h \
 ../../../src/kernel/StringList.h ../../../src/kernel/miscFuncs.h \
 ../../../src/kernel/logNew.h ../../../src/kernel/Error.h \
 ../../../src/kernel/DataStruct.h ../../../src/kernel/type.h \
 ../../../src/kernel/InfString.h \
 ../../../src/utils/libptmathematica/MathematicaIfcFuns.h \
 ../../../src/compat/ptolemy/compat.h
MatlabIfc.o: MatlabIfc.cc ../../../src/kernel/StringList.h \
 ../../../src/kernel/miscFuncs.h ../../../src/kernel/logNew.h \
 ../../../src/kernel/Error.h ../../../src/kernel/DataStruct.h \
 ../../../src/kernel/type.h ../../../src/kernel/InfString.h \
 MatlabIfc.h MatlabIfcFuns.h ../../../src/compat/ptolemy/compat.h \
 MatlabIfc.h.Auto ../../../src/kernel/Linker.h MatlabIfc.cc.Auto1 \
 MatlabIfc.cc.Auto2
MatlabPtIfc.o: MatlabPtIfc.cc MatlabPtIfc.h \
 ../../../src/kernel/Matrix.h ../../../src/kernel/Message.h \
 ../../../src/kernel/Particle.h ../../../src/kernel/StringList.h \
 ../../../src/kernel/miscFuncs.h ../../../src/kernel/logNew.h \
 ../../../src/kernel/Error.h ../../../src/kernel/DataStruct.h \
 ../../../src/kernel/type.h ../../../src/kernel/dataType.h \
 ../../../src/kernel/Fix.h ../../../src/kernel/ComplexSubset.h \
 ../../../src/kernel/isa.h ../../../src/kernel/ComplexArrayState.h \
 ../../../src/kernel/State.h ../../../src/kernel/InvokeInterp.h \
 ../../../src/kernel/NamedObj.h ../../../src/kernel/FlagArray.h \
 ../../../src/kernel/Attribute.h ../../../src/kernel/FixArrayState.h \
 ../../../src/kernel/PrecisionState.h \
 ../../../src/kernel/FloatArrayState.h \
 ../../../src/kernel/IntArrayState.h ../../../src/kernel/PortHole.h \
 MatlabIfc.h ../../../src/kernel/InfString.h MatlabIfcFuns.h \
 ../../../src/compat/ptolemy/compat.h MatlabIfc.h.Auto
