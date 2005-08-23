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
# Makefile for Ptolemy SDF matrix/stars
# @(#)make.template	1.29 20 Sep 1996
#
# If this file's name is "make.template" and there is no makefile in
# this directory, do
#	cp make.template makefile
#	chmod +w makefile
#	make depend
# This will add the dependencies to the makefile.

ROOT = ../../../../..
VPATH = $(ROOT)/src/domains/sdf/matrix/stars

# get configuration info
CONFIG=$(ROOT)/mk/config-$(PTARCH).mk
include $(CONFIG)

# SDFDIR is the directory containing SDF domain support
SDFDIR = $(ROOT)/src/domains/sdf/kernel
SDFSTARDIR = $(ROOT)/src/domains/sdf/stars
INCL = -I$(SDFSTARDIR) -I$(SDFDIR) -I$(KERNDIR) -I$(ROOT)/src/utils/libptdsp

# Library name
PTLIB=libsdfmatrixstars.$(LIBSUFFIX)

# source files for all stars, using preprocessor
PL_SRCS= \
  SDFAbs_M.pl \
  SDFAddCx_M.pl \
  SDFAddFix_M.pl \
  SDFAddInt_M.pl \
  SDFAdd_M.pl \
  SDFAvgSqrErr.pl \
  SDFConjugate_M.pl \
  SDFCxToFix_M.pl \
  SDFCxToFloat_M.pl \
  SDFCxToInt_M.pl \
  SDFFixToCx_M.pl \
  SDFFixToFloat_M.pl \
  SDFFixToInt_M.pl \
  SDFFloatToCx_M.pl \
  SDFFloatToFix_M.pl \
  SDFFloatToInt_M.pl \
  SDFGainCx_M.pl \
  SDFGainFix_M.pl \
  SDFGainInt_M.pl \
  SDFGain_M.pl \
  SDFHermitian_M.pl \
  SDFIdentityCx_M.pl \
  SDFIdentityFix_M.pl \
  SDFIdentityInt_M.pl \
  SDFIdentity_M.pl \
  SDFIntToCx_M.pl \
  SDFIntToFix_M.pl \
  SDFIntToFloat_M.pl \
  SDFInverseCx_M.pl \
  SDFInverseFix_M.pl \
  SDFInverseInt_M.pl \
  SDFInverse_M.pl \
  SDFMatrix.pl \
  SDFMatrixCx.pl \
  SDFMatrixFix.pl \
  SDFMatrixInt.pl \
  SDFMpyCx_M.pl \
  SDFMpyFix_M.pl \
  SDFMpyInt_M.pl \
  SDFMpy_M.pl \
  SDFMpyScalarCx_M.pl \
  SDFMpyScalarFix_M.pl \
  SDFMpyScalarInt_M.pl \
  SDFMpyScalar_M.pl \
  SDFMxCom_M.pl \
  SDFMxDecom_M.pl \
  SDFPackCx_M.pl \
  SDFPackFix_M.pl \
  SDFPackInt_M.pl \
  SDFPack_M.pl \
  SDFSampleMean.pl \
  SDFSmithForm.pl \
  SDFSubCx_M.pl \
  SDFSubFix_M.pl \
  SDFSubInt_M.pl \
  SDFSubMxCx_M.pl \
  SDFSubMxFix_M.pl \
  SDFSubMxInt_M.pl \
  SDFSubMx_M.pl \
  SDFSub_M.pl \
  SDFTableCx_M.pl \
  SDFTableInt_M.pl \
  SDFTable_M.pl \
  SDFToeplitzCx_M.pl \
  SDFToeplitzFix_M.pl \
  SDFToeplitzInt_M.pl \
  SDFToeplitz_M.pl \
  SDFTransposeCx_M.pl \
  SDFTransposeFix_M.pl \
  SDFTransposeInt_M.pl \
  SDFTranspose_M.pl \
  SDFUnPkCx_M.pl \
  SDFUnPkFix_M.pl \
  SDFUnPkInt_M.pl \
  SDFUnPk_M.pl

# The .cc files
SRCS=$(PL_SRCS:.pl=.cc)

# object files for all stars
OBJS= $(SRCS:.cc=.o)

# header files for all stars
HDRS= $(SRCS:.cc=.h)

STAR_MK=sdfmatrixstars
DOMAIN=SDF_MATRIX
DOMAIN_2=SDF
EXTRA_SRCS=$(STAR_MK).c $(PL_SRCS)

all:		$(SRCS) stars_all

install:	stars_install

# include common definitions, rules
include $(ROOT)/mk/common.mk


# Don't add anything after the next line; makedepend will zap it.
# DO NOT DELETE THIS LINE -- make depend depends on it.

SDFAbs_M.o: SDFAbs_M.cc SDFAbs_M.h \
 ../../../../../src/domains/sdf/kernel/SDFStar.h \
 ../../../../../src/kernel/DataFlowStar.h \
 ../../../../../src/kernel/Star.h ../../../../../src/kernel/Block.h \
 ../../../../../src/kernel/NamedObj.h \
 ../../../../../src/kernel/StringList.h \
 ../../../../../src/kernel/miscFuncs.h \
 ../../../../../src/kernel/logNew.h ../../../../../src/kernel/Error.h \
 ../../../../../src/kernel/DataStruct.h \
 ../../../../../src/kernel/type.h ../../../../../src/kernel/isa.h \
 ../../../../../src/kernel/FlagArray.h \
 ../../../../../src/kernel/PortHole.h \
 ../../../../../src/kernel/dataType.h \
 ../../../../../src/kernel/Particle.h ../../../../../src/kernel/Fix.h \
 ../../../../../src/kernel/ComplexSubset.h \
 ../../../../../src/kernel/Attribute.h \
 ../../../../../src/kernel/State.h \
 ../../../../../src/kernel/InvokeInterp.h \
 ../../../../../src/kernel/Fraction.h \
 ../../../../../src/domains/sdf/kernel/SDFPortHole.h \
 ../../../../../src/kernel/DFPortHole.h \
 ../../../../../src/kernel/IntState.h \
 ../../../../../src/kernel/Matrix.h \
 ../../../../../src/kernel/Message.h \
 ../../../../../src/kernel/ComplexArrayState.h \
 ../../../../../src/kernel/FixArrayState.h \
 ../../../../../src/kernel/PrecisionState.h \
 ../../../../../src/kernel/FloatArrayState.h \
 ../../../../../src/kernel/IntArrayState.h
SDFAddCx_M.o: SDFAddCx_M.cc SDFAddCx_M.h \
 ../../../../../src/domains/sdf/kernel/SDFStar.h \
 ../../../../../src/kernel/DataFlowStar.h \
 ../../../../../src/kernel/Star.h ../../../../../src/kernel/Block.h \
 ../../../../../src/kernel/NamedObj.h \
 ../../../../../src/kernel/StringList.h \
 ../../../../../src/kernel/miscFuncs.h \
 ../../../../../src/kernel/logNew.h ../../../../../src/kernel/Error.h \
 ../../../../../src/kernel/DataStruct.h \
 ../../../../../src/kernel/type.h ../../../../../src/kernel/isa.h \
 ../../../../../src/kernel/FlagArray.h \
 ../../../../../src/kernel/PortHole.h \
 ../../../../../src/kernel/dataType.h \
 ../../../../../src/kernel/Particle.h ../../../../../src/kernel/Fix.h \
 ../../../../../src/kernel/ComplexSubset.h \
 ../../../../../src/kernel/Attribute.h \
 ../../../../../src/kernel/State.h \
 ../../../../../src/kernel/InvokeInterp.h \
 ../../../../../src/kernel/Fraction.h \
 ../../../../../src/domains/sdf/kernel/SDFPortHole.h \
 ../../../../../src/kernel/DFPortHole.h \
 ../../../../../src/kernel/IntState.h \
 ../../../../../src/kernel/Matrix.h \
 ../../../../../src/kernel/Message.h \
 ../../../../../src/kernel/ComplexArrayState.h \
 ../../../../../src/kernel/FixArrayState.h \
 ../../../../../src/kernel/PrecisionState.h \
 ../../../../../src/kernel/FloatArrayState.h \
 ../../../../../src/kernel/IntArrayState.h
SDFAddFix_M.o: SDFAddFix_M.cc SDFAddFix_M.h \
 ../../../../../src/domains/sdf/kernel/SDFStar.h \
 ../../../../../src/kernel/DataFlowStar.h \
 ../../../../../src/kernel/Star.h ../../../../../src/kernel/Block.h \
 ../../../../../src/kernel/NamedObj.h \
 ../../../../../src/kernel/StringList.h \
 ../../../../../src/kernel/miscFuncs.h \
 ../../../../../src/kernel/logNew.h ../../../../../src/kernel/Error.h \
 ../../../../../src/kernel/DataStruct.h \
 ../../../../../src/kernel/type.h ../../../../../src/kernel/isa.h \
 ../../../../../src/kernel/FlagArray.h \
 ../../../../../src/kernel/PortHole.h \
 ../../../../../src/kernel/dataType.h \
 ../../../../../src/kernel/Particle.h ../../../../../src/kernel/Fix.h \
 ../../../../../src/kernel/ComplexSubset.h \
 ../../../../../src/kernel/Attribute.h \
 ../../../../../src/kernel/State.h \
 ../../../../../src/kernel/InvokeInterp.h \
 ../../../../../src/kernel/Fraction.h \
 ../../../../../src/domains/sdf/kernel/SDFPortHole.h \
 ../../../../../src/kernel/DFPortHole.h \
 ../../../../../src/kernel/IntState.h \
 ../../../../../src/kernel/StringState.h \
 ../../../../../src/kernel/KnownState.h \
 ../../../../../src/kernel/Matrix.h \
 ../../../../../src/kernel/Message.h \
 ../../../../../src/kernel/ComplexArrayState.h \
 ../../../../../src/kernel/FixArrayState.h \
 ../../../../../src/kernel/PrecisionState.h \
 ../../../../../src/kernel/FloatArrayState.h \
 ../../../../../src/kernel/IntArrayState.h
SDFAddInt_M.o: SDFAddInt_M.cc SDFAddInt_M.h \
 ../../../../../src/domains/sdf/kernel/SDFStar.h \
 ../../../../../src/kernel/DataFlowStar.h \
 ../../../../../src/kernel/Star.h ../../../../../src/kernel/Block.h \
 ../../../../../src/kernel/NamedObj.h \
 ../../../../../src/kernel/StringList.h \
 ../../../../../src/kernel/miscFuncs.h \
 ../../../../../src/kernel/logNew.h ../../../../../src/kernel/Error.h \
 ../../../../../src/kernel/DataStruct.h \
 ../../../../../src/kernel/type.h ../../../../../src/kernel/isa.h \
 ../../../../../src/kernel/FlagArray.h \
 ../../../../../src/kernel/PortHole.h \
 ../../../../../src/kernel/dataType.h \
 ../../../../../src/kernel/Particle.h ../../../../../src/kernel/Fix.h \
 ../../../../../src/kernel/ComplexSubset.h \
 ../../../../../src/kernel/Attribute.h \
 ../../../../../src/kernel/State.h \
 ../../../../../src/kernel/InvokeInterp.h \
 ../../../../../src/kernel/Fraction.h \
 ../../../../../src/domains/sdf/kernel/SDFPortHole.h \
 ../../../../../src/kernel/DFPortHole.h \
 ../../../../../src/kernel/IntState.h \
 ../../../../../src/kernel/Matrix.h \
 ../../../../../src/kernel/Message.h \
 ../../../../../src/kernel/ComplexArrayState.h \
 ../../../../../src/kernel/FixArrayState.h \
 ../../../../../src/kernel/PrecisionState.h \
 ../../../../../src/kernel/FloatArrayState.h \
 ../../../../../src/kernel/IntArrayState.h
SDFAdd_M.o: SDFAdd_M.cc SDFAdd_M.h \
 ../../../../../src/domains/sdf/kernel/SDFStar.h \
 ../../../../../src/kernel/DataFlowStar.h \
 ../../../../../src/kernel/Star.h ../../../../../src/kernel/Block.h \
 ../../../../../src/kernel/NamedObj.h \
 ../../../../../src/kernel/StringList.h \
 ../../../../../src/kernel/miscFuncs.h \
 ../../../../../src/kernel/logNew.h ../../../../../src/kernel/Error.h \
 ../../../../../src/kernel/DataStruct.h \
 ../../../../../src/kernel/type.h ../../../../../src/kernel/isa.h \
 ../../../../../src/kernel/FlagArray.h \
 ../../../../../src/kernel/PortHole.h \
 ../../../../../src/kernel/dataType.h \
 ../../../../../src/kernel/Particle.h ../../../../../src/kernel/Fix.h \
 ../../../../../src/kernel/ComplexSubset.h \
 ../../../../../src/kernel/Attribute.h \
 ../../../../../src/kernel/State.h \
 ../../../../../src/kernel/InvokeInterp.h \
 ../../../../../src/kernel/Fraction.h \
 ../../../../../src/domains/sdf/kernel/SDFPortHole.h \
 ../../../../../src/kernel/DFPortHole.h \
 ../../../../../src/kernel/IntState.h \
 ../../../../../src/kernel/Matrix.h \
 ../../../../../src/kernel/Message.h \
 ../../../../../src/kernel/ComplexArrayState.h \
 ../../../../../src/kernel/FixArrayState.h \
 ../../../../../src/kernel/PrecisionState.h \
 ../../../../../src/kernel/FloatArrayState.h \
 ../../../../../src/kernel/IntArrayState.h
SDFAvgSqrErr.o: SDFAvgSqrErr.cc SDFAvgSqrErr.h \
 ../../../../../src/domains/sdf/kernel/SDFStar.h \
 ../../../../../src/kernel/DataFlowStar.h \
 ../../../../../src/kernel/Star.h ../../../../../src/kernel/Block.h \
 ../../../../../src/kernel/NamedObj.h \
 ../../../../../src/kernel/StringList.h \
 ../../../../../src/kernel/miscFuncs.h \
 ../../../../../src/kernel/logNew.h ../../../../../src/kernel/Error.h \
 ../../../../../src/kernel/DataStruct.h \
 ../../../../../src/kernel/type.h ../../../../../src/kernel/isa.h \
 ../../../../../src/kernel/FlagArray.h \
 ../../../../../src/kernel/PortHole.h \
 ../../../../../src/kernel/dataType.h \
 ../../../../../src/kernel/Particle.h ../../../../../src/kernel/Fix.h \
 ../../../../../src/kernel/ComplexSubset.h \
 ../../../../../src/kernel/Attribute.h \
 ../../../../../src/kernel/State.h \
 ../../../../../src/kernel/InvokeInterp.h \
 ../../../../../src/kernel/Fraction.h \
 ../../../../../src/domains/sdf/kernel/SDFPortHole.h \
 ../../../../../src/kernel/DFPortHole.h \
 ../../../../../src/kernel/IntState.h \
 ../../../../../src/kernel/Matrix.h \
 ../../../../../src/kernel/Message.h \
 ../../../../../src/kernel/ComplexArrayState.h \
 ../../../../../src/kernel/FixArrayState.h \
 ../../../../../src/kernel/PrecisionState.h \
 ../../../../../src/kernel/FloatArrayState.h \
 ../../../../../src/kernel/IntArrayState.h
SDFConjugate_M.o: SDFConjugate_M.cc SDFConjugate_M.h \
 ../../../../../src/domains/sdf/kernel/SDFStar.h \
 ../../../../../src/kernel/DataFlowStar.h \
 ../../../../../src/kernel/Star.h ../../../../../src/kernel/Block.h \
 ../../../../../src/kernel/NamedObj.h \
 ../../../../../src/kernel/StringList.h \
 ../../../../../src/kernel/miscFuncs.h \
 ../../../../../src/kernel/logNew.h ../../../../../src/kernel/Error.h \
 ../../../../../src/kernel/DataStruct.h \
 ../../../../../src/kernel/type.h ../../../../../src/kernel/isa.h \
 ../../../../../src/kernel/FlagArray.h \
 ../../../../../src/kernel/PortHole.h \
 ../../../../../src/kernel/dataType.h \
 ../../../../../src/kernel/Particle.h ../../../../../src/kernel/Fix.h \
 ../../../../../src/kernel/ComplexSubset.h \
 ../../../../../src/kernel/Attribute.h \
 ../../../../../src/kernel/State.h \
 ../../../../../src/kernel/InvokeInterp.h \
 ../../../../../src/kernel/Fraction.h \
 ../../../../../src/domains/sdf/kernel/SDFPortHole.h \
 ../../../../../src/kernel/DFPortHole.h \
 ../../../../../src/kernel/IntState.h \
 ../../../../../src/kernel/Matrix.h \
 ../../../../../src/kernel/Message.h \
 ../../../../../src/kernel/ComplexArrayState.h \
 ../../../../../src/kernel/FixArrayState.h \
 ../../../../../src/kernel/PrecisionState.h \
 ../../../../../src/kernel/FloatArrayState.h \
 ../../../../../src/kernel/IntArrayState.h
SDFCxToFix_M.o: SDFCxToFix_M.cc SDFCxToFix_M.h \
 ../../../../../src/domains/sdf/kernel/SDFStar.h \
 ../../../../../src/kernel/DataFlowStar.h \
 ../../../../../src/kernel/Star.h ../../../../../src/kernel/Block.h \
 ../../../../../src/kernel/NamedObj.h \
 ../../../../../src/kernel/StringList.h \
 ../../../../../src/kernel/miscFuncs.h \
 ../../../../../src/kernel/logNew.h ../../../../../src/kernel/Error.h \
 ../../../../../src/kernel/DataStruct.h \
 ../../../../../src/kernel/type.h ../../../../../src/kernel/isa.h \
 ../../../../../src/kernel/FlagArray.h \
 ../../../../../src/kernel/PortHole.h \
 ../../../../../src/kernel/dataType.h \
 ../../../../../src/kernel/Particle.h ../../../../../src/kernel/Fix.h \
 ../../../../../src/kernel/ComplexSubset.h \
 ../../../../../src/kernel/Attribute.h \
 ../../../../../src/kernel/State.h \
 ../../../../../src/kernel/InvokeInterp.h \
 ../../../../../src/kernel/Fraction.h \
 ../../../../../src/domains/sdf/kernel/SDFPortHole.h \
 ../../../../../src/kernel/DFPortHole.h \
 ../../../../../src/kernel/StringState.h \
 ../../../../../src/kernel/KnownState.h \
 ../../../../../src/kernel/Matrix.h \
 ../../../../../src/kernel/Message.h \
 ../../../../../src/kernel/ComplexArrayState.h \
 ../../../../../src/kernel/FixArrayState.h \
 ../../../../../src/kernel/PrecisionState.h \
 ../../../../../src/kernel/FloatArrayState.h \
 ../../../../../src/kernel/IntArrayState.h
SDFCxToFloat_M.o: SDFCxToFloat_M.cc SDFCxToFloat_M.h \
 ../../../../../src/domains/sdf/kernel/SDFStar.h \
 ../../../../../src/kernel/DataFlowStar.h \
 ../../../../../src/kernel/Star.h ../../../../../src/kernel/Block.h \
 ../../../../../src/kernel/NamedObj.h \
 ../../../../../src/kernel/StringList.h \
 ../../../../../src/kernel/miscFuncs.h \
 ../../../../../src/kernel/logNew.h ../../../../../src/kernel/Error.h \
 ../../../../../src/kernel/DataStruct.h \
 ../../../../../src/kernel/type.h ../../../../../src/kernel/isa.h \
 ../../../../../src/kernel/FlagArray.h \
 ../../../../../src/kernel/PortHole.h \
 ../../../../../src/kernel/dataType.h \
 ../../../../../src/kernel/Particle.h ../../../../../src/kernel/Fix.h \
 ../../../../../src/kernel/ComplexSubset.h \
 ../../../../../src/kernel/Attribute.h \
 ../../../../../src/kernel/State.h \
 ../../../../../src/kernel/InvokeInterp.h \
 ../../../../../src/kernel/Fraction.h \
 ../../../../../src/domains/sdf/kernel/SDFPortHole.h \
 ../../../../../src/kernel/DFPortHole.h \
 ../../../../../src/kernel/Matrix.h \
 ../../../../../src/kernel/Message.h \
 ../../../../../src/kernel/ComplexArrayState.h \
 ../../../../../src/kernel/FixArrayState.h \
 ../../../../../src/kernel/PrecisionState.h \
 ../../../../../src/kernel/FloatArrayState.h \
 ../../../../../src/kernel/IntArrayState.h
SDFCxToInt_M.o: SDFCxToInt_M.cc SDFCxToInt_M.h \
 ../../../../../src/domains/sdf/kernel/SDFStar.h \
 ../../../../../src/kernel/DataFlowStar.h \
 ../../../../../src/kernel/Star.h ../../../../../src/kernel/Block.h \
 ../../../../../src/kernel/NamedObj.h \
 ../../../../../src/kernel/StringList.h \
 ../../../../../src/kernel/miscFuncs.h \
 ../../../../../src/kernel/logNew.h ../../../../../src/kernel/Error.h \
 ../../../../../src/kernel/DataStruct.h \
 ../../../../../src/kernel/type.h ../../../../../src/kernel/isa.h \
 ../../../../../src/kernel/FlagArray.h \
 ../../../../../src/kernel/PortHole.h \
 ../../../../../src/kernel/dataType.h \
 ../../../../../src/kernel/Particle.h ../../../../../src/kernel/Fix.h \
 ../../../../../src/kernel/ComplexSubset.h \
 ../../../../../src/kernel/Attribute.h \
 ../../../../../src/kernel/State.h \
 ../../../../../src/kernel/InvokeInterp.h \
 ../../../../../src/kernel/Fraction.h \
 ../../../../../src/domains/sdf/kernel/SDFPortHole.h \
 ../../../../../src/kernel/DFPortHole.h \
 ../../../../../src/kernel/Matrix.h \
 ../../../../../src/kernel/Message.h \
 ../../../../../src/kernel/ComplexArrayState.h \
 ../../../../../src/kernel/FixArrayState.h \
 ../../../../../src/kernel/PrecisionState.h \
 ../../../../../src/kernel/FloatArrayState.h \
 ../../../../../src/kernel/IntArrayState.h
SDFFixToCx_M.o: SDFFixToCx_M.cc SDFFixToCx_M.h \
 ../../../../../src/domains/sdf/kernel/SDFStar.h \
 ../../../../../src/kernel/DataFlowStar.h \
 ../../../../../src/kernel/Star.h ../../../../../src/kernel/Block.h \
 ../../../../../src/kernel/NamedObj.h \
 ../../../../../src/kernel/StringList.h \
 ../../../../../src/kernel/miscFuncs.h \
 ../../../../../src/kernel/logNew.h ../../../../../src/kernel/Error.h \
 ../../../../../src/kernel/DataStruct.h \
 ../../../../../src/kernel/type.h ../../../../../src/kernel/isa.h \
 ../../../../../src/kernel/FlagArray.h \
 ../../../../../src/kernel/PortHole.h \
 ../../../../../src/kernel/dataType.h \
 ../../../../../src/kernel/Particle.h ../../../../../src/kernel/Fix.h \
 ../../../../../src/kernel/ComplexSubset.h \
 ../../../../../src/kernel/Attribute.h \
 ../../../../../src/kernel/State.h \
 ../../../../../src/kernel/InvokeInterp.h \
 ../../../../../src/kernel/Fraction.h \
 ../../../../../src/domains/sdf/kernel/SDFPortHole.h \
 ../../../../../src/kernel/DFPortHole.h \
 ../../../../../src/kernel/Matrix.h \
 ../../../../../src/kernel/Message.h \
 ../../../../../src/kernel/ComplexArrayState.h \
 ../../../../../src/kernel/FixArrayState.h \
 ../../../../../src/kernel/PrecisionState.h \
 ../../../../../src/kernel/FloatArrayState.h \
 ../../../../../src/kernel/IntArrayState.h
SDFFixToFloat_M.o: SDFFixToFloat_M.cc SDFFixToFloat_M.h \
 ../../../../../src/domains/sdf/kernel/SDFStar.h \
 ../../../../../src/kernel/DataFlowStar.h \
 ../../../../../src/kernel/Star.h ../../../../../src/kernel/Block.h \
 ../../../../../src/kernel/NamedObj.h \
 ../../../../../src/kernel/StringList.h \
 ../../../../../src/kernel/miscFuncs.h \
 ../../../../../src/kernel/logNew.h ../../../../../src/kernel/Error.h \
 ../../../../../src/kernel/DataStruct.h \
 ../../../../../src/kernel/type.h ../../../../../src/kernel/isa.h \
 ../../../../../src/kernel/FlagArray.h \
 ../../../../../src/kernel/PortHole.h \
 ../../../../../src/kernel/dataType.h \
 ../../../../../src/kernel/Particle.h ../../../../../src/kernel/Fix.h \
 ../../../../../src/kernel/ComplexSubset.h \
 ../../../../../src/kernel/Attribute.h \
 ../../../../../src/kernel/State.h \
 ../../../../../src/kernel/InvokeInterp.h \
 ../../../../../src/kernel/Fraction.h \
 ../../../../../src/domains/sdf/kernel/SDFPortHole.h \
 ../../../../../src/kernel/DFPortHole.h \
 ../../../../../src/kernel/Matrix.h \
 ../../../../../src/kernel/Message.h \
 ../../../../../src/kernel/ComplexArrayState.h \
 ../../../../../src/kernel/FixArrayState.h \
 ../../../../../src/kernel/PrecisionState.h \
 ../../../../../src/kernel/FloatArrayState.h \
 ../../../../../src/kernel/IntArrayState.h
SDFFixToInt_M.o: SDFFixToInt_M.cc SDFFixToInt_M.h \
 ../../../../../src/domains/sdf/kernel/SDFStar.h \
 ../../../../../src/kernel/DataFlowStar.h \
 ../../../../../src/kernel/Star.h ../../../../../src/kernel/Block.h \
 ../../../../../src/kernel/NamedObj.h \
 ../../../../../src/kernel/StringList.h \
 ../../../../../src/kernel/miscFuncs.h \
 ../../../../../src/kernel/logNew.h ../../../../../src/kernel/Error.h \
 ../../../../../src/kernel/DataStruct.h \
 ../../../../../src/kernel/type.h ../../../../../src/kernel/isa.h \
 ../../../../../src/kernel/FlagArray.h \
 ../../../../../src/kernel/PortHole.h \
 ../../../../../src/kernel/dataType.h \
 ../../../../../src/kernel/Particle.h ../../../../../src/kernel/Fix.h \
 ../../../../../src/kernel/ComplexSubset.h \
 ../../../../../src/kernel/Attribute.h \
 ../../../../../src/kernel/State.h \
 ../../../../../src/kernel/InvokeInterp.h \
 ../../../../../src/kernel/Fraction.h \
 ../../../../../src/domains/sdf/kernel/SDFPortHole.h \
 ../../../../../src/kernel/DFPortHole.h \
 ../../../../../src/kernel/Matrix.h \
 ../../../../../src/kernel/Message.h \
 ../../../../../src/kernel/ComplexArrayState.h \
 ../../../../../src/kernel/FixArrayState.h \
 ../../../../../src/kernel/PrecisionState.h \
 ../../../../../src/kernel/FloatArrayState.h \
 ../../../../../src/kernel/IntArrayState.h
SDFFloatToCx_M.o: SDFFloatToCx_M.cc SDFFloatToCx_M.h \
 ../../../../../src/domains/sdf/kernel/SDFStar.h \
 ../../../../../src/kernel/DataFlowStar.h \
 ../../../../../src/kernel/Star.h ../../../../../src/kernel/Block.h \
 ../../../../../src/kernel/NamedObj.h \
 ../../../../../src/kernel/StringList.h \
 ../../../../../src/kernel/miscFuncs.h \
 ../../../../../src/kernel/logNew.h ../../../../../src/kernel/Error.h \
 ../../../../../src/kernel/DataStruct.h \
 ../../../../../src/kernel/type.h ../../../../../src/kernel/isa.h \
 ../../../../../src/kernel/FlagArray.h \
 ../../../../../src/kernel/PortHole.h \
 ../../../../../src/kernel/dataType.h \
 ../../../../../src/kernel/Particle.h ../../../../../src/kernel/Fix.h \
 ../../../../../src/kernel/ComplexSubset.h \
 ../../../../../src/kernel/Attribute.h \
 ../../../../../src/kernel/State.h \
 ../../../../../src/kernel/InvokeInterp.h \
 ../../../../../src/kernel/Fraction.h \
 ../../../../../src/domains/sdf/kernel/SDFPortHole.h \
 ../../../../../src/kernel/DFPortHole.h \
 ../../../../../src/kernel/Matrix.h \
 ../../../../../src/kernel/Message.h \
 ../../../../../src/kernel/ComplexArrayState.h \
 ../../../../../src/kernel/FixArrayState.h \
 ../../../../../src/kernel/PrecisionState.h \
 ../../../../../src/kernel/FloatArrayState.h \
 ../../../../../src/kernel/IntArrayState.h
SDFFloatToFix_M.o: SDFFloatToFix_M.cc SDFFloatToFix_M.h \
 ../../../../../src/domains/sdf/kernel/SDFStar.h \
 ../../../../../src/kernel/DataFlowStar.h \
 ../../../../../src/kernel/Star.h ../../../../../src/kernel/Block.h \
 ../../../../../src/kernel/NamedObj.h \
 ../../../../../src/kernel/StringList.h \
 ../../../../../src/kernel/miscFuncs.h \
 ../../../../../src/kernel/logNew.h ../../../../../src/kernel/Error.h \
 ../../../../../src/kernel/DataStruct.h \
 ../../../../../src/kernel/type.h ../../../../../src/kernel/isa.h \
 ../../../../../src/kernel/FlagArray.h \
 ../../../../../src/kernel/PortHole.h \
 ../../../../../src/kernel/dataType.h \
 ../../../../../src/kernel/Particle.h ../../../../../src/kernel/Fix.h \
 ../../../../../src/kernel/ComplexSubset.h \
 ../../../../../src/kernel/Attribute.h \
 ../../../../../src/kernel/State.h \
 ../../../../../src/kernel/InvokeInterp.h \
 ../../../../../src/kernel/Fraction.h \
 ../../../../../src/domains/sdf/kernel/SDFPortHole.h \
 ../../../../../src/kernel/DFPortHole.h \
 ../../../../../src/kernel/StringState.h \
 ../../../../../src/kernel/KnownState.h \
 ../../../../../src/kernel/Matrix.h \
 ../../../../../src/kernel/Message.h \
 ../../../../../src/kernel/ComplexArrayState.h \
 ../../../../../src/kernel/FixArrayState.h \
 ../../../../../src/kernel/PrecisionState.h \
 ../../../../../src/kernel/FloatArrayState.h \
 ../../../../../src/kernel/IntArrayState.h
SDFFloatToInt_M.o: SDFFloatToInt_M.cc SDFFloatToInt_M.h \
 ../../../../../src/domains/sdf/kernel/SDFStar.h \
 ../../../../../src/kernel/DataFlowStar.h \
 ../../../../../src/kernel/Star.h ../../../../../src/kernel/Block.h \
 ../../../../../src/kernel/NamedObj.h \
 ../../../../../src/kernel/StringList.h \
 ../../../../../src/kernel/miscFuncs.h \
 ../../../../../src/kernel/logNew.h ../../../../../src/kernel/Error.h \
 ../../../../../src/kernel/DataStruct.h \
 ../../../../../src/kernel/type.h ../../../../../src/kernel/isa.h \
 ../../../../../src/kernel/FlagArray.h \
 ../../../../../src/kernel/PortHole.h \
 ../../../../../src/kernel/dataType.h \
 ../../../../../src/kernel/Particle.h ../../../../../src/kernel/Fix.h \
 ../../../../../src/kernel/ComplexSubset.h \
 ../../../../../src/kernel/Attribute.h \
 ../../../../../src/kernel/State.h \
 ../../../../../src/kernel/InvokeInterp.h \
 ../../../../../src/kernel/Fraction.h \
 ../../../../../src/domains/sdf/kernel/SDFPortHole.h \
 ../../../../../src/kernel/DFPortHole.h \
 ../../../../../src/kernel/Matrix.h \
 ../../../../../src/kernel/Message.h \
 ../../../../../src/kernel/ComplexArrayState.h \
 ../../../../../src/kernel/FixArrayState.h \
 ../../../../../src/kernel/PrecisionState.h \
 ../../../../../src/kernel/FloatArrayState.h \
 ../../../../../src/kernel/IntArrayState.h
SDFGainCx_M.o: SDFGainCx_M.cc SDFGainCx_M.h \
 ../../../../../src/domains/sdf/kernel/SDFStar.h \
 ../../../../../src/kernel/DataFlowStar.h \
 ../../../../../src/kernel/Star.h ../../../../../src/kernel/Block.h \
 ../../../../../src/kernel/NamedObj.h \
 ../../../../../src/kernel/StringList.h \
 ../../../../../src/kernel/miscFuncs.h \
 ../../../../../src/kernel/logNew.h ../../../../../src/kernel/Error.h \
 ../../../../../src/kernel/DataStruct.h \
 ../../../../../src/kernel/type.h ../../../../../src/kernel/isa.h \
 ../../../../../src/kernel/FlagArray.h \
 ../../../../../src/kernel/PortHole.h \
 ../../../../../src/kernel/dataType.h \
 ../../../../../src/kernel/Particle.h ../../../../../src/kernel/Fix.h \
 ../../../../../src/kernel/ComplexSubset.h \
 ../../../../../src/kernel/Attribute.h \
 ../../../../../src/kernel/State.h \
 ../../../../../src/kernel/InvokeInterp.h \
 ../../../../../src/kernel/Fraction.h \
 ../../../../../src/domains/sdf/kernel/SDFPortHole.h \
 ../../../../../src/kernel/DFPortHole.h \
 ../../../../../src/kernel/ComplexState.h \
 ../../../../../src/kernel/Matrix.h \
 ../../../../../src/kernel/Message.h \
 ../../../../../src/kernel/ComplexArrayState.h \
 ../../../../../src/kernel/FixArrayState.h \
 ../../../../../src/kernel/PrecisionState.h \
 ../../../../../src/kernel/FloatArrayState.h \
 ../../../../../src/kernel/IntArrayState.h
SDFGainFix_M.o: SDFGainFix_M.cc SDFGainFix_M.h \
 ../../../../../src/domains/sdf/kernel/SDFStar.h \
 ../../../../../src/kernel/DataFlowStar.h \
 ../../../../../src/kernel/Star.h ../../../../../src/kernel/Block.h \
 ../../../../../src/kernel/NamedObj.h \
 ../../../../../src/kernel/StringList.h \
 ../../../../../src/kernel/miscFuncs.h \
 ../../../../../src/kernel/logNew.h ../../../../../src/kernel/Error.h \
 ../../../../../src/kernel/DataStruct.h \
 ../../../../../src/kernel/type.h ../../../../../src/kernel/isa.h \
 ../../../../../src/kernel/FlagArray.h \
 ../../../../../src/kernel/PortHole.h \
 ../../../../../src/kernel/dataType.h \
 ../../../../../src/kernel/Particle.h ../../../../../src/kernel/Fix.h \
 ../../../../../src/kernel/ComplexSubset.h \
 ../../../../../src/kernel/Attribute.h \
 ../../../../../src/kernel/State.h \
 ../../../../../src/kernel/InvokeInterp.h \
 ../../../../../src/kernel/Fraction.h \
 ../../../../../src/domains/sdf/kernel/SDFPortHole.h \
 ../../../../../src/kernel/DFPortHole.h \
 ../../../../../src/kernel/IntState.h \
 ../../../../../src/kernel/StringState.h \
 ../../../../../src/kernel/KnownState.h \
 ../../../../../src/kernel/FixState.h \
 ../../../../../src/kernel/PrecisionState.h \
 ../../../../../src/kernel/Matrix.h \
 ../../../../../src/kernel/Message.h \
 ../../../../../src/kernel/ComplexArrayState.h \
 ../../../../../src/kernel/FixArrayState.h \
 ../../../../../src/kernel/FloatArrayState.h \
 ../../../../../src/kernel/IntArrayState.h
SDFGainInt_M.o: SDFGainInt_M.cc SDFGainInt_M.h \
 ../../../../../src/domains/sdf/kernel/SDFStar.h \
 ../../../../../src/kernel/DataFlowStar.h \
 ../../../../../src/kernel/Star.h ../../../../../src/kernel/Block.h \
 ../../../../../src/kernel/NamedObj.h \
 ../../../../../src/kernel/StringList.h \
 ../../../../../src/kernel/miscFuncs.h \
 ../../../../../src/kernel/logNew.h ../../../../../src/kernel/Error.h \
 ../../../../../src/kernel/DataStruct.h \
 ../../../../../src/kernel/type.h ../../../../../src/kernel/isa.h \
 ../../../../../src/kernel/FlagArray.h \
 ../../../../../src/kernel/PortHole.h \
 ../../../../../src/kernel/dataType.h \
 ../../../../../src/kernel/Particle.h ../../../../../src/kernel/Fix.h \
 ../../../../../src/kernel/ComplexSubset.h \
 ../../../../../src/kernel/Attribute.h \
 ../../../../../src/kernel/State.h \
 ../../../../../src/kernel/InvokeInterp.h \
 ../../../../../src/kernel/Fraction.h \
 ../../../../../src/domains/sdf/kernel/SDFPortHole.h \
 ../../../../../src/kernel/DFPortHole.h \
 ../../../../../src/kernel/IntState.h \
 ../../../../../src/kernel/Matrix.h \
 ../../../../../src/kernel/Message.h \
 ../../../../../src/kernel/ComplexArrayState.h \
 ../../../../../src/kernel/FixArrayState.h \
 ../../../../../src/kernel/PrecisionState.h \
 ../../../../../src/kernel/FloatArrayState.h \
 ../../../../../src/kernel/IntArrayState.h
SDFGain_M.o: SDFGain_M.cc SDFGain_M.h \
 ../../../../../src/domains/sdf/kernel/SDFStar.h \
 ../../../../../src/kernel/DataFlowStar.h \
 ../../../../../src/kernel/Star.h ../../../../../src/kernel/Block.h \
 ../../../../../src/kernel/NamedObj.h \
 ../../../../../src/kernel/StringList.h \
 ../../../../../src/kernel/miscFuncs.h \
 ../../../../../src/kernel/logNew.h ../../../../../src/kernel/Error.h \
 ../../../../../src/kernel/DataStruct.h \
 ../../../../../src/kernel/type.h ../../../../../src/kernel/isa.h \
 ../../../../../src/kernel/FlagArray.h \
 ../../../../../src/kernel/PortHole.h \
 ../../../../../src/kernel/dataType.h \
 ../../../../../src/kernel/Particle.h ../../../../../src/kernel/Fix.h \
 ../../../../../src/kernel/ComplexSubset.h \
 ../../../../../src/kernel/Attribute.h \
 ../../../../../src/kernel/State.h \
 ../../../../../src/kernel/InvokeInterp.h \
 ../../../../../src/kernel/Fraction.h \
 ../../../../../src/domains/sdf/kernel/SDFPortHole.h \
 ../../../../../src/kernel/DFPortHole.h \
 ../../../../../src/kernel/FloatState.h \
 ../../../../../src/kernel/Matrix.h \
 ../../../../../src/kernel/Message.h \
 ../../../../../src/kernel/ComplexArrayState.h \
 ../../../../../src/kernel/FixArrayState.h \
 ../../../../../src/kernel/PrecisionState.h \
 ../../../../../src/kernel/FloatArrayState.h \
 ../../../../../src/kernel/IntArrayState.h
SDFHermitian_M.o: SDFHermitian_M.cc SDFHermitian_M.h \
 ../../../../../src/domains/sdf/kernel/SDFStar.h \
 ../../../../../src/kernel/DataFlowStar.h \
 ../../../../../src/kernel/Star.h ../../../../../src/kernel/Block.h \
 ../../../../../src/kernel/NamedObj.h \
 ../../../../../src/kernel/StringList.h \
 ../../../../../src/kernel/miscFuncs.h \
 ../../../../../src/kernel/logNew.h ../../../../../src/kernel/Error.h \
 ../../../../../src/kernel/DataStruct.h \
 ../../../../../src/kernel/type.h ../../../../../src/kernel/isa.h \
 ../../../../../src/kernel/FlagArray.h \
 ../../../../../src/kernel/PortHole.h \
 ../../../../../src/kernel/dataType.h \
 ../../../../../src/kernel/Particle.h ../../../../../src/kernel/Fix.h \
 ../../../../../src/kernel/ComplexSubset.h \
 ../../../../../src/kernel/Attribute.h \
 ../../../../../src/kernel/State.h \
 ../../../../../src/kernel/InvokeInterp.h \
 ../../../../../src/kernel/Fraction.h \
 ../../../../../src/domains/sdf/kernel/SDFPortHole.h \
 ../../../../../src/kernel/DFPortHole.h \
 ../../../../../src/kernel/IntState.h \
 ../../../../../src/kernel/Matrix.h \
 ../../../../../src/kernel/Message.h \
 ../../../../../src/kernel/ComplexArrayState.h \
 ../../../../../src/kernel/FixArrayState.h \
 ../../../../../src/kernel/PrecisionState.h \
 ../../../../../src/kernel/FloatArrayState.h \
 ../../../../../src/kernel/IntArrayState.h
SDFIdentityCx_M.o: SDFIdentityCx_M.cc SDFIdentityCx_M.h \
 ../../../../../src/domains/sdf/kernel/SDFStar.h \
 ../../../../../src/kernel/DataFlowStar.h \
 ../../../../../src/kernel/Star.h ../../../../../src/kernel/Block.h \
 ../../../../../src/kernel/NamedObj.h \
 ../../../../../src/kernel/StringList.h \
 ../../../../../src/kernel/miscFuncs.h \
 ../../../../../src/kernel/logNew.h ../../../../../src/kernel/Error.h \
 ../../../../../src/kernel/DataStruct.h \
 ../../../../../src/kernel/type.h ../../../../../src/kernel/isa.h \
 ../../../../../src/kernel/FlagArray.h \
 ../../../../../src/kernel/PortHole.h \
 ../../../../../src/kernel/dataType.h \
 ../../../../../src/kernel/Particle.h ../../../../../src/kernel/Fix.h \
 ../../../../../src/kernel/ComplexSubset.h \
 ../../../../../src/kernel/Attribute.h \
 ../../../../../src/kernel/State.h \
 ../../../../../src/kernel/InvokeInterp.h \
 ../../../../../src/kernel/Fraction.h \
 ../../../../../src/domains/sdf/kernel/SDFPortHole.h \
 ../../../../../src/kernel/DFPortHole.h \
 ../../../../../src/kernel/IntState.h \
 ../../../../../src/kernel/Matrix.h \
 ../../../../../src/kernel/Message.h \
 ../../../../../src/kernel/ComplexArrayState.h \
 ../../../../../src/kernel/FixArrayState.h \
 ../../../../../src/kernel/PrecisionState.h \
 ../../../../../src/kernel/FloatArrayState.h \
 ../../../../../src/kernel/IntArrayState.h
SDFIdentityFix_M.o: SDFIdentityFix_M.cc SDFIdentityFix_M.h \
 ../../../../../src/domains/sdf/kernel/SDFStar.h \
 ../../../../../src/kernel/DataFlowStar.h \
 ../../../../../src/kernel/Star.h ../../../../../src/kernel/Block.h \
 ../../../../../src/kernel/NamedObj.h \
 ../../../../../src/kernel/StringList.h \
 ../../../../../src/kernel/miscFuncs.h \
 ../../../../../src/kernel/logNew.h ../../../../../src/kernel/Error.h \
 ../../../../../src/kernel/DataStruct.h \
 ../../../../../src/kernel/type.h ../../../../../src/kernel/isa.h \
 ../../../../../src/kernel/FlagArray.h \
 ../../../../../src/kernel/PortHole.h \
 ../../../../../src/kernel/dataType.h \
 ../../../../../src/kernel/Particle.h ../../../../../src/kernel/Fix.h \
 ../../../../../src/kernel/ComplexSubset.h \
 ../../../../../src/kernel/Attribute.h \
 ../../../../../src/kernel/State.h \
 ../../../../../src/kernel/InvokeInterp.h \
 ../../../../../src/kernel/Fraction.h \
 ../../../../../src/domains/sdf/kernel/SDFPortHole.h \
 ../../../../../src/kernel/DFPortHole.h \
 ../../../../../src/kernel/IntState.h \
 ../../../../../src/kernel/StringState.h \
 ../../../../../src/kernel/KnownState.h \
 ../../../../../src/kernel/Matrix.h \
 ../../../../../src/kernel/Message.h \
 ../../../../../src/kernel/ComplexArrayState.h \
 ../../../../../src/kernel/FixArrayState.h \
 ../../../../../src/kernel/PrecisionState.h \
 ../../../../../src/kernel/FloatArrayState.h \
 ../../../../../src/kernel/IntArrayState.h
SDFIdentityInt_M.o: SDFIdentityInt_M.cc SDFIdentityInt_M.h \
 ../../../../../src/domains/sdf/kernel/SDFStar.h \
 ../../../../../src/kernel/DataFlowStar.h \
 ../../../../../src/kernel/Star.h ../../../../../src/kernel/Block.h \
 ../../../../../src/kernel/NamedObj.h \
 ../../../../../src/kernel/StringList.h \
 ../../../../../src/kernel/miscFuncs.h \
 ../../../../../src/kernel/logNew.h ../../../../../src/kernel/Error.h \
 ../../../../../src/kernel/DataStruct.h \
 ../../../../../src/kernel/type.h ../../../../../src/kernel/isa.h \
 ../../../../../src/kernel/FlagArray.h \
 ../../../../../src/kernel/PortHole.h \
 ../../../../../src/kernel/dataType.h \
 ../../../../../src/kernel/Particle.h ../../../../../src/kernel/Fix.h \
 ../../../../../src/kernel/ComplexSubset.h \
 ../../../../../src/kernel/Attribute.h \
 ../../../../../src/kernel/State.h \
 ../../../../../src/kernel/InvokeInterp.h \
 ../../../../../src/kernel/Fraction.h \
 ../../../../../src/domains/sdf/kernel/SDFPortHole.h \
 ../../../../../src/kernel/DFPortHole.h \
 ../../../../../src/kernel/IntState.h \
 ../../../../../src/kernel/Matrix.h \
 ../../../../../src/kernel/Message.h \
 ../../../../../src/kernel/ComplexArrayState.h \
 ../../../../../src/kernel/FixArrayState.h \
 ../../../../../src/kernel/PrecisionState.h \
 ../../../../../src/kernel/FloatArrayState.h \
 ../../../../../src/kernel/IntArrayState.h
SDFIdentity_M.o: SDFIdentity_M.cc SDFIdentity_M.h \
 ../../../../../src/domains/sdf/kernel/SDFStar.h \
 ../../../../../src/kernel/DataFlowStar.h \
 ../../../../../src/kernel/Star.h ../../../../../src/kernel/Block.h \
 ../../../../../src/kernel/NamedObj.h \
 ../../../../../src/kernel/StringList.h \
 ../../../../../src/kernel/miscFuncs.h \
 ../../../../../src/kernel/logNew.h ../../../../../src/kernel/Error.h \
 ../../../../../src/kernel/DataStruct.h \
 ../../../../../src/kernel/type.h ../../../../../src/kernel/isa.h \
 ../../../../../src/kernel/FlagArray.h \
 ../../../../../src/kernel/PortHole.h \
 ../../../../../src/kernel/dataType.h \
 ../../../../../src/kernel/Particle.h ../../../../../src/kernel/Fix.h \
 ../../../../../src/kernel/ComplexSubset.h \
 ../../../../../src/kernel/Attribute.h \
 ../../../../../src/kernel/State.h \
 ../../../../../src/kernel/InvokeInterp.h \
 ../../../../../src/kernel/Fraction.h \
 ../../../../../src/domains/sdf/kernel/SDFPortHole.h \
 ../../../../../src/kernel/DFPortHole.h \
 ../../../../../src/kernel/IntState.h \
 ../../../../../src/kernel/Matrix.h \
 ../../../../../src/kernel/Message.h \
 ../../../../../src/kernel/ComplexArrayState.h \
 ../../../../../src/kernel/FixArrayState.h \
 ../../../../../src/kernel/PrecisionState.h \
 ../../../../../src/kernel/FloatArrayState.h \
 ../../../../../src/kernel/IntArrayState.h
SDFIntToCx_M.o: SDFIntToCx_M.cc SDFIntToCx_M.h \
 ../../../../../src/domains/sdf/kernel/SDFStar.h \
 ../../../../../src/kernel/DataFlowStar.h \
 ../../../../../src/kernel/Star.h ../../../../../src/kernel/Block.h \
 ../../../../../src/kernel/NamedObj.h \
 ../../../../../src/kernel/StringList.h \
 ../../../../../src/kernel/miscFuncs.h \
 ../../../../../src/kernel/logNew.h ../../../../../src/kernel/Error.h \
 ../../../../../src/kernel/DataStruct.h \
 ../../../../../src/kernel/type.h ../../../../../src/kernel/isa.h \
 ../../../../../src/kernel/FlagArray.h \
 ../../../../../src/kernel/PortHole.h \
 ../../../../../src/kernel/dataType.h \
 ../../../../../src/kernel/Particle.h ../../../../../src/kernel/Fix.h \
 ../../../../../src/kernel/ComplexSubset.h \
 ../../../../../src/kernel/Attribute.h \
 ../../../../../src/kernel/State.h \
 ../../../../../src/kernel/InvokeInterp.h \
 ../../../../../src/kernel/Fraction.h \
 ../../../../../src/domains/sdf/kernel/SDFPortHole.h \
 ../../../../../src/kernel/DFPortHole.h \
 ../../../../../src/kernel/Matrix.h \
 ../../../../../src/kernel/Message.h \
 ../../../../../src/kernel/ComplexArrayState.h \
 ../../../../../src/kernel/FixArrayState.h \
 ../../../../../src/kernel/PrecisionState.h \
 ../../../../../src/kernel/FloatArrayState.h \
 ../../../../../src/kernel/IntArrayState.h
SDFIntToFix_M.o: SDFIntToFix_M.cc SDFIntToFix_M.h \
 ../../../../../src/domains/sdf/kernel/SDFStar.h \
 ../../../../../src/kernel/DataFlowStar.h \
 ../../../../../src/kernel/Star.h ../../../../../src/kernel/Block.h \
 ../../../../../src/kernel/NamedObj.h \
 ../../../../../src/kernel/StringList.h \
 ../../../../../src/kernel/miscFuncs.h \
 ../../../../../src/kernel/logNew.h ../../../../../src/kernel/Error.h \
 ../../../../../src/kernel/DataStruct.h \
 ../../../../../src/kernel/type.h ../../../../../src/kernel/isa.h \
 ../../../../../src/kernel/FlagArray.h \
 ../../../../../src/kernel/PortHole.h \
 ../../../../../src/kernel/dataType.h \
 ../../../../../src/kernel/Particle.h ../../../../../src/kernel/Fix.h \
 ../../../../../src/kernel/ComplexSubset.h \
 ../../../../../src/kernel/Attribute.h \
 ../../../../../src/kernel/State.h \
 ../../../../../src/kernel/InvokeInterp.h \
 ../../../../../src/kernel/Fraction.h \
 ../../../../../src/domains/sdf/kernel/SDFPortHole.h \
 ../../../../../src/kernel/DFPortHole.h \
 ../../../../../src/kernel/StringState.h \
 ../../../../../src/kernel/KnownState.h \
 ../../../../../src/kernel/Matrix.h \
 ../../../../../src/kernel/Message.h \
 ../../../../../src/kernel/ComplexArrayState.h \
 ../../../../../src/kernel/FixArrayState.h \
 ../../../../../src/kernel/PrecisionState.h \
 ../../../../../src/kernel/FloatArrayState.h \
 ../../../../../src/kernel/IntArrayState.h
SDFIntToFloat_M.o: SDFIntToFloat_M.cc SDFIntToFloat_M.h \
 ../../../../../src/domains/sdf/kernel/SDFStar.h \
 ../../../../../src/kernel/DataFlowStar.h \
 ../../../../../src/kernel/Star.h ../../../../../src/kernel/Block.h \
 ../../../../../src/kernel/NamedObj.h \
 ../../../../../src/kernel/StringList.h \
 ../../../../../src/kernel/miscFuncs.h \
 ../../../../../src/kernel/logNew.h ../../../../../src/kernel/Error.h \
 ../../../../../src/kernel/DataStruct.h \
 ../../../../../src/kernel/type.h ../../../../../src/kernel/isa.h \
 ../../../../../src/kernel/FlagArray.h \
 ../../../../../src/kernel/PortHole.h \
 ../../../../../src/kernel/dataType.h \
 ../../../../../src/kernel/Particle.h ../../../../../src/kernel/Fix.h \
 ../../../../../src/kernel/ComplexSubset.h \
 ../../../../../src/kernel/Attribute.h \
 ../../../../../src/kernel/State.h \
 ../../../../../src/kernel/InvokeInterp.h \
 ../../../../../src/kernel/Fraction.h \
 ../../../../../src/domains/sdf/kernel/SDFPortHole.h \
 ../../../../../src/kernel/DFPortHole.h \
 ../../../../../src/kernel/Matrix.h \
 ../../../../../src/kernel/Message.h \
 ../../../../../src/kernel/ComplexArrayState.h \
 ../../../../../src/kernel/FixArrayState.h \
 ../../../../../src/kernel/PrecisionState.h \
 ../../../../../src/kernel/FloatArrayState.h \
 ../../../../../src/kernel/IntArrayState.h
SDFInverseCx_M.o: SDFInverseCx_M.cc SDFInverseCx_M.h \
 ../../../../../src/domains/sdf/kernel/SDFStar.h \
 ../../../../../src/kernel/DataFlowStar.h \
 ../../../../../src/kernel/Star.h ../../../../../src/kernel/Block.h \
 ../../../../../src/kernel/NamedObj.h \
 ../../../../../src/kernel/StringList.h \
 ../../../../../src/kernel/miscFuncs.h \
 ../../../../../src/kernel/logNew.h ../../../../../src/kernel/Error.h \
 ../../../../../src/kernel/DataStruct.h \
 ../../../../../src/kernel/type.h ../../../../../src/kernel/isa.h \
 ../../../../../src/kernel/FlagArray.h \
 ../../../../../src/kernel/PortHole.h \
 ../../../../../src/kernel/dataType.h \
 ../../../../../src/kernel/Particle.h ../../../../../src/kernel/Fix.h \
 ../../../../../src/kernel/ComplexSubset.h \
 ../../../../../src/kernel/Attribute.h \
 ../../../../../src/kernel/State.h \
 ../../../../../src/kernel/InvokeInterp.h \
 ../../../../../src/kernel/Fraction.h \
 ../../../../../src/domains/sdf/kernel/SDFPortHole.h \
 ../../../../../src/kernel/DFPortHole.h \
 ../../../../../src/kernel/IntState.h \
 ../../../../../src/kernel/Matrix.h \
 ../../../../../src/kernel/Message.h \
 ../../../../../src/kernel/ComplexArrayState.h \
 ../../../../../src/kernel/FixArrayState.h \
 ../../../../../src/kernel/PrecisionState.h \
 ../../../../../src/kernel/FloatArrayState.h \
 ../../../../../src/kernel/IntArrayState.h
SDFInverseFix_M.o: SDFInverseFix_M.cc SDFInverseFix_M.h \
 ../../../../../src/domains/sdf/kernel/SDFStar.h \
 ../../../../../src/kernel/DataFlowStar.h \
 ../../../../../src/kernel/Star.h ../../../../../src/kernel/Block.h \
 ../../../../../src/kernel/NamedObj.h \
 ../../../../../src/kernel/StringList.h \
 ../../../../../src/kernel/miscFuncs.h \
 ../../../../../src/kernel/logNew.h ../../../../../src/kernel/Error.h \
 ../../../../../src/kernel/DataStruct.h \
 ../../../../../src/kernel/type.h ../../../../../src/kernel/isa.h \
 ../../../../../src/kernel/FlagArray.h \
 ../../../../../src/kernel/PortHole.h \
 ../../../../../src/kernel/dataType.h \
 ../../../../../src/kernel/Particle.h ../../../../../src/kernel/Fix.h \
 ../../../../../src/kernel/ComplexSubset.h \
 ../../../../../src/kernel/Attribute.h \
 ../../../../../src/kernel/State.h \
 ../../../../../src/kernel/InvokeInterp.h \
 ../../../../../src/kernel/Fraction.h \
 ../../../../../src/domains/sdf/kernel/SDFPortHole.h \
 ../../../../../src/kernel/DFPortHole.h \
 ../../../../../src/kernel/IntState.h \
 ../../../../../src/kernel/Matrix.h \
 ../../../../../src/kernel/Message.h \
 ../../../../../src/kernel/ComplexArrayState.h \
 ../../../../../src/kernel/FixArrayState.h \
 ../../../../../src/kernel/PrecisionState.h \
 ../../../../../src/kernel/FloatArrayState.h \
 ../../../../../src/kernel/IntArrayState.h
SDFInverseInt_M.o: SDFInverseInt_M.cc SDFInverseInt_M.h \
 ../../../../../src/domains/sdf/kernel/SDFStar.h \
 ../../../../../src/kernel/DataFlowStar.h \
 ../../../../../src/kernel/Star.h ../../../../../src/kernel/Block.h \
 ../../../../../src/kernel/NamedObj.h \
 ../../../../../src/kernel/StringList.h \
 ../../../../../src/kernel/miscFuncs.h \
 ../../../../../src/kernel/logNew.h ../../../../../src/kernel/Error.h \
 ../../../../../src/kernel/DataStruct.h \
 ../../../../../src/kernel/type.h ../../../../../src/kernel/isa.h \
 ../../../../../src/kernel/FlagArray.h \
 ../../../../../src/kernel/PortHole.h \
 ../../../../../src/kernel/dataType.h \
 ../../../../../src/kernel/Particle.h ../../../../../src/kernel/Fix.h \
 ../../../../../src/kernel/ComplexSubset.h \
 ../../../../../src/kernel/Attribute.h \
 ../../../../../src/kernel/State.h \
 ../../../../../src/kernel/InvokeInterp.h \
 ../../../../../src/kernel/Fraction.h \
 ../../../../../src/domains/sdf/kernel/SDFPortHole.h \
 ../../../../../src/kernel/DFPortHole.h \
 ../../../../../src/kernel/IntState.h \
 ../../../../../src/kernel/Matrix.h \
 ../../../../../src/kernel/Message.h \
 ../../../../../src/kernel/ComplexArrayState.h \
 ../../../../../src/kernel/FixArrayState.h \
 ../../../../../src/kernel/PrecisionState.h \
 ../../../../../src/kernel/FloatArrayState.h \
 ../../../../../src/kernel/IntArrayState.h
SDFInverse_M.o: SDFInverse_M.cc SDFInverse_M.h \
 ../../../../../src/domains/sdf/kernel/SDFStar.h \
 ../../../../../src/kernel/DataFlowStar.h \
 ../../../../../src/kernel/Star.h ../../../../../src/kernel/Block.h \
 ../../../../../src/kernel/NamedObj.h \
 ../../../../../src/kernel/StringList.h \
 ../../../../../src/kernel/miscFuncs.h \
 ../../../../../src/kernel/logNew.h ../../../../../src/kernel/Error.h \
 ../../../../../src/kernel/DataStruct.h \
 ../../../../../src/kernel/type.h ../../../../../src/kernel/isa.h \
 ../../../../../src/kernel/FlagArray.h \
 ../../../../../src/kernel/PortHole.h \
 ../../../../../src/kernel/dataType.h \
 ../../../../../src/kernel/Particle.h ../../../../../src/kernel/Fix.h \
 ../../../../../src/kernel/ComplexSubset.h \
 ../../../../../src/kernel/Attribute.h \
 ../../../../../src/kernel/State.h \
 ../../../../../src/kernel/InvokeInterp.h \
 ../../../../../src/kernel/Fraction.h \
 ../../../../../src/domains/sdf/kernel/SDFPortHole.h \
 ../../../../../src/kernel/DFPortHole.h \
 ../../../../../src/kernel/IntState.h \
 ../../../../../src/kernel/Matrix.h \
 ../../../../../src/kernel/Message.h \
 ../../../../../src/kernel/ComplexArrayState.h \
 ../../../../../src/kernel/FixArrayState.h \
 ../../../../../src/kernel/PrecisionState.h \
 ../../../../../src/kernel/FloatArrayState.h \
 ../../../../../src/kernel/IntArrayState.h
SDFMatrix.o: SDFMatrix.cc SDFMatrix.h \
 ../../../../../src/domains/sdf/kernel/SDFStar.h \
 ../../../../../src/kernel/DataFlowStar.h \
 ../../../../../src/kernel/Star.h ../../../../../src/kernel/Block.h \
 ../../../../../src/kernel/NamedObj.h \
 ../../../../../src/kernel/StringList.h \
 ../../../../../src/kernel/miscFuncs.h \
 ../../../../../src/kernel/logNew.h ../../../../../src/kernel/Error.h \
 ../../../../../src/kernel/DataStruct.h \
 ../../../../../src/kernel/type.h ../../../../../src/kernel/isa.h \
 ../../../../../src/kernel/FlagArray.h \
 ../../../../../src/kernel/PortHole.h \
 ../../../../../src/kernel/dataType.h \
 ../../../../../src/kernel/Particle.h ../../../../../src/kernel/Fix.h \
 ../../../../../src/kernel/ComplexSubset.h \
 ../../../../../src/kernel/Attribute.h \
 ../../../../../src/kernel/State.h \
 ../../../../../src/kernel/InvokeInterp.h \
 ../../../../../src/kernel/Fraction.h \
 ../../../../../src/domains/sdf/kernel/SDFPortHole.h \
 ../../../../../src/kernel/DFPortHole.h \
 ../../../../../src/kernel/IntState.h \
 ../../../../../src/kernel/FloatArrayState.h \
 ../../../../../src/kernel/Matrix.h \
 ../../../../../src/kernel/Message.h \
 ../../../../../src/kernel/ComplexArrayState.h \
 ../../../../../src/kernel/FixArrayState.h \
 ../../../../../src/kernel/PrecisionState.h \
 ../../../../../src/kernel/IntArrayState.h
SDFMatrixCx.o: SDFMatrixCx.cc SDFMatrixCx.h \
 ../../../../../src/domains/sdf/kernel/SDFStar.h \
 ../../../../../src/kernel/DataFlowStar.h \
 ../../../../../src/kernel/Star.h ../../../../../src/kernel/Block.h \
 ../../../../../src/kernel/NamedObj.h \
 ../../../../../src/kernel/StringList.h \
 ../../../../../src/kernel/miscFuncs.h \
 ../../../../../src/kernel/logNew.h ../../../../../src/kernel/Error.h \
 ../../../../../src/kernel/DataStruct.h \
 ../../../../../src/kernel/type.h ../../../../../src/kernel/isa.h \
 ../../../../../src/kernel/FlagArray.h \
 ../../../../../src/kernel/PortHole.h \
 ../../../../../src/kernel/dataType.h \
 ../../../../../src/kernel/Particle.h ../../../../../src/kernel/Fix.h \
 ../../../../../src/kernel/ComplexSubset.h \
 ../../../../../src/kernel/Attribute.h \
 ../../../../../src/kernel/State.h \
 ../../../../../src/kernel/InvokeInterp.h \
 ../../../../../src/kernel/Fraction.h \
 ../../../../../src/domains/sdf/kernel/SDFPortHole.h \
 ../../../../../src/kernel/DFPortHole.h \
 ../../../../../src/kernel/IntState.h \
 ../../../../../src/kernel/ComplexArrayState.h \
 ../../../../../src/kernel/Matrix.h \
 ../../../../../src/kernel/Message.h \
 ../../../../../src/kernel/FixArrayState.h \
 ../../../../../src/kernel/PrecisionState.h \
 ../../../../../src/kernel/FloatArrayState.h \
 ../../../../../src/kernel/IntArrayState.h
SDFMatrixFix.o: SDFMatrixFix.cc SDFMatrixFix.h \
 ../../../../../src/domains/sdf/kernel/SDFStar.h \
 ../../../../../src/kernel/DataFlowStar.h \
 ../../../../../src/kernel/Star.h ../../../../../src/kernel/Block.h \
 ../../../../../src/kernel/NamedObj.h \
 ../../../../../src/kernel/StringList.h \
 ../../../../../src/kernel/miscFuncs.h \
 ../../../../../src/kernel/logNew.h ../../../../../src/kernel/Error.h \
 ../../../../../src/kernel/DataStruct.h \
 ../../../../../src/kernel/type.h ../../../../../src/kernel/isa.h \
 ../../../../../src/kernel/FlagArray.h \
 ../../../../../src/kernel/PortHole.h \
 ../../../../../src/kernel/dataType.h \
 ../../../../../src/kernel/Particle.h ../../../../../src/kernel/Fix.h \
 ../../../../../src/kernel/ComplexSubset.h \
 ../../../../../src/kernel/Attribute.h \
 ../../../../../src/kernel/State.h \
 ../../../../../src/kernel/InvokeInterp.h \
 ../../../../../src/kernel/Fraction.h \
 ../../../../../src/domains/sdf/kernel/SDFPortHole.h \
 ../../../../../src/kernel/DFPortHole.h \
 ../../../../../src/kernel/IntState.h \
 ../../../../../src/kernel/StringState.h \
 ../../../../../src/kernel/KnownState.h \
 ../../../../../src/kernel/FixArrayState.h \
 ../../../../../src/kernel/PrecisionState.h \
 ../../../../../src/kernel/Matrix.h \
 ../../../../../src/kernel/Message.h \
 ../../../../../src/kernel/ComplexArrayState.h \
 ../../../../../src/kernel/FloatArrayState.h \
 ../../../../../src/kernel/IntArrayState.h
SDFMatrixInt.o: SDFMatrixInt.cc SDFMatrixInt.h \
 ../../../../../src/domains/sdf/kernel/SDFStar.h \
 ../../../../../src/kernel/DataFlowStar.h \
 ../../../../../src/kernel/Star.h ../../../../../src/kernel/Block.h \
 ../../../../../src/kernel/NamedObj.h \
 ../../../../../src/kernel/StringList.h \
 ../../../../../src/kernel/miscFuncs.h \
 ../../../../../src/kernel/logNew.h ../../../../../src/kernel/Error.h \
 ../../../../../src/kernel/DataStruct.h \
 ../../../../../src/kernel/type.h ../../../../../src/kernel/isa.h \
 ../../../../../src/kernel/FlagArray.h \
 ../../../../../src/kernel/PortHole.h \
 ../../../../../src/kernel/dataType.h \
 ../../../../../src/kernel/Particle.h ../../../../../src/kernel/Fix.h \
 ../../../../../src/kernel/ComplexSubset.h \
 ../../../../../src/kernel/Attribute.h \
 ../../../../../src/kernel/State.h \
 ../../../../../src/kernel/InvokeInterp.h \
 ../../../../../src/kernel/Fraction.h \
 ../../../../../src/domains/sdf/kernel/SDFPortHole.h \
 ../../../../../src/kernel/DFPortHole.h \
 ../../../../../src/kernel/IntState.h \
 ../../../../../src/kernel/IntArrayState.h \
 ../../../../../src/kernel/Matrix.h \
 ../../../../../src/kernel/Message.h \
 ../../../../../src/kernel/ComplexArrayState.h \
 ../../../../../src/kernel/FixArrayState.h \
 ../../../../../src/kernel/PrecisionState.h \
 ../../../../../src/kernel/FloatArrayState.h
SDFMpyCx_M.o: SDFMpyCx_M.cc SDFMpyCx_M.h \
 ../../../../../src/domains/sdf/kernel/SDFStar.h \
 ../../../../../src/kernel/DataFlowStar.h \
 ../../../../../src/kernel/Star.h ../../../../../src/kernel/Block.h \
 ../../../../../src/kernel/NamedObj.h \
 ../../../../../src/kernel/StringList.h \
 ../../../../../src/kernel/miscFuncs.h \
 ../../../../../src/kernel/logNew.h ../../../../../src/kernel/Error.h \
 ../../../../../src/kernel/DataStruct.h \
 ../../../../../src/kernel/type.h ../../../../../src/kernel/isa.h \
 ../../../../../src/kernel/FlagArray.h \
 ../../../../../src/kernel/PortHole.h \
 ../../../../../src/kernel/dataType.h \
 ../../../../../src/kernel/Particle.h ../../../../../src/kernel/Fix.h \
 ../../../../../src/kernel/ComplexSubset.h \
 ../../../../../src/kernel/Attribute.h \
 ../../../../../src/kernel/State.h \
 ../../../../../src/kernel/InvokeInterp.h \
 ../../../../../src/kernel/Fraction.h \
 ../../../../../src/domains/sdf/kernel/SDFPortHole.h \
 ../../../../../src/kernel/DFPortHole.h \
 ../../../../../src/kernel/IntState.h \
 ../../../../../src/kernel/Matrix.h \
 ../../../../../src/kernel/Message.h \
 ../../../../../src/kernel/ComplexArrayState.h \
 ../../../../../src/kernel/FixArrayState.h \
 ../../../../../src/kernel/PrecisionState.h \
 ../../../../../src/kernel/FloatArrayState.h \
 ../../../../../src/kernel/IntArrayState.h
SDFMpyFix_M.o: SDFMpyFix_M.cc SDFMpyFix_M.h \
 ../../../../../src/domains/sdf/kernel/SDFStar.h \
 ../../../../../src/kernel/DataFlowStar.h \
 ../../../../../src/kernel/Star.h ../../../../../src/kernel/Block.h \
 ../../../../../src/kernel/NamedObj.h \
 ../../../../../src/kernel/StringList.h \
 ../../../../../src/kernel/miscFuncs.h \
 ../../../../../src/kernel/logNew.h ../../../../../src/kernel/Error.h \
 ../../../../../src/kernel/DataStruct.h \
 ../../../../../src/kernel/type.h ../../../../../src/kernel/isa.h \
 ../../../../../src/kernel/FlagArray.h \
 ../../../../../src/kernel/PortHole.h \
 ../../../../../src/kernel/dataType.h \
 ../../../../../src/kernel/Particle.h ../../../../../src/kernel/Fix.h \
 ../../../../../src/kernel/ComplexSubset.h \
 ../../../../../src/kernel/Attribute.h \
 ../../../../../src/kernel/State.h \
 ../../../../../src/kernel/InvokeInterp.h \
 ../../../../../src/kernel/Fraction.h \
 ../../../../../src/domains/sdf/kernel/SDFPortHole.h \
 ../../../../../src/kernel/DFPortHole.h \
 ../../../../../src/kernel/IntState.h \
 ../../../../../src/kernel/StringState.h \
 ../../../../../src/kernel/KnownState.h \
 ../../../../../src/kernel/Matrix.h \
 ../../../../../src/kernel/Message.h \
 ../../../../../src/kernel/ComplexArrayState.h \
 ../../../../../src/kernel/FixArrayState.h \
 ../../../../../src/kernel/PrecisionState.h \
 ../../../../../src/kernel/FloatArrayState.h \
 ../../../../../src/kernel/IntArrayState.h
SDFMpyInt_M.o: SDFMpyInt_M.cc SDFMpyInt_M.h \
 ../../../../../src/domains/sdf/kernel/SDFStar.h \
 ../../../../../src/kernel/DataFlowStar.h \
 ../../../../../src/kernel/Star.h ../../../../../src/kernel/Block.h \
 ../../../../../src/kernel/NamedObj.h \
 ../../../../../src/kernel/StringList.h \
 ../../../../../src/kernel/miscFuncs.h \
 ../../../../../src/kernel/logNew.h ../../../../../src/kernel/Error.h \
 ../../../../../src/kernel/DataStruct.h \
 ../../../../../src/kernel/type.h ../../../../../src/kernel/isa.h \
 ../../../../../src/kernel/FlagArray.h \
 ../../../../../src/kernel/PortHole.h \
 ../../../../../src/kernel/dataType.h \
 ../../../../../src/kernel/Particle.h ../../../../../src/kernel/Fix.h \
 ../../../../../src/kernel/ComplexSubset.h \
 ../../../../../src/kernel/Attribute.h \
 ../../../../../src/kernel/State.h \
 ../../../../../src/kernel/InvokeInterp.h \
 ../../../../../src/kernel/Fraction.h \
 ../../../../../src/domains/sdf/kernel/SDFPortHole.h \
 ../../../../../src/kernel/DFPortHole.h \
 ../../../../../src/kernel/IntState.h \
 ../../../../../src/kernel/Matrix.h \
 ../../../../../src/kernel/Message.h \
 ../../../../../src/kernel/ComplexArrayState.h \
 ../../../../../src/kernel/FixArrayState.h \
 ../../../../../src/kernel/PrecisionState.h \
 ../../../../../src/kernel/FloatArrayState.h \
 ../../../../../src/kernel/IntArrayState.h
SDFMpy_M.o: SDFMpy_M.cc SDFMpy_M.h \
 ../../../../../src/domains/sdf/kernel/SDFStar.h \
 ../../../../../src/kernel/DataFlowStar.h \
 ../../../../../src/kernel/Star.h ../../../../../src/kernel/Block.h \
 ../../../../../src/kernel/NamedObj.h \
 ../../../../../src/kernel/StringList.h \
 ../../../../../src/kernel/miscFuncs.h \
 ../../../../../src/kernel/logNew.h ../../../../../src/kernel/Error.h \
 ../../../../../src/kernel/DataStruct.h \
 ../../../../../src/kernel/type.h ../../../../../src/kernel/isa.h \
 ../../../../../src/kernel/FlagArray.h \
 ../../../../../src/kernel/PortHole.h \
 ../../../../../src/kernel/dataType.h \
 ../../../../../src/kernel/Particle.h ../../../../../src/kernel/Fix.h \
 ../../../../../src/kernel/ComplexSubset.h \
 ../../../../../src/kernel/Attribute.h \
 ../../../../../src/kernel/State.h \
 ../../../../../src/kernel/InvokeInterp.h \
 ../../../../../src/kernel/Fraction.h \
 ../../../../../src/domains/sdf/kernel/SDFPortHole.h \
 ../../../../../src/kernel/DFPortHole.h \
 ../../../../../src/kernel/IntState.h \
 ../../../../../src/kernel/Matrix.h \
 ../../../../../src/kernel/Message.h \
 ../../../../../src/kernel/ComplexArrayState.h \
 ../../../../../src/kernel/FixArrayState.h \
 ../../../../../src/kernel/PrecisionState.h \
 ../../../../../src/kernel/FloatArrayState.h \
 ../../../../../src/kernel/IntArrayState.h
SDFMpyScalarCx_M.o: SDFMpyScalarCx_M.cc SDFMpyScalarCx_M.h \
 ../../../../../src/domains/sdf/kernel/SDFStar.h \
 ../../../../../src/kernel/DataFlowStar.h \
 ../../../../../src/kernel/Star.h ../../../../../src/kernel/Block.h \
 ../../../../../src/kernel/NamedObj.h \
 ../../../../../src/kernel/StringList.h \
 ../../../../../src/kernel/miscFuncs.h \
 ../../../../../src/kernel/logNew.h ../../../../../src/kernel/Error.h \
 ../../../../../src/kernel/DataStruct.h \
 ../../../../../src/kernel/type.h ../../../../../src/kernel/isa.h \
 ../../../../../src/kernel/FlagArray.h \
 ../../../../../src/kernel/PortHole.h \
 ../../../../../src/kernel/dataType.h \
 ../../../../../src/kernel/Particle.h ../../../../../src/kernel/Fix.h \
 ../../../../../src/kernel/ComplexSubset.h \
 ../../../../../src/kernel/Attribute.h \
 ../../../../../src/kernel/State.h \
 ../../../../../src/kernel/InvokeInterp.h \
 ../../../../../src/kernel/Fraction.h \
 ../../../../../src/domains/sdf/kernel/SDFPortHole.h \
 ../../../../../src/kernel/DFPortHole.h \
 ../../../../../src/kernel/Matrix.h \
 ../../../../../src/kernel/Message.h \
 ../../../../../src/kernel/ComplexArrayState.h \
 ../../../../../src/kernel/FixArrayState.h \
 ../../../../../src/kernel/PrecisionState.h \
 ../../../../../src/kernel/FloatArrayState.h \
 ../../../../../src/kernel/IntArrayState.h
SDFMpyScalarFix_M.o: SDFMpyScalarFix_M.cc SDFMpyScalarFix_M.h \
 ../../../../../src/domains/sdf/kernel/SDFStar.h \
 ../../../../../src/kernel/DataFlowStar.h \
 ../../../../../src/kernel/Star.h ../../../../../src/kernel/Block.h \
 ../../../../../src/kernel/NamedObj.h \
 ../../../../../src/kernel/StringList.h \
 ../../../../../src/kernel/miscFuncs.h \
 ../../../../../src/kernel/logNew.h ../../../../../src/kernel/Error.h \
 ../../../../../src/kernel/DataStruct.h \
 ../../../../../src/kernel/type.h ../../../../../src/kernel/isa.h \
 ../../../../../src/kernel/FlagArray.h \
 ../../../../../src/kernel/PortHole.h \
 ../../../../../src/kernel/dataType.h \
 ../../../../../src/kernel/Particle.h ../../../../../src/kernel/Fix.h \
 ../../../../../src/kernel/ComplexSubset.h \
 ../../../../../src/kernel/Attribute.h \
 ../../../../../src/kernel/State.h \
 ../../../../../src/kernel/InvokeInterp.h \
 ../../../../../src/kernel/Fraction.h \
 ../../../../../src/domains/sdf/kernel/SDFPortHole.h \
 ../../../../../src/kernel/DFPortHole.h \
 ../../../../../src/kernel/IntState.h \
 ../../../../../src/kernel/StringState.h \
 ../../../../../src/kernel/KnownState.h \
 ../../../../../src/kernel/Matrix.h \
 ../../../../../src/kernel/Message.h \
 ../../../../../src/kernel/ComplexArrayState.h \
 ../../../../../src/kernel/FixArrayState.h \
 ../../../../../src/kernel/PrecisionState.h \
 ../../../../../src/kernel/FloatArrayState.h \
 ../../../../../src/kernel/IntArrayState.h
SDFMpyScalarInt_M.o: SDFMpyScalarInt_M.cc SDFMpyScalarInt_M.h \
 ../../../../../src/domains/sdf/kernel/SDFStar.h \
 ../../../../../src/kernel/DataFlowStar.h \
 ../../../../../src/kernel/Star.h ../../../../../src/kernel/Block.h \
 ../../../../../src/kernel/NamedObj.h \
 ../../../../../src/kernel/StringList.h \
 ../../../../../src/kernel/miscFuncs.h \
 ../../../../../src/kernel/logNew.h ../../../../../src/kernel/Error.h \
 ../../../../../src/kernel/DataStruct.h \
 ../../../../../src/kernel/type.h ../../../../../src/kernel/isa.h \
 ../../../../../src/kernel/FlagArray.h \
 ../../../../../src/kernel/PortHole.h \
 ../../../../../src/kernel/dataType.h \
 ../../../../../src/kernel/Particle.h ../../../../../src/kernel/Fix.h \
 ../../../../../src/kernel/ComplexSubset.h \
 ../../../../../src/kernel/Attribute.h \
 ../../../../../src/kernel/State.h \
 ../../../../../src/kernel/InvokeInterp.h \
 ../../../../../src/kernel/Fraction.h \
 ../../../../../src/domains/sdf/kernel/SDFPortHole.h \
 ../../../../../src/kernel/DFPortHole.h \
 ../../../../../src/kernel/Matrix.h \
 ../../../../../src/kernel/Message.h \
 ../../../../../src/kernel/ComplexArrayState.h \
 ../../../../../src/kernel/FixArrayState.h \
 ../../../../../src/kernel/PrecisionState.h \
 ../../../../../src/kernel/FloatArrayState.h \
 ../../../../../src/kernel/IntArrayState.h
SDFMpyScalar_M.o: SDFMpyScalar_M.cc SDFMpyScalar_M.h \
 ../../../../../src/domains/sdf/kernel/SDFStar.h \
 ../../../../../src/kernel/DataFlowStar.h \
 ../../../../../src/kernel/Star.h ../../../../../src/kernel/Block.h \
 ../../../../../src/kernel/NamedObj.h \
 ../../../../../src/kernel/StringList.h \
 ../../../../../src/kernel/miscFuncs.h \
 ../../../../../src/kernel/logNew.h ../../../../../src/kernel/Error.h \
 ../../../../../src/kernel/DataStruct.h \
 ../../../../../src/kernel/type.h ../../../../../src/kernel/isa.h \
 ../../../../../src/kernel/FlagArray.h \
 ../../../../../src/kernel/PortHole.h \
 ../../../../../src/kernel/dataType.h \
 ../../../../../src/kernel/Particle.h ../../../../../src/kernel/Fix.h \
 ../../../../../src/kernel/ComplexSubset.h \
 ../../../../../src/kernel/Attribute.h \
 ../../../../../src/kernel/State.h \
 ../../../../../src/kernel/InvokeInterp.h \
 ../../../../../src/kernel/Fraction.h \
 ../../../../../src/domains/sdf/kernel/SDFPortHole.h \
 ../../../../../src/kernel/DFPortHole.h \
 ../../../../../src/kernel/Matrix.h \
 ../../../../../src/kernel/Message.h \
 ../../../../../src/kernel/ComplexArrayState.h \
 ../../../../../src/kernel/FixArrayState.h \
 ../../../../../src/kernel/PrecisionState.h \
 ../../../../../src/kernel/FloatArrayState.h \
 ../../../../../src/kernel/IntArrayState.h
SDFMxCom_M.o: SDFMxCom_M.cc SDFMxCom_M.h \
 ../../../../../src/domains/sdf/kernel/SDFStar.h \
 ../../../../../src/kernel/DataFlowStar.h \
 ../../../../../src/kernel/Star.h ../../../../../src/kernel/Block.h \
 ../../../../../src/kernel/NamedObj.h \
 ../../../../../src/kernel/StringList.h \
 ../../../../../src/kernel/miscFuncs.h \
 ../../../../../src/kernel/logNew.h ../../../../../src/kernel/Error.h \
 ../../../../../src/kernel/DataStruct.h \
 ../../../../../src/kernel/type.h ../../../../../src/kernel/isa.h \
 ../../../../../src/kernel/FlagArray.h \
 ../../../../../src/kernel/PortHole.h \
 ../../../../../src/kernel/dataType.h \
 ../../../../../src/kernel/Particle.h ../../../../../src/kernel/Fix.h \
 ../../../../../src/kernel/ComplexSubset.h \
 ../../../../../src/kernel/Attribute.h \
 ../../../../../src/kernel/State.h \
 ../../../../../src/kernel/InvokeInterp.h \
 ../../../../../src/kernel/Fraction.h \
 ../../../../../src/domains/sdf/kernel/SDFPortHole.h \
 ../../../../../src/kernel/DFPortHole.h \
 ../../../../../src/kernel/IntState.h \
 ../../../../../src/kernel/Matrix.h \
 ../../../../../src/kernel/Message.h \
 ../../../../../src/kernel/ComplexArrayState.h \
 ../../../../../src/kernel/FixArrayState.h \
 ../../../../../src/kernel/PrecisionState.h \
 ../../../../../src/kernel/FloatArrayState.h \
 ../../../../../src/kernel/IntArrayState.h
SDFMxDecom_M.o: SDFMxDecom_M.cc SDFMxDecom_M.h \
 ../../../../../src/domains/sdf/kernel/SDFStar.h \
 ../../../../../src/kernel/DataFlowStar.h \
 ../../../../../src/kernel/Star.h ../../../../../src/kernel/Block.h \
 ../../../../../src/kernel/NamedObj.h \
 ../../../../../src/kernel/StringList.h \
 ../../../../../src/kernel/miscFuncs.h \
 ../../../../../src/kernel/logNew.h ../../../../../src/kernel/Error.h \
 ../../../../../src/kernel/DataStruct.h \
 ../../../../../src/kernel/type.h ../../../../../src/kernel/isa.h \
 ../../../../../src/kernel/FlagArray.h \
 ../../../../../src/kernel/PortHole.h \
 ../../../../../src/kernel/dataType.h \
 ../../../../../src/kernel/Particle.h ../../../../../src/kernel/Fix.h \
 ../../../../../src/kernel/ComplexSubset.h \
 ../../../../../src/kernel/Attribute.h \
 ../../../../../src/kernel/State.h \
 ../../../../../src/kernel/InvokeInterp.h \
 ../../../../../src/kernel/Fraction.h \
 ../../../../../src/domains/sdf/kernel/SDFPortHole.h \
 ../../../../../src/kernel/DFPortHole.h \
 ../../../../../src/kernel/IntState.h \
 ../../../../../src/kernel/Matrix.h \
 ../../../../../src/kernel/Message.h \
 ../../../../../src/kernel/ComplexArrayState.h \
 ../../../../../src/kernel/FixArrayState.h \
 ../../../../../src/kernel/PrecisionState.h \
 ../../../../../src/kernel/FloatArrayState.h \
 ../../../../../src/kernel/IntArrayState.h
SDFPackCx_M.o: SDFPackCx_M.cc SDFPackCx_M.h \
 ../../../../../src/domains/sdf/kernel/SDFStar.h \
 ../../../../../src/kernel/DataFlowStar.h \
 ../../../../../src/kernel/Star.h ../../../../../src/kernel/Block.h \
 ../../../../../src/kernel/NamedObj.h \
 ../../../../../src/kernel/StringList.h \
 ../../../../../src/kernel/miscFuncs.h \
 ../../../../../src/kernel/logNew.h ../../../../../src/kernel/Error.h \
 ../../../../../src/kernel/DataStruct.h \
 ../../../../../src/kernel/type.h ../../../../../src/kernel/isa.h \
 ../../../../../src/kernel/FlagArray.h \
 ../../../../../src/kernel/PortHole.h \
 ../../../../../src/kernel/dataType.h \
 ../../../../../src/kernel/Particle.h ../../../../../src/kernel/Fix.h \
 ../../../../../src/kernel/ComplexSubset.h \
 ../../../../../src/kernel/Attribute.h \
 ../../../../../src/kernel/State.h \
 ../../../../../src/kernel/InvokeInterp.h \
 ../../../../../src/kernel/Fraction.h \
 ../../../../../src/domains/sdf/kernel/SDFPortHole.h \
 ../../../../../src/kernel/DFPortHole.h \
 ../../../../../src/kernel/IntState.h \
 ../../../../../src/kernel/Matrix.h \
 ../../../../../src/kernel/Message.h \
 ../../../../../src/kernel/ComplexArrayState.h \
 ../../../../../src/kernel/FixArrayState.h \
 ../../../../../src/kernel/PrecisionState.h \
 ../../../../../src/kernel/FloatArrayState.h \
 ../../../../../src/kernel/IntArrayState.h
SDFPackFix_M.o: SDFPackFix_M.cc SDFPackFix_M.h \
 ../../../../../src/domains/sdf/kernel/SDFStar.h \
 ../../../../../src/kernel/DataFlowStar.h \
 ../../../../../src/kernel/Star.h ../../../../../src/kernel/Block.h \
 ../../../../../src/kernel/NamedObj.h \
 ../../../../../src/kernel/StringList.h \
 ../../../../../src/kernel/miscFuncs.h \
 ../../../../../src/kernel/logNew.h ../../../../../src/kernel/Error.h \
 ../../../../../src/kernel/DataStruct.h \
 ../../../../../src/kernel/type.h ../../../../../src/kernel/isa.h \
 ../../../../../src/kernel/FlagArray.h \
 ../../../../../src/kernel/PortHole.h \
 ../../../../../src/kernel/dataType.h \
 ../../../../../src/kernel/Particle.h ../../../../../src/kernel/Fix.h \
 ../../../../../src/kernel/ComplexSubset.h \
 ../../../../../src/kernel/Attribute.h \
 ../../../../../src/kernel/State.h \
 ../../../../../src/kernel/InvokeInterp.h \
 ../../../../../src/kernel/Fraction.h \
 ../../../../../src/domains/sdf/kernel/SDFPortHole.h \
 ../../../../../src/kernel/DFPortHole.h \
 ../../../../../src/kernel/IntState.h \
 ../../../../../src/kernel/Matrix.h \
 ../../../../../src/kernel/Message.h \
 ../../../../../src/kernel/ComplexArrayState.h \
 ../../../../../src/kernel/FixArrayState.h \
 ../../../../../src/kernel/PrecisionState.h \
 ../../../../../src/kernel/FloatArrayState.h \
 ../../../../../src/kernel/IntArrayState.h
SDFPackInt_M.o: SDFPackInt_M.cc SDFPackInt_M.h \
 ../../../../../src/domains/sdf/kernel/SDFStar.h \
 ../../../../../src/kernel/DataFlowStar.h \
 ../../../../../src/kernel/Star.h ../../../../../src/kernel/Block.h \
 ../../../../../src/kernel/NamedObj.h \
 ../../../../../src/kernel/StringList.h \
 ../../../../../src/kernel/miscFuncs.h \
 ../../../../../src/kernel/logNew.h ../../../../../src/kernel/Error.h \
 ../../../../../src/kernel/DataStruct.h \
 ../../../../../src/kernel/type.h ../../../../../src/kernel/isa.h \
 ../../../../../src/kernel/FlagArray.h \
 ../../../../../src/kernel/PortHole.h \
 ../../../../../src/kernel/dataType.h \
 ../../../../../src/kernel/Particle.h ../../../../../src/kernel/Fix.h \
 ../../../../../src/kernel/ComplexSubset.h \
 ../../../../../src/kernel/Attribute.h \
 ../../../../../src/kernel/State.h \
 ../../../../../src/kernel/InvokeInterp.h \
 ../../../../../src/kernel/Fraction.h \
 ../../../../../src/domains/sdf/kernel/SDFPortHole.h \
 ../../../../../src/kernel/DFPortHole.h \
 ../../../../../src/kernel/IntState.h \
 ../../../../../src/kernel/Matrix.h \
 ../../../../../src/kernel/Message.h \
 ../../../../../src/kernel/ComplexArrayState.h \
 ../../../../../src/kernel/FixArrayState.h \
 ../../../../../src/kernel/PrecisionState.h \
 ../../../../../src/kernel/FloatArrayState.h \
 ../../../../../src/kernel/IntArrayState.h
SDFPack_M.o: SDFPack_M.cc SDFPack_M.h \
 ../../../../../src/domains/sdf/kernel/SDFStar.h \
 ../../../../../src/kernel/DataFlowStar.h \
 ../../../../../src/kernel/Star.h ../../../../../src/kernel/Block.h \
 ../../../../../src/kernel/NamedObj.h \
 ../../../../../src/kernel/StringList.h \
 ../../../../../src/kernel/miscFuncs.h \
 ../../../../../src/kernel/logNew.h ../../../../../src/kernel/Error.h \
 ../../../../../src/kernel/DataStruct.h \
 ../../../../../src/kernel/type.h ../../../../../src/kernel/isa.h \
 ../../../../../src/kernel/FlagArray.h \
 ../../../../../src/kernel/PortHole.h \
 ../../../../../src/kernel/dataType.h \
 ../../../../../src/kernel/Particle.h ../../../../../src/kernel/Fix.h \
 ../../../../../src/kernel/ComplexSubset.h \
 ../../../../../src/kernel/Attribute.h \
 ../../../../../src/kernel/State.h \
 ../../../../../src/kernel/InvokeInterp.h \
 ../../../../../src/kernel/Fraction.h \
 ../../../../../src/domains/sdf/kernel/SDFPortHole.h \
 ../../../../../src/kernel/DFPortHole.h \
 ../../../../../src/kernel/IntState.h \
 ../../../../../src/kernel/Matrix.h \
 ../../../../../src/kernel/Message.h \
 ../../../../../src/kernel/ComplexArrayState.h \
 ../../../../../src/kernel/FixArrayState.h \
 ../../../../../src/kernel/PrecisionState.h \
 ../../../../../src/kernel/FloatArrayState.h \
 ../../../../../src/kernel/IntArrayState.h
SDFSampleMean.o: SDFSampleMean.cc SDFSampleMean.h \
 ../../../../../src/domains/sdf/kernel/SDFStar.h \
 ../../../../../src/kernel/DataFlowStar.h \
 ../../../../../src/kernel/Star.h ../../../../../src/kernel/Block.h \
 ../../../../../src/kernel/NamedObj.h \
 ../../../../../src/kernel/StringList.h \
 ../../../../../src/kernel/miscFuncs.h \
 ../../../../../src/kernel/logNew.h ../../../../../src/kernel/Error.h \
 ../../../../../src/kernel/DataStruct.h \
 ../../../../../src/kernel/type.h ../../../../../src/kernel/isa.h \
 ../../../../../src/kernel/FlagArray.h \
 ../../../../../src/kernel/PortHole.h \
 ../../../../../src/kernel/dataType.h \
 ../../../../../src/kernel/Particle.h ../../../../../src/kernel/Fix.h \
 ../../../../../src/kernel/ComplexSubset.h \
 ../../../../../src/kernel/Attribute.h \
 ../../../../../src/kernel/State.h \
 ../../../../../src/kernel/InvokeInterp.h \
 ../../../../../src/kernel/Fraction.h \
 ../../../../../src/domains/sdf/kernel/SDFPortHole.h \
 ../../../../../src/kernel/DFPortHole.h \
 ../../../../../src/kernel/Matrix.h \
 ../../../../../src/kernel/Message.h \
 ../../../../../src/kernel/ComplexArrayState.h \
 ../../../../../src/kernel/FixArrayState.h \
 ../../../../../src/kernel/PrecisionState.h \
 ../../../../../src/kernel/FloatArrayState.h \
 ../../../../../src/kernel/IntArrayState.h
SDFSmithForm.o: SDFSmithForm.cc SDFSmithForm.h \
 ../../../../../src/domains/sdf/kernel/SDFStar.h \
 ../../../../../src/kernel/DataFlowStar.h \
 ../../../../../src/kernel/Star.h ../../../../../src/kernel/Block.h \
 ../../../../../src/kernel/NamedObj.h \
 ../../../../../src/kernel/StringList.h \
 ../../../../../src/kernel/miscFuncs.h \
 ../../../../../src/kernel/logNew.h ../../../../../src/kernel/Error.h \
 ../../../../../src/kernel/DataStruct.h \
 ../../../../../src/kernel/type.h ../../../../../src/kernel/isa.h \
 ../../../../../src/kernel/FlagArray.h \
 ../../../../../src/kernel/PortHole.h \
 ../../../../../src/kernel/dataType.h \
 ../../../../../src/kernel/Particle.h ../../../../../src/kernel/Fix.h \
 ../../../../../src/kernel/ComplexSubset.h \
 ../../../../../src/kernel/Attribute.h \
 ../../../../../src/kernel/State.h \
 ../../../../../src/kernel/InvokeInterp.h \
 ../../../../../src/kernel/Fraction.h \
 ../../../../../src/domains/sdf/kernel/SDFPortHole.h \
 ../../../../../src/kernel/DFPortHole.h \
 ../../../../../src/kernel/IntState.h \
 ../../../../../src/kernel/Matrix.h \
 ../../../../../src/kernel/Message.h \
 ../../../../../src/kernel/ComplexArrayState.h \
 ../../../../../src/kernel/FixArrayState.h \
 ../../../../../src/kernel/PrecisionState.h \
 ../../../../../src/kernel/FloatArrayState.h \
 ../../../../../src/kernel/IntArrayState.h \
 ../../../../../src/utils/libptdsp/ptdspSmithForm.h
SDFSubCx_M.o: SDFSubCx_M.cc SDFSubCx_M.h \
 ../../../../../src/domains/sdf/kernel/SDFStar.h \
 ../../../../../src/kernel/DataFlowStar.h \
 ../../../../../src/kernel/Star.h ../../../../../src/kernel/Block.h \
 ../../../../../src/kernel/NamedObj.h \
 ../../../../../src/kernel/StringList.h \
 ../../../../../src/kernel/miscFuncs.h \
 ../../../../../src/kernel/logNew.h ../../../../../src/kernel/Error.h \
 ../../../../../src/kernel/DataStruct.h \
 ../../../../../src/kernel/type.h ../../../../../src/kernel/isa.h \
 ../../../../../src/kernel/FlagArray.h \
 ../../../../../src/kernel/PortHole.h \
 ../../../../../src/kernel/dataType.h \
 ../../../../../src/kernel/Particle.h ../../../../../src/kernel/Fix.h \
 ../../../../../src/kernel/ComplexSubset.h \
 ../../../../../src/kernel/Attribute.h \
 ../../../../../src/kernel/State.h \
 ../../../../../src/kernel/InvokeInterp.h \
 ../../../../../src/kernel/Fraction.h \
 ../../../../../src/domains/sdf/kernel/SDFPortHole.h \
 ../../../../../src/kernel/DFPortHole.h \
 ../../../../../src/kernel/IntState.h \
 ../../../../../src/kernel/Matrix.h \
 ../../../../../src/kernel/Message.h \
 ../../../../../src/kernel/ComplexArrayState.h \
 ../../../../../src/kernel/FixArrayState.h \
 ../../../../../src/kernel/PrecisionState.h \
 ../../../../../src/kernel/FloatArrayState.h \
 ../../../../../src/kernel/IntArrayState.h
SDFSubFix_M.o: SDFSubFix_M.cc SDFSubFix_M.h \
 ../../../../../src/domains/sdf/kernel/SDFStar.h \
 ../../../../../src/kernel/DataFlowStar.h \
 ../../../../../src/kernel/Star.h ../../../../../src/kernel/Block.h \
 ../../../../../src/kernel/NamedObj.h \
 ../../../../../src/kernel/StringList.h \
 ../../../../../src/kernel/miscFuncs.h \
 ../../../../../src/kernel/logNew.h ../../../../../src/kernel/Error.h \
 ../../../../../src/kernel/DataStruct.h \
 ../../../../../src/kernel/type.h ../../../../../src/kernel/isa.h \
 ../../../../../src/kernel/FlagArray.h \
 ../../../../../src/kernel/PortHole.h \
 ../../../../../src/kernel/dataType.h \
 ../../../../../src/kernel/Particle.h ../../../../../src/kernel/Fix.h \
 ../../../../../src/kernel/ComplexSubset.h \
 ../../../../../src/kernel/Attribute.h \
 ../../../../../src/kernel/State.h \
 ../../../../../src/kernel/InvokeInterp.h \
 ../../../../../src/kernel/Fraction.h \
 ../../../../../src/domains/sdf/kernel/SDFPortHole.h \
 ../../../../../src/kernel/DFPortHole.h \
 ../../../../../src/kernel/IntState.h \
 ../../../../../src/kernel/StringState.h \
 ../../../../../src/kernel/KnownState.h \
 ../../../../../src/kernel/Matrix.h \
 ../../../../../src/kernel/Message.h \
 ../../../../../src/kernel/ComplexArrayState.h \
 ../../../../../src/kernel/FixArrayState.h \
 ../../../../../src/kernel/PrecisionState.h \
 ../../../../../src/kernel/FloatArrayState.h \
 ../../../../../src/kernel/IntArrayState.h
SDFSubInt_M.o: SDFSubInt_M.cc SDFSubInt_M.h \
 ../../../../../src/domains/sdf/kernel/SDFStar.h \
 ../../../../../src/kernel/DataFlowStar.h \
 ../../../../../src/kernel/Star.h ../../../../../src/kernel/Block.h \
 ../../../../../src/kernel/NamedObj.h \
 ../../../../../src/kernel/StringList.h \
 ../../../../../src/kernel/miscFuncs.h \
 ../../../../../src/kernel/logNew.h ../../../../../src/kernel/Error.h \
 ../../../../../src/kernel/DataStruct.h \
 ../../../../../src/kernel/type.h ../../../../../src/kernel/isa.h \
 ../../../../../src/kernel/FlagArray.h \
 ../../../../../src/kernel/PortHole.h \
 ../../../../../src/kernel/dataType.h \
 ../../../../../src/kernel/Particle.h ../../../../../src/kernel/Fix.h \
 ../../../../../src/kernel/ComplexSubset.h \
 ../../../../../src/kernel/Attribute.h \
 ../../../../../src/kernel/State.h \
 ../../../../../src/kernel/InvokeInterp.h \
 ../../../../../src/kernel/Fraction.h \
 ../../../../../src/domains/sdf/kernel/SDFPortHole.h \
 ../../../../../src/kernel/DFPortHole.h \
 ../../../../../src/kernel/IntState.h \
 ../../../../../src/kernel/Matrix.h \
 ../../../../../src/kernel/Message.h \
 ../../../../../src/kernel/ComplexArrayState.h \
 ../../../../../src/kernel/FixArrayState.h \
 ../../../../../src/kernel/PrecisionState.h \
 ../../../../../src/kernel/FloatArrayState.h \
 ../../../../../src/kernel/IntArrayState.h
SDFSubMxCx_M.o: SDFSubMxCx_M.cc SDFSubMxCx_M.h \
 ../../../../../src/domains/sdf/kernel/SDFStar.h \
 ../../../../../src/kernel/DataFlowStar.h \
 ../../../../../src/kernel/Star.h ../../../../../src/kernel/Block.h \
 ../../../../../src/kernel/NamedObj.h \
 ../../../../../src/kernel/StringList.h \
 ../../../../../src/kernel/miscFuncs.h \
 ../../../../../src/kernel/logNew.h ../../../../../src/kernel/Error.h \
 ../../../../../src/kernel/DataStruct.h \
 ../../../../../src/kernel/type.h ../../../../../src/kernel/isa.h \
 ../../../../../src/kernel/FlagArray.h \
 ../../../../../src/kernel/PortHole.h \
 ../../../../../src/kernel/dataType.h \
 ../../../../../src/kernel/Particle.h ../../../../../src/kernel/Fix.h \
 ../../../../../src/kernel/ComplexSubset.h \
 ../../../../../src/kernel/Attribute.h \
 ../../../../../src/kernel/State.h \
 ../../../../../src/kernel/InvokeInterp.h \
 ../../../../../src/kernel/Fraction.h \
 ../../../../../src/domains/sdf/kernel/SDFPortHole.h \
 ../../../../../src/kernel/DFPortHole.h \
 ../../../../../src/kernel/IntState.h \
 ../../../../../src/kernel/Matrix.h \
 ../../../../../src/kernel/Message.h \
 ../../../../../src/kernel/ComplexArrayState.h \
 ../../../../../src/kernel/FixArrayState.h \
 ../../../../../src/kernel/PrecisionState.h \
 ../../../../../src/kernel/FloatArrayState.h \
 ../../../../../src/kernel/IntArrayState.h
SDFSubMxFix_M.o: SDFSubMxFix_M.cc SDFSubMxFix_M.h \
 ../../../../../src/domains/sdf/kernel/SDFStar.h \
 ../../../../../src/kernel/DataFlowStar.h \
 ../../../../../src/kernel/Star.h ../../../../../src/kernel/Block.h \
 ../../../../../src/kernel/NamedObj.h \
 ../../../../../src/kernel/StringList.h \
 ../../../../../src/kernel/miscFuncs.h \
 ../../../../../src/kernel/logNew.h ../../../../../src/kernel/Error.h \
 ../../../../../src/kernel/DataStruct.h \
 ../../../../../src/kernel/type.h ../../../../../src/kernel/isa.h \
 ../../../../../src/kernel/FlagArray.h \
 ../../../../../src/kernel/PortHole.h \
 ../../../../../src/kernel/dataType.h \
 ../../../../../src/kernel/Particle.h ../../../../../src/kernel/Fix.h \
 ../../../../../src/kernel/ComplexSubset.h \
 ../../../../../src/kernel/Attribute.h \
 ../../../../../src/kernel/State.h \
 ../../../../../src/kernel/InvokeInterp.h \
 ../../../../../src/kernel/Fraction.h \
 ../../../../../src/domains/sdf/kernel/SDFPortHole.h \
 ../../../../../src/kernel/DFPortHole.h \
 ../../../../../src/kernel/IntState.h \
 ../../../../../src/kernel/StringState.h \
 ../../../../../src/kernel/KnownState.h \
 ../../../../../src/kernel/Matrix.h \
 ../../../../../src/kernel/Message.h \
 ../../../../../src/kernel/ComplexArrayState.h \
 ../../../../../src/kernel/FixArrayState.h \
 ../../../../../src/kernel/PrecisionState.h \
 ../../../../../src/kernel/FloatArrayState.h \
 ../../../../../src/kernel/IntArrayState.h
SDFSubMxInt_M.o: SDFSubMxInt_M.cc SDFSubMxInt_M.h \
 ../../../../../src/domains/sdf/kernel/SDFStar.h \
 ../../../../../src/kernel/DataFlowStar.h \
 ../../../../../src/kernel/Star.h ../../../../../src/kernel/Block.h \
 ../../../../../src/kernel/NamedObj.h \
 ../../../../../src/kernel/StringList.h \
 ../../../../../src/kernel/miscFuncs.h \
 ../../../../../src/kernel/logNew.h ../../../../../src/kernel/Error.h \
 ../../../../../src/kernel/DataStruct.h \
 ../../../../../src/kernel/type.h ../../../../../src/kernel/isa.h \
 ../../../../../src/kernel/FlagArray.h \
 ../../../../../src/kernel/PortHole.h \
 ../../../../../src/kernel/dataType.h \
 ../../../../../src/kernel/Particle.h ../../../../../src/kernel/Fix.h \
 ../../../../../src/kernel/ComplexSubset.h \
 ../../../../../src/kernel/Attribute.h \
 ../../../../../src/kernel/State.h \
 ../../../../../src/kernel/InvokeInterp.h \
 ../../../../../src/kernel/Fraction.h \
 ../../../../../src/domains/sdf/kernel/SDFPortHole.h \
 ../../../../../src/kernel/DFPortHole.h \
 ../../../../../src/kernel/IntState.h \
 ../../../../../src/kernel/Matrix.h \
 ../../../../../src/kernel/Message.h \
 ../../../../../src/kernel/ComplexArrayState.h \
 ../../../../../src/kernel/FixArrayState.h \
 ../../../../../src/kernel/PrecisionState.h \
 ../../../../../src/kernel/FloatArrayState.h \
 ../../../../../src/kernel/IntArrayState.h
SDFSubMx_M.o: SDFSubMx_M.cc SDFSubMx_M.h \
 ../../../../../src/domains/sdf/kernel/SDFStar.h \
 ../../../../../src/kernel/DataFlowStar.h \
 ../../../../../src/kernel/Star.h ../../../../../src/kernel/Block.h \
 ../../../../../src/kernel/NamedObj.h \
 ../../../../../src/kernel/StringList.h \
 ../../../../../src/kernel/miscFuncs.h \
 ../../../../../src/kernel/logNew.h ../../../../../src/kernel/Error.h \
 ../../../../../src/kernel/DataStruct.h \
 ../../../../../src/kernel/type.h ../../../../../src/kernel/isa.h \
 ../../../../../src/kernel/FlagArray.h \
 ../../../../../src/kernel/PortHole.h \
 ../../../../../src/kernel/dataType.h \
 ../../../../../src/kernel/Particle.h ../../../../../src/kernel/Fix.h \
 ../../../../../src/kernel/ComplexSubset.h \
 ../../../../../src/kernel/Attribute.h \
 ../../../../../src/kernel/State.h \
 ../../../../../src/kernel/InvokeInterp.h \
 ../../../../../src/kernel/Fraction.h \
 ../../../../../src/domains/sdf/kernel/SDFPortHole.h \
 ../../../../../src/kernel/DFPortHole.h \
 ../../../../../src/kernel/IntState.h \
 ../../../../../src/kernel/Matrix.h \
 ../../../../../src/kernel/Message.h \
 ../../../../../src/kernel/ComplexArrayState.h \
 ../../../../../src/kernel/FixArrayState.h \
 ../../../../../src/kernel/PrecisionState.h \
 ../../../../../src/kernel/FloatArrayState.h \
 ../../../../../src/kernel/IntArrayState.h
SDFSub_M.o: SDFSub_M.cc SDFSub_M.h \
 ../../../../../src/domains/sdf/kernel/SDFStar.h \
 ../../../../../src/kernel/DataFlowStar.h \
 ../../../../../src/kernel/Star.h ../../../../../src/kernel/Block.h \
 ../../../../../src/kernel/NamedObj.h \
 ../../../../../src/kernel/StringList.h \
 ../../../../../src/kernel/miscFuncs.h \
 ../../../../../src/kernel/logNew.h ../../../../../src/kernel/Error.h \
 ../../../../../src/kernel/DataStruct.h \
 ../../../../../src/kernel/type.h ../../../../../src/kernel/isa.h \
 ../../../../../src/kernel/FlagArray.h \
 ../../../../../src/kernel/PortHole.h \
 ../../../../../src/kernel/dataType.h \
 ../../../../../src/kernel/Particle.h ../../../../../src/kernel/Fix.h \
 ../../../../../src/kernel/ComplexSubset.h \
 ../../../../../src/kernel/Attribute.h \
 ../../../../../src/kernel/State.h \
 ../../../../../src/kernel/InvokeInterp.h \
 ../../../../../src/kernel/Fraction.h \
 ../../../../../src/domains/sdf/kernel/SDFPortHole.h \
 ../../../../../src/kernel/DFPortHole.h \
 ../../../../../src/kernel/IntState.h \
 ../../../../../src/kernel/Matrix.h \
 ../../../../../src/kernel/Message.h \
 ../../../../../src/kernel/ComplexArrayState.h \
 ../../../../../src/kernel/FixArrayState.h \
 ../../../../../src/kernel/PrecisionState.h \
 ../../../../../src/kernel/FloatArrayState.h \
 ../../../../../src/kernel/IntArrayState.h
SDFTableCx_M.o: SDFTableCx_M.cc SDFTableCx_M.h \
 ../../../../../src/domains/sdf/kernel/SDFStar.h \
 ../../../../../src/kernel/DataFlowStar.h \
 ../../../../../src/kernel/Star.h ../../../../../src/kernel/Block.h \
 ../../../../../src/kernel/NamedObj.h \
 ../../../../../src/kernel/StringList.h \
 ../../../../../src/kernel/miscFuncs.h \
 ../../../../../src/kernel/logNew.h ../../../../../src/kernel/Error.h \
 ../../../../../src/kernel/DataStruct.h \
 ../../../../../src/kernel/type.h ../../../../../src/kernel/isa.h \
 ../../../../../src/kernel/FlagArray.h \
 ../../../../../src/kernel/PortHole.h \
 ../../../../../src/kernel/dataType.h \
 ../../../../../src/kernel/Particle.h ../../../../../src/kernel/Fix.h \
 ../../../../../src/kernel/ComplexSubset.h \
 ../../../../../src/kernel/Attribute.h \
 ../../../../../src/kernel/State.h \
 ../../../../../src/kernel/InvokeInterp.h \
 ../../../../../src/kernel/Fraction.h \
 ../../../../../src/domains/sdf/kernel/SDFPortHole.h \
 ../../../../../src/kernel/DFPortHole.h \
 ../../../../../src/kernel/IntState.h \
 ../../../../../src/kernel/ComplexArrayState.h \
 ../../../../../src/kernel/Matrix.h \
 ../../../../../src/kernel/Message.h \
 ../../../../../src/kernel/FixArrayState.h \
 ../../../../../src/kernel/PrecisionState.h \
 ../../../../../src/kernel/FloatArrayState.h \
 ../../../../../src/kernel/IntArrayState.h
SDFTableInt_M.o: SDFTableInt_M.cc SDFTableInt_M.h \
 ../../../../../src/domains/sdf/kernel/SDFStar.h \
 ../../../../../src/kernel/DataFlowStar.h \
 ../../../../../src/kernel/Star.h ../../../../../src/kernel/Block.h \
 ../../../../../src/kernel/NamedObj.h \
 ../../../../../src/kernel/StringList.h \
 ../../../../../src/kernel/miscFuncs.h \
 ../../../../../src/kernel/logNew.h ../../../../../src/kernel/Error.h \
 ../../../../../src/kernel/DataStruct.h \
 ../../../../../src/kernel/type.h ../../../../../src/kernel/isa.h \
 ../../../../../src/kernel/FlagArray.h \
 ../../../../../src/kernel/PortHole.h \
 ../../../../../src/kernel/dataType.h \
 ../../../../../src/kernel/Particle.h ../../../../../src/kernel/Fix.h \
 ../../../../../src/kernel/ComplexSubset.h \
 ../../../../../src/kernel/Attribute.h \
 ../../../../../src/kernel/State.h \
 ../../../../../src/kernel/InvokeInterp.h \
 ../../../../../src/kernel/Fraction.h \
 ../../../../../src/domains/sdf/kernel/SDFPortHole.h \
 ../../../../../src/kernel/DFPortHole.h \
 ../../../../../src/kernel/IntState.h \
 ../../../../../src/kernel/IntArrayState.h \
 ../../../../../src/kernel/Matrix.h \
 ../../../../../src/kernel/Message.h \
 ../../../../../src/kernel/ComplexArrayState.h \
 ../../../../../src/kernel/FixArrayState.h \
 ../../../../../src/kernel/PrecisionState.h \
 ../../../../../src/kernel/FloatArrayState.h
SDFTable_M.o: SDFTable_M.cc SDFTable_M.h \
 ../../../../../src/domains/sdf/kernel/SDFStar.h \
 ../../../../../src/kernel/DataFlowStar.h \
 ../../../../../src/kernel/Star.h ../../../../../src/kernel/Block.h \
 ../../../../../src/kernel/NamedObj.h \
 ../../../../../src/kernel/StringList.h \
 ../../../../../src/kernel/miscFuncs.h \
 ../../../../../src/kernel/logNew.h ../../../../../src/kernel/Error.h \
 ../../../../../src/kernel/DataStruct.h \
 ../../../../../src/kernel/type.h ../../../../../src/kernel/isa.h \
 ../../../../../src/kernel/FlagArray.h \
 ../../../../../src/kernel/PortHole.h \
 ../../../../../src/kernel/dataType.h \
 ../../../../../src/kernel/Particle.h ../../../../../src/kernel/Fix.h \
 ../../../../../src/kernel/ComplexSubset.h \
 ../../../../../src/kernel/Attribute.h \
 ../../../../../src/kernel/State.h \
 ../../../../../src/kernel/InvokeInterp.h \
 ../../../../../src/kernel/Fraction.h \
 ../../../../../src/domains/sdf/kernel/SDFPortHole.h \
 ../../../../../src/kernel/DFPortHole.h \
 ../../../../../src/kernel/IntState.h \
 ../../../../../src/kernel/FloatArrayState.h \
 ../../../../../src/kernel/Matrix.h \
 ../../../../../src/kernel/Message.h \
 ../../../../../src/kernel/ComplexArrayState.h \
 ../../../../../src/kernel/FixArrayState.h \
 ../../../../../src/kernel/PrecisionState.h \
 ../../../../../src/kernel/IntArrayState.h
SDFToeplitzCx_M.o: SDFToeplitzCx_M.cc SDFToeplitzCx_M.h \
 ../../../../../src/kernel/Matrix.h \
 ../../../../../src/kernel/Message.h \
 ../../../../../src/kernel/Particle.h \
 ../../../../../src/kernel/StringList.h \
 ../../../../../src/kernel/miscFuncs.h \
 ../../../../../src/kernel/logNew.h ../../../../../src/kernel/Error.h \
 ../../../../../src/kernel/DataStruct.h \
 ../../../../../src/kernel/type.h ../../../../../src/kernel/dataType.h \
 ../../../../../src/kernel/Fix.h \
 ../../../../../src/kernel/ComplexSubset.h \
 ../../../../../src/kernel/isa.h \
 ../../../../../src/kernel/ComplexArrayState.h \
 ../../../../../src/kernel/State.h \
 ../../../../../src/kernel/InvokeInterp.h \
 ../../../../../src/kernel/NamedObj.h \
 ../../../../../src/kernel/FlagArray.h \
 ../../../../../src/kernel/Attribute.h \
 ../../../../../src/kernel/FixArrayState.h \
 ../../../../../src/kernel/PrecisionState.h \
 ../../../../../src/kernel/FloatArrayState.h \
 ../../../../../src/kernel/IntArrayState.h \
 ../../../../../src/kernel/PortHole.h \
 ../../../../../src/domains/sdf/kernel/SDFStar.h \
 ../../../../../src/kernel/DataFlowStar.h \
 ../../../../../src/kernel/Star.h ../../../../../src/kernel/Block.h \
 ../../../../../src/kernel/Fraction.h \
 ../../../../../src/domains/sdf/kernel/SDFPortHole.h \
 ../../../../../src/kernel/DFPortHole.h \
 ../../../../../src/kernel/IntState.h
SDFToeplitzFix_M.o: SDFToeplitzFix_M.cc SDFToeplitzFix_M.h \
 ../../../../../src/kernel/Matrix.h \
 ../../../../../src/kernel/Message.h \
 ../../../../../src/kernel/Particle.h \
 ../../../../../src/kernel/StringList.h \
 ../../../../../src/kernel/miscFuncs.h \
 ../../../../../src/kernel/logNew.h ../../../../../src/kernel/Error.h \
 ../../../../../src/kernel/DataStruct.h \
 ../../../../../src/kernel/type.h ../../../../../src/kernel/dataType.h \
 ../../../../../src/kernel/Fix.h \
 ../../../../../src/kernel/ComplexSubset.h \
 ../../../../../src/kernel/isa.h \
 ../../../../../src/kernel/ComplexArrayState.h \
 ../../../../../src/kernel/State.h \
 ../../../../../src/kernel/InvokeInterp.h \
 ../../../../../src/kernel/NamedObj.h \
 ../../../../../src/kernel/FlagArray.h \
 ../../../../../src/kernel/Attribute.h \
 ../../../../../src/kernel/FixArrayState.h \
 ../../../../../src/kernel/PrecisionState.h \
 ../../../../../src/kernel/FloatArrayState.h \
 ../../../../../src/kernel/IntArrayState.h \
 ../../../../../src/kernel/PortHole.h \
 ../../../../../src/domains/sdf/kernel/SDFStar.h \
 ../../../../../src/kernel/DataFlowStar.h \
 ../../../../../src/kernel/Star.h ../../../../../src/kernel/Block.h \
 ../../../../../src/kernel/Fraction.h \
 ../../../../../src/domains/sdf/kernel/SDFPortHole.h \
 ../../../../../src/kernel/DFPortHole.h \
 ../../../../../src/kernel/IntState.h
SDFToeplitzInt_M.o: SDFToeplitzInt_M.cc SDFToeplitzInt_M.h \
 ../../../../../src/kernel/Matrix.h \
 ../../../../../src/kernel/Message.h \
 ../../../../../src/kernel/Particle.h \
 ../../../../../src/kernel/StringList.h \
 ../../../../../src/kernel/miscFuncs.h \
 ../../../../../src/kernel/logNew.h ../../../../../src/kernel/Error.h \
 ../../../../../src/kernel/DataStruct.h \
 ../../../../../src/kernel/type.h ../../../../../src/kernel/dataType.h \
 ../../../../../src/kernel/Fix.h \
 ../../../../../src/kernel/ComplexSubset.h \
 ../../../../../src/kernel/isa.h \
 ../../../../../src/kernel/ComplexArrayState.h \
 ../../../../../src/kernel/State.h \
 ../../../../../src/kernel/InvokeInterp.h \
 ../../../../../src/kernel/NamedObj.h \
 ../../../../../src/kernel/FlagArray.h \
 ../../../../../src/kernel/Attribute.h \
 ../../../../../src/kernel/FixArrayState.h \
 ../../../../../src/kernel/PrecisionState.h \
 ../../../../../src/kernel/FloatArrayState.h \
 ../../../../../src/kernel/IntArrayState.h \
 ../../../../../src/kernel/PortHole.h \
 ../../../../../src/domains/sdf/kernel/SDFStar.h \
 ../../../../../src/kernel/DataFlowStar.h \
 ../../../../../src/kernel/Star.h ../../../../../src/kernel/Block.h \
 ../../../../../src/kernel/Fraction.h \
 ../../../../../src/domains/sdf/kernel/SDFPortHole.h \
 ../../../../../src/kernel/DFPortHole.h \
 ../../../../../src/kernel/IntState.h
SDFToeplitz_M.o: SDFToeplitz_M.cc SDFToeplitz_M.h \
 ../../../../../src/kernel/Matrix.h \
 ../../../../../src/kernel/Message.h \
 ../../../../../src/kernel/Particle.h \
 ../../../../../src/kernel/StringList.h \
 ../../../../../src/kernel/miscFuncs.h \
 ../../../../../src/kernel/logNew.h ../../../../../src/kernel/Error.h \
 ../../../../../src/kernel/DataStruct.h \
 ../../../../../src/kernel/type.h ../../../../../src/kernel/dataType.h \
 ../../../../../src/kernel/Fix.h \
 ../../../../../src/kernel/ComplexSubset.h \
 ../../../../../src/kernel/isa.h \
 ../../../../../src/kernel/ComplexArrayState.h \
 ../../../../../src/kernel/State.h \
 ../../../../../src/kernel/InvokeInterp.h \
 ../../../../../src/kernel/NamedObj.h \
 ../../../../../src/kernel/FlagArray.h \
 ../../../../../src/kernel/Attribute.h \
 ../../../../../src/kernel/FixArrayState.h \
 ../../../../../src/kernel/PrecisionState.h \
 ../../../../../src/kernel/FloatArrayState.h \
 ../../../../../src/kernel/IntArrayState.h \
 ../../../../../src/kernel/PortHole.h \
 ../../../../../src/domains/sdf/kernel/SDFStar.h \
 ../../../../../src/kernel/DataFlowStar.h \
 ../../../../../src/kernel/Star.h ../../../../../src/kernel/Block.h \
 ../../../../../src/kernel/Fraction.h \
 ../../../../../src/domains/sdf/kernel/SDFPortHole.h \
 ../../../../../src/kernel/DFPortHole.h \
 ../../../../../src/kernel/IntState.h
SDFTransposeCx_M.o: SDFTransposeCx_M.cc SDFTransposeCx_M.h \
 ../../../../../src/domains/sdf/kernel/SDFStar.h \
 ../../../../../src/kernel/DataFlowStar.h \
 ../../../../../src/kernel/Star.h ../../../../../src/kernel/Block.h \
 ../../../../../src/kernel/NamedObj.h \
 ../../../../../src/kernel/StringList.h \
 ../../../../../src/kernel/miscFuncs.h \
 ../../../../../src/kernel/logNew.h ../../../../../src/kernel/Error.h \
 ../../../../../src/kernel/DataStruct.h \
 ../../../../../src/kernel/type.h ../../../../../src/kernel/isa.h \
 ../../../../../src/kernel/FlagArray.h \
 ../../../../../src/kernel/PortHole.h \
 ../../../../../src/kernel/dataType.h \
 ../../../../../src/kernel/Particle.h ../../../../../src/kernel/Fix.h \
 ../../../../../src/kernel/ComplexSubset.h \
 ../../../../../src/kernel/Attribute.h \
 ../../../../../src/kernel/State.h \
 ../../../../../src/kernel/InvokeInterp.h \
 ../../../../../src/kernel/Fraction.h \
 ../../../../../src/domains/sdf/kernel/SDFPortHole.h \
 ../../../../../src/kernel/DFPortHole.h \
 ../../../../../src/kernel/IntState.h \
 ../../../../../src/kernel/Matrix.h \
 ../../../../../src/kernel/Message.h \
 ../../../../../src/kernel/ComplexArrayState.h \
 ../../../../../src/kernel/FixArrayState.h \
 ../../../../../src/kernel/PrecisionState.h \
 ../../../../../src/kernel/FloatArrayState.h \
 ../../../../../src/kernel/IntArrayState.h
SDFTransposeFix_M.o: SDFTransposeFix_M.cc SDFTransposeFix_M.h \
 ../../../../../src/domains/sdf/kernel/SDFStar.h \
 ../../../../../src/kernel/DataFlowStar.h \
 ../../../../../src/kernel/Star.h ../../../../../src/kernel/Block.h \
 ../../../../../src/kernel/NamedObj.h \
 ../../../../../src/kernel/StringList.h \
 ../../../../../src/kernel/miscFuncs.h \
 ../../../../../src/kernel/logNew.h ../../../../../src/kernel/Error.h \
 ../../../../../src/kernel/DataStruct.h \
 ../../../../../src/kernel/type.h ../../../../../src/kernel/isa.h \
 ../../../../../src/kernel/FlagArray.h \
 ../../../../../src/kernel/PortHole.h \
 ../../../../../src/kernel/dataType.h \
 ../../../../../src/kernel/Particle.h ../../../../../src/kernel/Fix.h \
 ../../../../../src/kernel/ComplexSubset.h \
 ../../../../../src/kernel/Attribute.h \
 ../../../../../src/kernel/State.h \
 ../../../../../src/kernel/InvokeInterp.h \
 ../../../../../src/kernel/Fraction.h \
 ../../../../../src/domains/sdf/kernel/SDFPortHole.h \
 ../../../../../src/kernel/DFPortHole.h \
 ../../../../../src/kernel/IntState.h \
 ../../../../../src/kernel/Matrix.h \
 ../../../../../src/kernel/Message.h \
 ../../../../../src/kernel/ComplexArrayState.h \
 ../../../../../src/kernel/FixArrayState.h \
 ../../../../../src/kernel/PrecisionState.h \
 ../../../../../src/kernel/FloatArrayState.h \
 ../../../../../src/kernel/IntArrayState.h
SDFTransposeInt_M.o: SDFTransposeInt_M.cc SDFTransposeInt_M.h \
 ../../../../../src/domains/sdf/kernel/SDFStar.h \
 ../../../../../src/kernel/DataFlowStar.h \
 ../../../../../src/kernel/Star.h ../../../../../src/kernel/Block.h \
 ../../../../../src/kernel/NamedObj.h \
 ../../../../../src/kernel/StringList.h \
 ../../../../../src/kernel/miscFuncs.h \
 ../../../../../src/kernel/logNew.h ../../../../../src/kernel/Error.h \
 ../../../../../src/kernel/DataStruct.h \
 ../../../../../src/kernel/type.h ../../../../../src/kernel/isa.h \
 ../../../../../src/kernel/FlagArray.h \
 ../../../../../src/kernel/PortHole.h \
 ../../../../../src/kernel/dataType.h \
 ../../../../../src/kernel/Particle.h ../../../../../src/kernel/Fix.h \
 ../../../../../src/kernel/ComplexSubset.h \
 ../../../../../src/kernel/Attribute.h \
 ../../../../../src/kernel/State.h \
 ../../../../../src/kernel/InvokeInterp.h \
 ../../../../../src/kernel/Fraction.h \
 ../../../../../src/domains/sdf/kernel/SDFPortHole.h \
 ../../../../../src/kernel/DFPortHole.h \
 ../../../../../src/kernel/IntState.h \
 ../../../../../src/kernel/Matrix.h \
 ../../../../../src/kernel/Message.h \
 ../../../../../src/kernel/ComplexArrayState.h \
 ../../../../../src/kernel/FixArrayState.h \
 ../../../../../src/kernel/PrecisionState.h \
 ../../../../../src/kernel/FloatArrayState.h \
 ../../../../../src/kernel/IntArrayState.h
SDFTranspose_M.o: SDFTranspose_M.cc SDFTranspose_M.h \
 ../../../../../src/domains/sdf/kernel/SDFStar.h \
 ../../../../../src/kernel/DataFlowStar.h \
 ../../../../../src/kernel/Star.h ../../../../../src/kernel/Block.h \
 ../../../../../src/kernel/NamedObj.h \
 ../../../../../src/kernel/StringList.h \
 ../../../../../src/kernel/miscFuncs.h \
 ../../../../../src/kernel/logNew.h ../../../../../src/kernel/Error.h \
 ../../../../../src/kernel/DataStruct.h \
 ../../../../../src/kernel/type.h ../../../../../src/kernel/isa.h \
 ../../../../../src/kernel/FlagArray.h \
 ../../../../../src/kernel/PortHole.h \
 ../../../../../src/kernel/dataType.h \
 ../../../../../src/kernel/Particle.h ../../../../../src/kernel/Fix.h \
 ../../../../../src/kernel/ComplexSubset.h \
 ../../../../../src/kernel/Attribute.h \
 ../../../../../src/kernel/State.h \
 ../../../../../src/kernel/InvokeInterp.h \
 ../../../../../src/kernel/Fraction.h \
 ../../../../../src/domains/sdf/kernel/SDFPortHole.h \
 ../../../../../src/kernel/DFPortHole.h \
 ../../../../../src/kernel/IntState.h \
 ../../../../../src/kernel/Matrix.h \
 ../../../../../src/kernel/Message.h \
 ../../../../../src/kernel/ComplexArrayState.h \
 ../../../../../src/kernel/FixArrayState.h \
 ../../../../../src/kernel/PrecisionState.h \
 ../../../../../src/kernel/FloatArrayState.h \
 ../../../../../src/kernel/IntArrayState.h
SDFUnPkCx_M.o: SDFUnPkCx_M.cc SDFUnPkCx_M.h \
 ../../../../../src/domains/sdf/kernel/SDFStar.h \
 ../../../../../src/kernel/DataFlowStar.h \
 ../../../../../src/kernel/Star.h ../../../../../src/kernel/Block.h \
 ../../../../../src/kernel/NamedObj.h \
 ../../../../../src/kernel/StringList.h \
 ../../../../../src/kernel/miscFuncs.h \
 ../../../../../src/kernel/logNew.h ../../../../../src/kernel/Error.h \
 ../../../../../src/kernel/DataStruct.h \
 ../../../../../src/kernel/type.h ../../../../../src/kernel/isa.h \
 ../../../../../src/kernel/FlagArray.h \
 ../../../../../src/kernel/PortHole.h \
 ../../../../../src/kernel/dataType.h \
 ../../../../../src/kernel/Particle.h ../../../../../src/kernel/Fix.h \
 ../../../../../src/kernel/ComplexSubset.h \
 ../../../../../src/kernel/Attribute.h \
 ../../../../../src/kernel/State.h \
 ../../../../../src/kernel/InvokeInterp.h \
 ../../../../../src/kernel/Fraction.h \
 ../../../../../src/domains/sdf/kernel/SDFPortHole.h \
 ../../../../../src/kernel/DFPortHole.h \
 ../../../../../src/kernel/IntState.h \
 ../../../../../src/kernel/Matrix.h \
 ../../../../../src/kernel/Message.h \
 ../../../../../src/kernel/ComplexArrayState.h \
 ../../../../../src/kernel/FixArrayState.h \
 ../../../../../src/kernel/PrecisionState.h \
 ../../../../../src/kernel/FloatArrayState.h \
 ../../../../../src/kernel/IntArrayState.h
SDFUnPkFix_M.o: SDFUnPkFix_M.cc SDFUnPkFix_M.h \
 ../../../../../src/domains/sdf/kernel/SDFStar.h \
 ../../../../../src/kernel/DataFlowStar.h \
 ../../../../../src/kernel/Star.h ../../../../../src/kernel/Block.h \
 ../../../../../src/kernel/NamedObj.h \
 ../../../../../src/kernel/StringList.h \
 ../../../../../src/kernel/miscFuncs.h \
 ../../../../../src/kernel/logNew.h ../../../../../src/kernel/Error.h \
 ../../../../../src/kernel/DataStruct.h \
 ../../../../../src/kernel/type.h ../../../../../src/kernel/isa.h \
 ../../../../../src/kernel/FlagArray.h \
 ../../../../../src/kernel/PortHole.h \
 ../../../../../src/kernel/dataType.h \
 ../../../../../src/kernel/Particle.h ../../../../../src/kernel/Fix.h \
 ../../../../../src/kernel/ComplexSubset.h \
 ../../../../../src/kernel/Attribute.h \
 ../../../../../src/kernel/State.h \
 ../../../../../src/kernel/InvokeInterp.h \
 ../../../../../src/kernel/Fraction.h \
 ../../../../../src/domains/sdf/kernel/SDFPortHole.h \
 ../../../../../src/kernel/DFPortHole.h \
 ../../../../../src/kernel/IntState.h \
 ../../../../../src/kernel/Matrix.h \
 ../../../../../src/kernel/Message.h \
 ../../../../../src/kernel/ComplexArrayState.h \
 ../../../../../src/kernel/FixArrayState.h \
 ../../../../../src/kernel/PrecisionState.h \
 ../../../../../src/kernel/FloatArrayState.h \
 ../../../../../src/kernel/IntArrayState.h
SDFUnPkInt_M.o: SDFUnPkInt_M.cc SDFUnPkInt_M.h \
 ../../../../../src/domains/sdf/kernel/SDFStar.h \
 ../../../../../src/kernel/DataFlowStar.h \
 ../../../../../src/kernel/Star.h ../../../../../src/kernel/Block.h \
 ../../../../../src/kernel/NamedObj.h \
 ../../../../../src/kernel/StringList.h \
 ../../../../../src/kernel/miscFuncs.h \
 ../../../../../src/kernel/logNew.h ../../../../../src/kernel/Error.h \
 ../../../../../src/kernel/DataStruct.h \
 ../../../../../src/kernel/type.h ../../../../../src/kernel/isa.h \
 ../../../../../src/kernel/FlagArray.h \
 ../../../../../src/kernel/PortHole.h \
 ../../../../../src/kernel/dataType.h \
 ../../../../../src/kernel/Particle.h ../../../../../src/kernel/Fix.h \
 ../../../../../src/kernel/ComplexSubset.h \
 ../../../../../src/kernel/Attribute.h \
 ../../../../../src/kernel/State.h \
 ../../../../../src/kernel/InvokeInterp.h \
 ../../../../../src/kernel/Fraction.h \
 ../../../../../src/domains/sdf/kernel/SDFPortHole.h \
 ../../../../../src/kernel/DFPortHole.h \
 ../../../../../src/kernel/IntState.h \
 ../../../../../src/kernel/Matrix.h \
 ../../../../../src/kernel/Message.h \
 ../../../../../src/kernel/ComplexArrayState.h \
 ../../../../../src/kernel/FixArrayState.h \
 ../../../../../src/kernel/PrecisionState.h \
 ../../../../../src/kernel/FloatArrayState.h \
 ../../../../../src/kernel/IntArrayState.h
SDFUnPk_M.o: SDFUnPk_M.cc SDFUnPk_M.h \
 ../../../../../src/domains/sdf/kernel/SDFStar.h \
 ../../../../../src/kernel/DataFlowStar.h \
 ../../../../../src/kernel/Star.h ../../../../../src/kernel/Block.h \
 ../../../../../src/kernel/NamedObj.h \
 ../../../../../src/kernel/StringList.h \
 ../../../../../src/kernel/miscFuncs.h \
 ../../../../../src/kernel/logNew.h ../../../../../src/kernel/Error.h \
 ../../../../../src/kernel/DataStruct.h \
 ../../../../../src/kernel/type.h ../../../../../src/kernel/isa.h \
 ../../../../../src/kernel/FlagArray.h \
 ../../../../../src/kernel/PortHole.h \
 ../../../../../src/kernel/dataType.h \
 ../../../../../src/kernel/Particle.h ../../../../../src/kernel/Fix.h \
 ../../../../../src/kernel/ComplexSubset.h \
 ../../../../../src/kernel/Attribute.h \
 ../../../../../src/kernel/State.h \
 ../../../../../src/kernel/InvokeInterp.h \
 ../../../../../src/kernel/Fraction.h \
 ../../../../../src/domains/sdf/kernel/SDFPortHole.h \
 ../../../../../src/kernel/DFPortHole.h \
 ../../../../../src/kernel/IntState.h \
 ../../../../../src/kernel/Matrix.h \
 ../../../../../src/kernel/Message.h \
 ../../../../../src/kernel/ComplexArrayState.h \
 ../../../../../src/kernel/FixArrayState.h \
 ../../../../../src/kernel/PrecisionState.h \
 ../../../../../src/kernel/FloatArrayState.h \
 ../../../../../src/kernel/IntArrayState.h
