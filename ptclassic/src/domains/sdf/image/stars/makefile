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
# Makefile for Ptolemy SDF image and network stars
# @(#)make.template	1.21	03/24/97
#
# If this file's name is "make.template" and there is no makefile in
# this directory, do
#	cp make.template makefile
#	chmod +w makefile
#	make depend
# This will add the dependencies to the makefile.

ROOT = ../../../../..
VPATH = $(ROOT)/src/domains/sdf/image/stars

# get configuration info
CONFIG=$(ROOT)/mk/config-$(PTARCH).mk
include $(CONFIG)

# SDFDIR is the directory containing SDF domain support
SDFDIR = $(ROOT)/src/domains/sdf/kernel
INCL = -I$(SDFDIR) -I$(KERNDIR) -I$(ROOT)/src/domains/sdf/stars \
       -I$(ROOT)/src/utils/libptdsp

# Library name
PTLIB=libsdfimagestars.$(LIBSUFFIX)

# source files for all stars, using preprocessor, put derived stars first
PL_SRCS= \
 SDFRankImage.pl \
 SDFAddMotionVecs.pl \
 SDFContrast.pl \
 SDFDCTImage.pl \
 SDFDCTImgCde.pl \
 SDFDCTImgCdeInv.pl \
 SDFDCTImageInv.pl \
 SDFDisplayImage.pl \
 SDFDisplayRGB.pl \
 SDFDisplayVideo.pl \
 SDFDither.pl \
 SDFDPCMImage.pl \
 SDFDPCMImageInv.pl \
 SDFEdgeDetect.pl \
 SDFMedianImage.pl \
 SDFMotionCmp.pl \
 SDFMotionCmpInv.pl \
 SDFReadImage.pl \
 SDFReadRGB.pl \
 SDFRGBToYUV.pl \
 SDFRunLenImg.pl \
 SDFRunLenImgInv.pl \
 SDFSunVideo.pl \
 SDFYUVToRGB.pl \
 SDFZigZagImg.pl \
 SDFZigZagImgInv.pl

# The .cc files
SRCS=$(PL_SRCS:.pl=.cc)

# object files for all stars
OBJS= $(SRCS:.cc=.o) 

# header files for all stars
HDRS= $(SRCS:.cc=.h)

STAR_MK=sdfimagestars
DOMAIN=SDF_IMAGE
DOMAIN_2=SDF
EXTRA_SRCS=$(STAR_MK).c $(PL_SRCS) rtvc.cc rtvc.h

all:		makefile stars_all

install:	stars_install


# include common definitions, rules
include $(ROOT)/mk/common.mk


# Don't add anything after the next line; makedepend will zap it.
# DO NOT DELETE THIS LINE -- make depend depends on it.

SDFRankImage.o: SDFRankImage.cc SDFRankImage.h \
 ../../../../../src/kernel/Matrix.h \
 ../../../../../src/kernel/Message.h \
 ../../../../../src/kernel/Particle.h \
 ../../../../../src/kernel/StringList.h \
 ../../../../../src/kernel/miscFuncs.h \
 ../../../../../src/kernel/DataStruct.h \
 ../../../../../src/kernel/type.h ../../../../../src/kernel/logNew.h \
 ../../../../../src/kernel/dataType.h ../../../../../src/kernel/Fix.h \
 ../../../../../src/kernel/ComplexSubset.h \
 ../../../../../src/kernel/isa.h \
 ../../../../../src/kernel/ComplexArrayState.h \
 ../../../../../src/kernel/State.h \
 ../../../../../src/kernel/InvokeInterp.h \
 ../../../../../src/kernel/Error.h \
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
SDFAddMotionVecs.o: SDFAddMotionVecs.cc SDFAddMotionVecs.h \
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
SDFContrast.o: SDFContrast.cc SDFContrast.h \
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
 ../../../../../src/kernel/IntArrayState.h \
 ../../../../../src/utils/libptdsp/ptdspHistogram.h
SDFDCTImage.o: SDFDCTImage.cc SDFDCTImage.h \
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
SDFDCTImgCde.o: SDFDCTImgCde.cc SDFDCTImgCde.h \
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
 ../../../../../src/utils/libptdsp/ptdspRunLength.h \
 ../../../../../src/domains/sdf/kernel/SDFStar.h \
 ../../../../../src/kernel/DataFlowStar.h \
 ../../../../../src/kernel/Star.h ../../../../../src/kernel/Block.h \
 ../../../../../src/kernel/Fraction.h \
 ../../../../../src/domains/sdf/kernel/SDFPortHole.h \
 ../../../../../src/kernel/DFPortHole.h \
 ../../../../../src/kernel/IntState.h \
 ../../../../../src/kernel/FloatState.h
SDFDCTImgCdeInv.o: SDFDCTImgCdeInv.cc SDFDCTImgCdeInv.h \
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
 ../../../../../src/kernel/IntState.h \
 ../../../../../src/utils/libptdsp/ptdspRunLength.h
SDFDCTImageInv.o: SDFDCTImageInv.cc SDFDCTImageInv.h \
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
SDFDisplayImage.o: SDFDisplayImage.cc SDFDisplayImage.h \
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
SDFDisplayRGB.o: SDFDisplayRGB.cc SDFDisplayRGB.h \
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
SDFDisplayVideo.o: SDFDisplayVideo.cc SDFDisplayVideo.h \
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
 ../../../../../src/kernel/IntState.h \
 ../../../../../src/kernel/StringState.h \
 ../../../../../src/kernel/KnownState.h
SDFDither.o: SDFDither.cc SDFDither.h \
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
 ../../../../../src/kernel/FloatArrayState.h \
 ../../../../../src/kernel/Matrix.h \
 ../../../../../src/kernel/Message.h \
 ../../../../../src/kernel/ComplexArrayState.h \
 ../../../../../src/kernel/FixArrayState.h \
 ../../../../../src/kernel/PrecisionState.h \
 ../../../../../src/kernel/IntArrayState.h
SDFDPCMImage.o: SDFDPCMImage.cc SDFDPCMImage.h \
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
 ../../../../../src/kernel/FloatState.h
SDFDPCMImageInv.o: SDFDPCMImageInv.cc SDFDPCMImageInv.h \
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
 ../../../../../src/kernel/FloatState.h
SDFEdgeDetect.o: SDFEdgeDetect.cc SDFEdgeDetect.h \
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
 ../../../../../src/kernel/StringState.h \
 ../../../../../src/kernel/KnownState.h \
 ../../../../../src/kernel/Matrix.h \
 ../../../../../src/kernel/Message.h \
 ../../../../../src/kernel/ComplexArrayState.h \
 ../../../../../src/kernel/FixArrayState.h \
 ../../../../../src/kernel/PrecisionState.h \
 ../../../../../src/kernel/FloatArrayState.h \
 ../../../../../src/kernel/IntArrayState.h
SDFMedianImage.o: SDFMedianImage.cc SDFMedianImage.h SDFRankImage.h \
 ../../../../../src/kernel/Matrix.h \
 ../../../../../src/kernel/Message.h \
 ../../../../../src/kernel/Particle.h \
 ../../../../../src/kernel/StringList.h \
 ../../../../../src/kernel/miscFuncs.h \
 ../../../../../src/kernel/DataStruct.h \
 ../../../../../src/kernel/type.h ../../../../../src/kernel/logNew.h \
 ../../../../../src/kernel/dataType.h ../../../../../src/kernel/Fix.h \
 ../../../../../src/kernel/ComplexSubset.h \
 ../../../../../src/kernel/isa.h \
 ../../../../../src/kernel/ComplexArrayState.h \
 ../../../../../src/kernel/State.h \
 ../../../../../src/kernel/InvokeInterp.h \
 ../../../../../src/kernel/Error.h \
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
SDFMotionCmp.o: SDFMotionCmp.cc SDFMotionCmp.h \
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
SDFMotionCmpInv.o: SDFMotionCmpInv.cc SDFMotionCmpInv.h \
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
SDFReadImage.o: SDFReadImage.cc SDFReadImage.h \
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
SDFReadRGB.o: SDFReadRGB.cc SDFReadRGB.h \
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
SDFRGBToYUV.o: SDFRGBToYUV.cc SDFRGBToYUV.h \
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
 ../../../../../src/utils/libptdsp/ptdspRGBYUVConversion.h
SDFRunLenImg.o: SDFRunLenImg.cc SDFRunLenImg.h \
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
SDFRunLenImgInv.o: SDFRunLenImgInv.cc SDFRunLenImgInv.h \
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
SDFSunVideo.o: SDFSunVideo.cc SDFSunVideo.h rtvc.h \
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
 ../../../../../src/kernel/FloatState.h \
 ../../../../../src/kernel/StringState.h \
 ../../../../../src/kernel/KnownState.h \
 ../../../../../src/kernel/Matrix.h \
 ../../../../../src/kernel/Message.h \
 ../../../../../src/kernel/ComplexArrayState.h \
 ../../../../../src/kernel/FixArrayState.h \
 ../../../../../src/kernel/PrecisionState.h \
 ../../../../../src/kernel/FloatArrayState.h \
 ../../../../../src/kernel/IntArrayState.h rtvc.cc
SDFYUVToRGB.o: SDFYUVToRGB.cc SDFYUVToRGB.h \
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
 ../../../../../src/utils/libptdsp/ptdspRGBYUVConversion.h
SDFZigZagImg.o: SDFZigZagImg.cc SDFZigZagImg.h \
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
 ../../../../../src/kernel/IntState.h \
 ../../../../../src/utils/libptdsp/ptdspZigZag.h
SDFZigZagImgInv.o: SDFZigZagImgInv.cc SDFZigZagImgInv.h \
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
 ../../../../../src/kernel/IntState.h \
 ../../../../../src/utils/libptdsp/ptdspZigZag.h
