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
# Makefile for Ptolemy MDSDF stars
# @(#)make.template	1.7 1/23/96
#
# If this file's name is "make.template" and there is no makefile in
# this directory, do
#	cp make.template makefile
#	chmod +w makefile
#	make depend
# This will add the dependencies to the makefile.

ROOT = ../../../..
VPATH = $(ROOT)/src/domains/mdsdf/stars

# get configuration info
CONFIG=$(ROOT)/mk/config-$(PTARCH).mk
include $(CONFIG)

#SDF kernel
SDFKERNDIR=$(ROOT)/src/domains/sdf/kernel

# MDSDFDIR is the directory containing MDSDF domain support
MDSDFDIR = $(ROOT)/src/domains/mdsdf/kernel
INCL = -I$(SDFKERNDIR) -I$(MDSDFDIR) -I$(KERNDIR)

# Library name
PTLIB=libmdsdfstars.$(LIBSUFFIX)

# source files for all stars, using preprocessor
PL_SRCS= \
 MDSDFAdd.pl \
 MDSDFBlackHole.pl \
 MDSDFConst.pl \
 MDSDFCxMatrix.pl \
 MDSDFCxToFloat.pl \
 MDSDFDB.pl \
 MDSDFShowImg.pl \
 MDSDFShowImg2.pl \
 MDSDFFIR.pl \
 MDSDFFFTCx.pl \
 MDSDFFFTCxCol.pl \
 MDSDFFloatMatrix.pl \
 MDSDFFloatToCx.pl \
 MDSDFFork.pl \
 MDSDFGain.pl \
 MDSDFIIDUniformMatrix.pl \
 MDSDFInverse.pl \
 MDSDFMult.pl \
 MDSDFQuant.pl \
 MDSDFReadImg.pl \
 MDSDFReadImg2.pl \
 MDSDFRepeat.pl \
 MDSDFTranspose.pl \
 MDSDFXprism3.pl

# The .cc files
SRCS=$(PL_SRCS:.pl=.cc)

# object files for all stars
OBJS= $(SRCS:.cc=.o)

# header files for all stars
HDRS= $(SRCS:.cc=.h)

STAR_MK=mdsdfstars
DOMAIN=MDSDF
EXTRA_SRCS=$(STAR_MK).mk $(STAR_MK).c $(PL_SRCS)

all:		makefile stars_all

install:	stars_install

# include common definitions, rules
include $(ROOT)/mk/common.mk


# Don't add anything after the next line; makedepend will zap it.
# DO NOT DELETE THIS LINE -- make depend depends on it.

MDSDFAdd.o: MDSDFAdd.cc MDSDFAdd.h \
 ../../../../src/domains/mdsdf/kernel/MDSDFStar.h \
 ../../../../src/domains/sdf/kernel/SDFStar.h \
 ../../../../src/kernel/DataFlowStar.h ../../../../src/kernel/Star.h \
 ../../../../src/kernel/Block.h ../../../../src/kernel/NamedObj.h \
 ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/logNew.h \
 ../../../../src/kernel/Error.h ../../../../src/kernel/DataStruct.h \
 ../../../../src/kernel/type.h ../../../../src/kernel/isa.h \
 ../../../../src/kernel/FlagArray.h ../../../../src/kernel/PortHole.h \
 ../../../../src/kernel/dataType.h ../../../../src/kernel/Particle.h \
 ../../../../src/kernel/Fix.h ../../../../src/kernel/ComplexSubset.h \
 ../../../../src/kernel/Attribute.h ../../../../src/kernel/State.h \
 ../../../../src/kernel/InvokeInterp.h \
 ../../../../src/kernel/Fraction.h \
 ../../../../src/domains/sdf/kernel/SDFPortHole.h \
 ../../../../src/kernel/DFPortHole.h \
 ../../../../src/domains/mdsdf/kernel/MDSDFPortHole.h \
 ../../../../src/kernel/MatrixParticle.h \
 ../../../../src/kernel/SubMatrix.h ../../../../src/kernel/Matrix.h \
 ../../../../src/kernel/Message.h \
 ../../../../src/kernel/ComplexArrayState.h \
 ../../../../src/kernel/FixArrayState.h \
 ../../../../src/kernel/PrecisionState.h \
 ../../../../src/kernel/FloatArrayState.h \
 ../../../../src/kernel/IntArrayState.h \
 ../../../../src/domains/mdsdf/kernel/MDSDFGeodesic.h \
 ../../../../src/kernel/Geodesic.h \
 ../../../../src/kernel/ParticleStack.h \
 ../../../../src/kernel/IntState.h
MDSDFBlackHole.o: MDSDFBlackHole.cc MDSDFBlackHole.h \
 ../../../../src/domains/mdsdf/kernel/MDSDFStar.h \
 ../../../../src/domains/sdf/kernel/SDFStar.h \
 ../../../../src/kernel/DataFlowStar.h ../../../../src/kernel/Star.h \
 ../../../../src/kernel/Block.h ../../../../src/kernel/NamedObj.h \
 ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/logNew.h \
 ../../../../src/kernel/Error.h ../../../../src/kernel/DataStruct.h \
 ../../../../src/kernel/type.h ../../../../src/kernel/isa.h \
 ../../../../src/kernel/FlagArray.h ../../../../src/kernel/PortHole.h \
 ../../../../src/kernel/dataType.h ../../../../src/kernel/Particle.h \
 ../../../../src/kernel/Fix.h ../../../../src/kernel/ComplexSubset.h \
 ../../../../src/kernel/Attribute.h ../../../../src/kernel/State.h \
 ../../../../src/kernel/InvokeInterp.h \
 ../../../../src/kernel/Fraction.h \
 ../../../../src/domains/sdf/kernel/SDFPortHole.h \
 ../../../../src/kernel/DFPortHole.h \
 ../../../../src/domains/mdsdf/kernel/MDSDFPortHole.h \
 ../../../../src/kernel/MatrixParticle.h \
 ../../../../src/kernel/SubMatrix.h ../../../../src/kernel/Matrix.h \
 ../../../../src/kernel/Message.h \
 ../../../../src/kernel/ComplexArrayState.h \
 ../../../../src/kernel/FixArrayState.h \
 ../../../../src/kernel/PrecisionState.h \
 ../../../../src/kernel/FloatArrayState.h \
 ../../../../src/kernel/IntArrayState.h \
 ../../../../src/domains/mdsdf/kernel/MDSDFGeodesic.h \
 ../../../../src/kernel/Geodesic.h \
 ../../../../src/kernel/ParticleStack.h
MDSDFConst.o: MDSDFConst.cc MDSDFConst.h \
 ../../../../src/domains/mdsdf/kernel/MDSDFStar.h \
 ../../../../src/domains/sdf/kernel/SDFStar.h \
 ../../../../src/kernel/DataFlowStar.h ../../../../src/kernel/Star.h \
 ../../../../src/kernel/Block.h ../../../../src/kernel/NamedObj.h \
 ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/logNew.h \
 ../../../../src/kernel/Error.h ../../../../src/kernel/DataStruct.h \
 ../../../../src/kernel/type.h ../../../../src/kernel/isa.h \
 ../../../../src/kernel/FlagArray.h ../../../../src/kernel/PortHole.h \
 ../../../../src/kernel/dataType.h ../../../../src/kernel/Particle.h \
 ../../../../src/kernel/Fix.h ../../../../src/kernel/ComplexSubset.h \
 ../../../../src/kernel/Attribute.h ../../../../src/kernel/State.h \
 ../../../../src/kernel/InvokeInterp.h \
 ../../../../src/kernel/Fraction.h \
 ../../../../src/domains/sdf/kernel/SDFPortHole.h \
 ../../../../src/kernel/DFPortHole.h \
 ../../../../src/domains/mdsdf/kernel/MDSDFPortHole.h \
 ../../../../src/kernel/MatrixParticle.h \
 ../../../../src/kernel/SubMatrix.h ../../../../src/kernel/Matrix.h \
 ../../../../src/kernel/Message.h \
 ../../../../src/kernel/ComplexArrayState.h \
 ../../../../src/kernel/FixArrayState.h \
 ../../../../src/kernel/PrecisionState.h \
 ../../../../src/kernel/FloatArrayState.h \
 ../../../../src/kernel/IntArrayState.h \
 ../../../../src/domains/mdsdf/kernel/MDSDFGeodesic.h \
 ../../../../src/kernel/Geodesic.h \
 ../../../../src/kernel/ParticleStack.h \
 ../../../../src/kernel/IntState.h ../../../../src/kernel/FloatState.h
MDSDFCxMatrix.o: MDSDFCxMatrix.cc MDSDFCxMatrix.h \
 ../../../../src/domains/mdsdf/kernel/MDSDFStar.h \
 ../../../../src/domains/sdf/kernel/SDFStar.h \
 ../../../../src/kernel/DataFlowStar.h ../../../../src/kernel/Star.h \
 ../../../../src/kernel/Block.h ../../../../src/kernel/NamedObj.h \
 ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/logNew.h \
 ../../../../src/kernel/Error.h ../../../../src/kernel/DataStruct.h \
 ../../../../src/kernel/type.h ../../../../src/kernel/isa.h \
 ../../../../src/kernel/FlagArray.h ../../../../src/kernel/PortHole.h \
 ../../../../src/kernel/dataType.h ../../../../src/kernel/Particle.h \
 ../../../../src/kernel/Fix.h ../../../../src/kernel/ComplexSubset.h \
 ../../../../src/kernel/Attribute.h ../../../../src/kernel/State.h \
 ../../../../src/kernel/InvokeInterp.h \
 ../../../../src/kernel/Fraction.h \
 ../../../../src/domains/sdf/kernel/SDFPortHole.h \
 ../../../../src/kernel/DFPortHole.h \
 ../../../../src/domains/mdsdf/kernel/MDSDFPortHole.h \
 ../../../../src/kernel/MatrixParticle.h \
 ../../../../src/kernel/SubMatrix.h ../../../../src/kernel/Matrix.h \
 ../../../../src/kernel/Message.h \
 ../../../../src/kernel/ComplexArrayState.h \
 ../../../../src/kernel/FixArrayState.h \
 ../../../../src/kernel/PrecisionState.h \
 ../../../../src/kernel/FloatArrayState.h \
 ../../../../src/kernel/IntArrayState.h \
 ../../../../src/domains/mdsdf/kernel/MDSDFGeodesic.h \
 ../../../../src/kernel/Geodesic.h \
 ../../../../src/kernel/ParticleStack.h \
 ../../../../src/kernel/IntState.h
MDSDFCxToFloat.o: MDSDFCxToFloat.cc MDSDFCxToFloat.h \
 ../../../../src/domains/mdsdf/kernel/MDSDFStar.h \
 ../../../../src/domains/sdf/kernel/SDFStar.h \
 ../../../../src/kernel/DataFlowStar.h ../../../../src/kernel/Star.h \
 ../../../../src/kernel/Block.h ../../../../src/kernel/NamedObj.h \
 ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/logNew.h \
 ../../../../src/kernel/Error.h ../../../../src/kernel/DataStruct.h \
 ../../../../src/kernel/type.h ../../../../src/kernel/isa.h \
 ../../../../src/kernel/FlagArray.h ../../../../src/kernel/PortHole.h \
 ../../../../src/kernel/dataType.h ../../../../src/kernel/Particle.h \
 ../../../../src/kernel/Fix.h ../../../../src/kernel/ComplexSubset.h \
 ../../../../src/kernel/Attribute.h ../../../../src/kernel/State.h \
 ../../../../src/kernel/InvokeInterp.h \
 ../../../../src/kernel/Fraction.h \
 ../../../../src/domains/sdf/kernel/SDFPortHole.h \
 ../../../../src/kernel/DFPortHole.h \
 ../../../../src/domains/mdsdf/kernel/MDSDFPortHole.h \
 ../../../../src/kernel/MatrixParticle.h \
 ../../../../src/kernel/SubMatrix.h ../../../../src/kernel/Matrix.h \
 ../../../../src/kernel/Message.h \
 ../../../../src/kernel/ComplexArrayState.h \
 ../../../../src/kernel/FixArrayState.h \
 ../../../../src/kernel/PrecisionState.h \
 ../../../../src/kernel/FloatArrayState.h \
 ../../../../src/kernel/IntArrayState.h \
 ../../../../src/domains/mdsdf/kernel/MDSDFGeodesic.h \
 ../../../../src/kernel/Geodesic.h \
 ../../../../src/kernel/ParticleStack.h \
 ../../../../src/kernel/IntState.h
MDSDFDB.o: MDSDFDB.cc MDSDFDB.h \
 ../../../../src/domains/mdsdf/kernel/MDSDFStar.h \
 ../../../../src/domains/sdf/kernel/SDFStar.h \
 ../../../../src/kernel/DataFlowStar.h ../../../../src/kernel/Star.h \
 ../../../../src/kernel/Block.h ../../../../src/kernel/NamedObj.h \
 ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/logNew.h \
 ../../../../src/kernel/Error.h ../../../../src/kernel/DataStruct.h \
 ../../../../src/kernel/type.h ../../../../src/kernel/isa.h \
 ../../../../src/kernel/FlagArray.h ../../../../src/kernel/PortHole.h \
 ../../../../src/kernel/dataType.h ../../../../src/kernel/Particle.h \
 ../../../../src/kernel/Fix.h ../../../../src/kernel/ComplexSubset.h \
 ../../../../src/kernel/Attribute.h ../../../../src/kernel/State.h \
 ../../../../src/kernel/InvokeInterp.h \
 ../../../../src/kernel/Fraction.h \
 ../../../../src/domains/sdf/kernel/SDFPortHole.h \
 ../../../../src/kernel/DFPortHole.h \
 ../../../../src/domains/mdsdf/kernel/MDSDFPortHole.h \
 ../../../../src/kernel/MatrixParticle.h \
 ../../../../src/kernel/SubMatrix.h ../../../../src/kernel/Matrix.h \
 ../../../../src/kernel/Message.h \
 ../../../../src/kernel/ComplexArrayState.h \
 ../../../../src/kernel/FixArrayState.h \
 ../../../../src/kernel/PrecisionState.h \
 ../../../../src/kernel/FloatArrayState.h \
 ../../../../src/kernel/IntArrayState.h \
 ../../../../src/domains/mdsdf/kernel/MDSDFGeodesic.h \
 ../../../../src/kernel/Geodesic.h \
 ../../../../src/kernel/ParticleStack.h \
 ../../../../src/kernel/IntState.h ../../../../src/kernel/FloatState.h
MDSDFShowImg.o: MDSDFShowImg.cc MDSDFShowImg.h \
 ../../../../src/domains/mdsdf/kernel/MDSDFStar.h \
 ../../../../src/domains/sdf/kernel/SDFStar.h \
 ../../../../src/kernel/DataFlowStar.h ../../../../src/kernel/Star.h \
 ../../../../src/kernel/Block.h ../../../../src/kernel/NamedObj.h \
 ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/logNew.h \
 ../../../../src/kernel/Error.h ../../../../src/kernel/DataStruct.h \
 ../../../../src/kernel/type.h ../../../../src/kernel/isa.h \
 ../../../../src/kernel/FlagArray.h ../../../../src/kernel/PortHole.h \
 ../../../../src/kernel/dataType.h ../../../../src/kernel/Particle.h \
 ../../../../src/kernel/Fix.h ../../../../src/kernel/ComplexSubset.h \
 ../../../../src/kernel/Attribute.h ../../../../src/kernel/State.h \
 ../../../../src/kernel/InvokeInterp.h \
 ../../../../src/kernel/Fraction.h \
 ../../../../src/domains/sdf/kernel/SDFPortHole.h \
 ../../../../src/kernel/DFPortHole.h \
 ../../../../src/domains/mdsdf/kernel/MDSDFPortHole.h \
 ../../../../src/kernel/MatrixParticle.h \
 ../../../../src/kernel/SubMatrix.h ../../../../src/kernel/Matrix.h \
 ../../../../src/kernel/Message.h \
 ../../../../src/kernel/ComplexArrayState.h \
 ../../../../src/kernel/FixArrayState.h \
 ../../../../src/kernel/PrecisionState.h \
 ../../../../src/kernel/FloatArrayState.h \
 ../../../../src/kernel/IntArrayState.h \
 ../../../../src/domains/mdsdf/kernel/MDSDFGeodesic.h \
 ../../../../src/kernel/Geodesic.h \
 ../../../../src/kernel/ParticleStack.h \
 ../../../../src/kernel/IntState.h \
 ../../../../src/kernel/StringState.h \
 ../../../../src/kernel/KnownState.h
MDSDFShowImg2.o: MDSDFShowImg2.cc MDSDFShowImg2.h \
 ../../../../src/domains/mdsdf/kernel/MDSDFStar.h \
 ../../../../src/domains/sdf/kernel/SDFStar.h \
 ../../../../src/kernel/DataFlowStar.h ../../../../src/kernel/Star.h \
 ../../../../src/kernel/Block.h ../../../../src/kernel/NamedObj.h \
 ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/logNew.h \
 ../../../../src/kernel/Error.h ../../../../src/kernel/DataStruct.h \
 ../../../../src/kernel/type.h ../../../../src/kernel/isa.h \
 ../../../../src/kernel/FlagArray.h ../../../../src/kernel/PortHole.h \
 ../../../../src/kernel/dataType.h ../../../../src/kernel/Particle.h \
 ../../../../src/kernel/Fix.h ../../../../src/kernel/ComplexSubset.h \
 ../../../../src/kernel/Attribute.h ../../../../src/kernel/State.h \
 ../../../../src/kernel/InvokeInterp.h \
 ../../../../src/kernel/Fraction.h \
 ../../../../src/domains/sdf/kernel/SDFPortHole.h \
 ../../../../src/kernel/DFPortHole.h \
 ../../../../src/domains/mdsdf/kernel/MDSDFPortHole.h \
 ../../../../src/kernel/MatrixParticle.h \
 ../../../../src/kernel/SubMatrix.h ../../../../src/kernel/Matrix.h \
 ../../../../src/kernel/Message.h \
 ../../../../src/kernel/ComplexArrayState.h \
 ../../../../src/kernel/FixArrayState.h \
 ../../../../src/kernel/PrecisionState.h \
 ../../../../src/kernel/FloatArrayState.h \
 ../../../../src/kernel/IntArrayState.h \
 ../../../../src/domains/mdsdf/kernel/MDSDFGeodesic.h \
 ../../../../src/kernel/Geodesic.h \
 ../../../../src/kernel/ParticleStack.h \
 ../../../../src/kernel/IntState.h \
 ../../../../src/kernel/StringState.h \
 ../../../../src/kernel/KnownState.h
MDSDFFIR.o: MDSDFFIR.cc MDSDFFIR.h \
 ../../../../src/domains/mdsdf/kernel/MDSDFStar.h \
 ../../../../src/domains/sdf/kernel/SDFStar.h \
 ../../../../src/kernel/DataFlowStar.h ../../../../src/kernel/Star.h \
 ../../../../src/kernel/Block.h ../../../../src/kernel/NamedObj.h \
 ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/logNew.h \
 ../../../../src/kernel/Error.h ../../../../src/kernel/DataStruct.h \
 ../../../../src/kernel/type.h ../../../../src/kernel/isa.h \
 ../../../../src/kernel/FlagArray.h ../../../../src/kernel/PortHole.h \
 ../../../../src/kernel/dataType.h ../../../../src/kernel/Particle.h \
 ../../../../src/kernel/Fix.h ../../../../src/kernel/ComplexSubset.h \
 ../../../../src/kernel/Attribute.h ../../../../src/kernel/State.h \
 ../../../../src/kernel/InvokeInterp.h \
 ../../../../src/kernel/Fraction.h \
 ../../../../src/domains/sdf/kernel/SDFPortHole.h \
 ../../../../src/kernel/DFPortHole.h \
 ../../../../src/domains/mdsdf/kernel/MDSDFPortHole.h \
 ../../../../src/kernel/MatrixParticle.h \
 ../../../../src/kernel/SubMatrix.h ../../../../src/kernel/Matrix.h \
 ../../../../src/kernel/Message.h \
 ../../../../src/kernel/ComplexArrayState.h \
 ../../../../src/kernel/FixArrayState.h \
 ../../../../src/kernel/PrecisionState.h \
 ../../../../src/kernel/FloatArrayState.h \
 ../../../../src/kernel/IntArrayState.h \
 ../../../../src/domains/mdsdf/kernel/MDSDFGeodesic.h \
 ../../../../src/kernel/Geodesic.h \
 ../../../../src/kernel/ParticleStack.h \
 ../../../../src/kernel/IntState.h
MDSDFFFTCx.o: MDSDFFFTCx.cc MDSDFFFTCx.h \
 ../../../../src/domains/mdsdf/kernel/MDSDFStar.h \
 ../../../../src/domains/sdf/kernel/SDFStar.h \
 ../../../../src/kernel/DataFlowStar.h ../../../../src/kernel/Star.h \
 ../../../../src/kernel/Block.h ../../../../src/kernel/NamedObj.h \
 ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/logNew.h \
 ../../../../src/kernel/Error.h ../../../../src/kernel/DataStruct.h \
 ../../../../src/kernel/type.h ../../../../src/kernel/isa.h \
 ../../../../src/kernel/FlagArray.h ../../../../src/kernel/PortHole.h \
 ../../../../src/kernel/dataType.h ../../../../src/kernel/Particle.h \
 ../../../../src/kernel/Fix.h ../../../../src/kernel/ComplexSubset.h \
 ../../../../src/kernel/Attribute.h ../../../../src/kernel/State.h \
 ../../../../src/kernel/InvokeInterp.h \
 ../../../../src/kernel/Fraction.h \
 ../../../../src/domains/sdf/kernel/SDFPortHole.h \
 ../../../../src/kernel/DFPortHole.h \
 ../../../../src/domains/mdsdf/kernel/MDSDFPortHole.h \
 ../../../../src/kernel/MatrixParticle.h \
 ../../../../src/kernel/SubMatrix.h ../../../../src/kernel/Matrix.h \
 ../../../../src/kernel/Message.h \
 ../../../../src/kernel/ComplexArrayState.h \
 ../../../../src/kernel/FixArrayState.h \
 ../../../../src/kernel/PrecisionState.h \
 ../../../../src/kernel/FloatArrayState.h \
 ../../../../src/kernel/IntArrayState.h \
 ../../../../src/domains/mdsdf/kernel/MDSDFGeodesic.h \
 ../../../../src/kernel/Geodesic.h \
 ../../../../src/kernel/ParticleStack.h \
 ../../../../src/kernel/IntState.h
MDSDFFFTCxCol.o: MDSDFFFTCxCol.cc MDSDFFFTCxCol.h \
 ../../../../src/domains/mdsdf/kernel/MDSDFStar.h \
 ../../../../src/domains/sdf/kernel/SDFStar.h \
 ../../../../src/kernel/DataFlowStar.h ../../../../src/kernel/Star.h \
 ../../../../src/kernel/Block.h ../../../../src/kernel/NamedObj.h \
 ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/logNew.h \
 ../../../../src/kernel/Error.h ../../../../src/kernel/DataStruct.h \
 ../../../../src/kernel/type.h ../../../../src/kernel/isa.h \
 ../../../../src/kernel/FlagArray.h ../../../../src/kernel/PortHole.h \
 ../../../../src/kernel/dataType.h ../../../../src/kernel/Particle.h \
 ../../../../src/kernel/Fix.h ../../../../src/kernel/ComplexSubset.h \
 ../../../../src/kernel/Attribute.h ../../../../src/kernel/State.h \
 ../../../../src/kernel/InvokeInterp.h \
 ../../../../src/kernel/Fraction.h \
 ../../../../src/domains/sdf/kernel/SDFPortHole.h \
 ../../../../src/kernel/DFPortHole.h \
 ../../../../src/domains/mdsdf/kernel/MDSDFPortHole.h \
 ../../../../src/kernel/MatrixParticle.h \
 ../../../../src/kernel/SubMatrix.h ../../../../src/kernel/Matrix.h \
 ../../../../src/kernel/Message.h \
 ../../../../src/kernel/ComplexArrayState.h \
 ../../../../src/kernel/FixArrayState.h \
 ../../../../src/kernel/PrecisionState.h \
 ../../../../src/kernel/FloatArrayState.h \
 ../../../../src/kernel/IntArrayState.h \
 ../../../../src/domains/mdsdf/kernel/MDSDFGeodesic.h \
 ../../../../src/kernel/Geodesic.h \
 ../../../../src/kernel/ParticleStack.h \
 ../../../../src/kernel/IntState.h
MDSDFFloatMatrix.o: MDSDFFloatMatrix.cc MDSDFFloatMatrix.h \
 ../../../../src/domains/mdsdf/kernel/MDSDFStar.h \
 ../../../../src/domains/sdf/kernel/SDFStar.h \
 ../../../../src/kernel/DataFlowStar.h ../../../../src/kernel/Star.h \
 ../../../../src/kernel/Block.h ../../../../src/kernel/NamedObj.h \
 ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/logNew.h \
 ../../../../src/kernel/Error.h ../../../../src/kernel/DataStruct.h \
 ../../../../src/kernel/type.h ../../../../src/kernel/isa.h \
 ../../../../src/kernel/FlagArray.h ../../../../src/kernel/PortHole.h \
 ../../../../src/kernel/dataType.h ../../../../src/kernel/Particle.h \
 ../../../../src/kernel/Fix.h ../../../../src/kernel/ComplexSubset.h \
 ../../../../src/kernel/Attribute.h ../../../../src/kernel/State.h \
 ../../../../src/kernel/InvokeInterp.h \
 ../../../../src/kernel/Fraction.h \
 ../../../../src/domains/sdf/kernel/SDFPortHole.h \
 ../../../../src/kernel/DFPortHole.h \
 ../../../../src/domains/mdsdf/kernel/MDSDFPortHole.h \
 ../../../../src/kernel/MatrixParticle.h \
 ../../../../src/kernel/SubMatrix.h ../../../../src/kernel/Matrix.h \
 ../../../../src/kernel/Message.h \
 ../../../../src/kernel/ComplexArrayState.h \
 ../../../../src/kernel/FixArrayState.h \
 ../../../../src/kernel/PrecisionState.h \
 ../../../../src/kernel/FloatArrayState.h \
 ../../../../src/kernel/IntArrayState.h \
 ../../../../src/domains/mdsdf/kernel/MDSDFGeodesic.h \
 ../../../../src/kernel/Geodesic.h \
 ../../../../src/kernel/ParticleStack.h \
 ../../../../src/kernel/IntState.h
MDSDFFloatToCx.o: MDSDFFloatToCx.cc MDSDFFloatToCx.h \
 ../../../../src/domains/mdsdf/kernel/MDSDFStar.h \
 ../../../../src/domains/sdf/kernel/SDFStar.h \
 ../../../../src/kernel/DataFlowStar.h ../../../../src/kernel/Star.h \
 ../../../../src/kernel/Block.h ../../../../src/kernel/NamedObj.h \
 ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/logNew.h \
 ../../../../src/kernel/Error.h ../../../../src/kernel/DataStruct.h \
 ../../../../src/kernel/type.h ../../../../src/kernel/isa.h \
 ../../../../src/kernel/FlagArray.h ../../../../src/kernel/PortHole.h \
 ../../../../src/kernel/dataType.h ../../../../src/kernel/Particle.h \
 ../../../../src/kernel/Fix.h ../../../../src/kernel/ComplexSubset.h \
 ../../../../src/kernel/Attribute.h ../../../../src/kernel/State.h \
 ../../../../src/kernel/InvokeInterp.h \
 ../../../../src/kernel/Fraction.h \
 ../../../../src/domains/sdf/kernel/SDFPortHole.h \
 ../../../../src/kernel/DFPortHole.h \
 ../../../../src/domains/mdsdf/kernel/MDSDFPortHole.h \
 ../../../../src/kernel/MatrixParticle.h \
 ../../../../src/kernel/SubMatrix.h ../../../../src/kernel/Matrix.h \
 ../../../../src/kernel/Message.h \
 ../../../../src/kernel/ComplexArrayState.h \
 ../../../../src/kernel/FixArrayState.h \
 ../../../../src/kernel/PrecisionState.h \
 ../../../../src/kernel/FloatArrayState.h \
 ../../../../src/kernel/IntArrayState.h \
 ../../../../src/domains/mdsdf/kernel/MDSDFGeodesic.h \
 ../../../../src/kernel/Geodesic.h \
 ../../../../src/kernel/ParticleStack.h \
 ../../../../src/kernel/IntState.h
MDSDFFork.o: MDSDFFork.cc MDSDFFork.h \
 ../../../../src/domains/mdsdf/kernel/MDSDFStar.h \
 ../../../../src/domains/sdf/kernel/SDFStar.h \
 ../../../../src/kernel/DataFlowStar.h ../../../../src/kernel/Star.h \
 ../../../../src/kernel/Block.h ../../../../src/kernel/NamedObj.h \
 ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/logNew.h \
 ../../../../src/kernel/Error.h ../../../../src/kernel/DataStruct.h \
 ../../../../src/kernel/type.h ../../../../src/kernel/isa.h \
 ../../../../src/kernel/FlagArray.h ../../../../src/kernel/PortHole.h \
 ../../../../src/kernel/dataType.h ../../../../src/kernel/Particle.h \
 ../../../../src/kernel/Fix.h ../../../../src/kernel/ComplexSubset.h \
 ../../../../src/kernel/Attribute.h ../../../../src/kernel/State.h \
 ../../../../src/kernel/InvokeInterp.h \
 ../../../../src/kernel/Fraction.h \
 ../../../../src/domains/sdf/kernel/SDFPortHole.h \
 ../../../../src/kernel/DFPortHole.h \
 ../../../../src/domains/mdsdf/kernel/MDSDFPortHole.h \
 ../../../../src/kernel/MatrixParticle.h \
 ../../../../src/kernel/SubMatrix.h ../../../../src/kernel/Matrix.h \
 ../../../../src/kernel/Message.h \
 ../../../../src/kernel/ComplexArrayState.h \
 ../../../../src/kernel/FixArrayState.h \
 ../../../../src/kernel/PrecisionState.h \
 ../../../../src/kernel/FloatArrayState.h \
 ../../../../src/kernel/IntArrayState.h \
 ../../../../src/domains/mdsdf/kernel/MDSDFGeodesic.h \
 ../../../../src/kernel/Geodesic.h \
 ../../../../src/kernel/ParticleStack.h
MDSDFGain.o: MDSDFGain.cc MDSDFGain.h \
 ../../../../src/domains/mdsdf/kernel/MDSDFStar.h \
 ../../../../src/domains/sdf/kernel/SDFStar.h \
 ../../../../src/kernel/DataFlowStar.h ../../../../src/kernel/Star.h \
 ../../../../src/kernel/Block.h ../../../../src/kernel/NamedObj.h \
 ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/logNew.h \
 ../../../../src/kernel/Error.h ../../../../src/kernel/DataStruct.h \
 ../../../../src/kernel/type.h ../../../../src/kernel/isa.h \
 ../../../../src/kernel/FlagArray.h ../../../../src/kernel/PortHole.h \
 ../../../../src/kernel/dataType.h ../../../../src/kernel/Particle.h \
 ../../../../src/kernel/Fix.h ../../../../src/kernel/ComplexSubset.h \
 ../../../../src/kernel/Attribute.h ../../../../src/kernel/State.h \
 ../../../../src/kernel/InvokeInterp.h \
 ../../../../src/kernel/Fraction.h \
 ../../../../src/domains/sdf/kernel/SDFPortHole.h \
 ../../../../src/kernel/DFPortHole.h \
 ../../../../src/domains/mdsdf/kernel/MDSDFPortHole.h \
 ../../../../src/kernel/MatrixParticle.h \
 ../../../../src/kernel/SubMatrix.h ../../../../src/kernel/Matrix.h \
 ../../../../src/kernel/Message.h \
 ../../../../src/kernel/ComplexArrayState.h \
 ../../../../src/kernel/FixArrayState.h \
 ../../../../src/kernel/PrecisionState.h \
 ../../../../src/kernel/FloatArrayState.h \
 ../../../../src/kernel/IntArrayState.h \
 ../../../../src/domains/mdsdf/kernel/MDSDFGeodesic.h \
 ../../../../src/kernel/Geodesic.h \
 ../../../../src/kernel/ParticleStack.h \
 ../../../../src/kernel/IntState.h ../../../../src/kernel/FloatState.h
MDSDFIIDUniformMatrix.o: MDSDFIIDUniformMatrix.cc \
 MDSDFIIDUniformMatrix.h \
 ../../../../src/domains/mdsdf/kernel/MDSDFStar.h \
 ../../../../src/domains/sdf/kernel/SDFStar.h \
 ../../../../src/kernel/DataFlowStar.h ../../../../src/kernel/Star.h \
 ../../../../src/kernel/Block.h ../../../../src/kernel/NamedObj.h \
 ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/logNew.h \
 ../../../../src/kernel/Error.h ../../../../src/kernel/DataStruct.h \
 ../../../../src/kernel/type.h ../../../../src/kernel/isa.h \
 ../../../../src/kernel/FlagArray.h ../../../../src/kernel/PortHole.h \
 ../../../../src/kernel/dataType.h ../../../../src/kernel/Particle.h \
 ../../../../src/kernel/Fix.h ../../../../src/kernel/ComplexSubset.h \
 ../../../../src/kernel/Attribute.h ../../../../src/kernel/State.h \
 ../../../../src/kernel/InvokeInterp.h \
 ../../../../src/kernel/Fraction.h \
 ../../../../src/domains/sdf/kernel/SDFPortHole.h \
 ../../../../src/kernel/DFPortHole.h \
 ../../../../src/domains/mdsdf/kernel/MDSDFPortHole.h \
 ../../../../src/kernel/MatrixParticle.h \
 ../../../../src/kernel/SubMatrix.h ../../../../src/kernel/Matrix.h \
 ../../../../src/kernel/Message.h \
 ../../../../src/kernel/ComplexArrayState.h \
 ../../../../src/kernel/FixArrayState.h \
 ../../../../src/kernel/PrecisionState.h \
 ../../../../src/kernel/FloatArrayState.h \
 ../../../../src/kernel/IntArrayState.h \
 ../../../../src/domains/mdsdf/kernel/MDSDFGeodesic.h \
 ../../../../src/kernel/Geodesic.h \
 ../../../../src/kernel/ParticleStack.h \
 ../../../../src/kernel/IntState.h ../../../../src/kernel/FloatState.h
MDSDFInverse.o: MDSDFInverse.cc MDSDFInverse.h \
 ../../../../src/domains/mdsdf/kernel/MDSDFStar.h \
 ../../../../src/domains/sdf/kernel/SDFStar.h \
 ../../../../src/kernel/DataFlowStar.h ../../../../src/kernel/Star.h \
 ../../../../src/kernel/Block.h ../../../../src/kernel/NamedObj.h \
 ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/logNew.h \
 ../../../../src/kernel/Error.h ../../../../src/kernel/DataStruct.h \
 ../../../../src/kernel/type.h ../../../../src/kernel/isa.h \
 ../../../../src/kernel/FlagArray.h ../../../../src/kernel/PortHole.h \
 ../../../../src/kernel/dataType.h ../../../../src/kernel/Particle.h \
 ../../../../src/kernel/Fix.h ../../../../src/kernel/ComplexSubset.h \
 ../../../../src/kernel/Attribute.h ../../../../src/kernel/State.h \
 ../../../../src/kernel/InvokeInterp.h \
 ../../../../src/kernel/Fraction.h \
 ../../../../src/domains/sdf/kernel/SDFPortHole.h \
 ../../../../src/kernel/DFPortHole.h \
 ../../../../src/domains/mdsdf/kernel/MDSDFPortHole.h \
 ../../../../src/kernel/MatrixParticle.h \
 ../../../../src/kernel/SubMatrix.h ../../../../src/kernel/Matrix.h \
 ../../../../src/kernel/Message.h \
 ../../../../src/kernel/ComplexArrayState.h \
 ../../../../src/kernel/FixArrayState.h \
 ../../../../src/kernel/PrecisionState.h \
 ../../../../src/kernel/FloatArrayState.h \
 ../../../../src/kernel/IntArrayState.h \
 ../../../../src/domains/mdsdf/kernel/MDSDFGeodesic.h \
 ../../../../src/kernel/Geodesic.h \
 ../../../../src/kernel/ParticleStack.h \
 ../../../../src/kernel/IntState.h
MDSDFMult.o: MDSDFMult.cc MDSDFMult.h \
 ../../../../src/domains/mdsdf/kernel/MDSDFStar.h \
 ../../../../src/domains/sdf/kernel/SDFStar.h \
 ../../../../src/kernel/DataFlowStar.h ../../../../src/kernel/Star.h \
 ../../../../src/kernel/Block.h ../../../../src/kernel/NamedObj.h \
 ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/logNew.h \
 ../../../../src/kernel/Error.h ../../../../src/kernel/DataStruct.h \
 ../../../../src/kernel/type.h ../../../../src/kernel/isa.h \
 ../../../../src/kernel/FlagArray.h ../../../../src/kernel/PortHole.h \
 ../../../../src/kernel/dataType.h ../../../../src/kernel/Particle.h \
 ../../../../src/kernel/Fix.h ../../../../src/kernel/ComplexSubset.h \
 ../../../../src/kernel/Attribute.h ../../../../src/kernel/State.h \
 ../../../../src/kernel/InvokeInterp.h \
 ../../../../src/kernel/Fraction.h \
 ../../../../src/domains/sdf/kernel/SDFPortHole.h \
 ../../../../src/kernel/DFPortHole.h \
 ../../../../src/domains/mdsdf/kernel/MDSDFPortHole.h \
 ../../../../src/kernel/MatrixParticle.h \
 ../../../../src/kernel/SubMatrix.h ../../../../src/kernel/Matrix.h \
 ../../../../src/kernel/Message.h \
 ../../../../src/kernel/ComplexArrayState.h \
 ../../../../src/kernel/FixArrayState.h \
 ../../../../src/kernel/PrecisionState.h \
 ../../../../src/kernel/FloatArrayState.h \
 ../../../../src/kernel/IntArrayState.h \
 ../../../../src/domains/mdsdf/kernel/MDSDFGeodesic.h \
 ../../../../src/kernel/Geodesic.h \
 ../../../../src/kernel/ParticleStack.h \
 ../../../../src/kernel/IntState.h
MDSDFQuant.o: MDSDFQuant.cc MDSDFQuant.h \
 ../../../../src/domains/mdsdf/kernel/MDSDFStar.h \
 ../../../../src/domains/sdf/kernel/SDFStar.h \
 ../../../../src/kernel/DataFlowStar.h ../../../../src/kernel/Star.h \
 ../../../../src/kernel/Block.h ../../../../src/kernel/NamedObj.h \
 ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/logNew.h \
 ../../../../src/kernel/Error.h ../../../../src/kernel/DataStruct.h \
 ../../../../src/kernel/type.h ../../../../src/kernel/isa.h \
 ../../../../src/kernel/FlagArray.h ../../../../src/kernel/PortHole.h \
 ../../../../src/kernel/dataType.h ../../../../src/kernel/Particle.h \
 ../../../../src/kernel/Fix.h ../../../../src/kernel/ComplexSubset.h \
 ../../../../src/kernel/Attribute.h ../../../../src/kernel/State.h \
 ../../../../src/kernel/InvokeInterp.h \
 ../../../../src/kernel/Fraction.h \
 ../../../../src/domains/sdf/kernel/SDFPortHole.h \
 ../../../../src/kernel/DFPortHole.h \
 ../../../../src/domains/mdsdf/kernel/MDSDFPortHole.h \
 ../../../../src/kernel/MatrixParticle.h \
 ../../../../src/kernel/SubMatrix.h ../../../../src/kernel/Matrix.h \
 ../../../../src/kernel/Message.h \
 ../../../../src/kernel/ComplexArrayState.h \
 ../../../../src/kernel/FixArrayState.h \
 ../../../../src/kernel/PrecisionState.h \
 ../../../../src/kernel/FloatArrayState.h \
 ../../../../src/kernel/IntArrayState.h \
 ../../../../src/domains/mdsdf/kernel/MDSDFGeodesic.h \
 ../../../../src/kernel/Geodesic.h \
 ../../../../src/kernel/ParticleStack.h \
 ../../../../src/kernel/IntState.h
MDSDFReadImg.o: MDSDFReadImg.cc MDSDFReadImg.h \
 ../../../../src/domains/mdsdf/kernel/MDSDFStar.h \
 ../../../../src/domains/sdf/kernel/SDFStar.h \
 ../../../../src/kernel/DataFlowStar.h ../../../../src/kernel/Star.h \
 ../../../../src/kernel/Block.h ../../../../src/kernel/NamedObj.h \
 ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/logNew.h \
 ../../../../src/kernel/Error.h ../../../../src/kernel/DataStruct.h \
 ../../../../src/kernel/type.h ../../../../src/kernel/isa.h \
 ../../../../src/kernel/FlagArray.h ../../../../src/kernel/PortHole.h \
 ../../../../src/kernel/dataType.h ../../../../src/kernel/Particle.h \
 ../../../../src/kernel/Fix.h ../../../../src/kernel/ComplexSubset.h \
 ../../../../src/kernel/Attribute.h ../../../../src/kernel/State.h \
 ../../../../src/kernel/InvokeInterp.h \
 ../../../../src/kernel/Fraction.h \
 ../../../../src/domains/sdf/kernel/SDFPortHole.h \
 ../../../../src/kernel/DFPortHole.h \
 ../../../../src/domains/mdsdf/kernel/MDSDFPortHole.h \
 ../../../../src/kernel/MatrixParticle.h \
 ../../../../src/kernel/SubMatrix.h ../../../../src/kernel/Matrix.h \
 ../../../../src/kernel/Message.h \
 ../../../../src/kernel/ComplexArrayState.h \
 ../../../../src/kernel/FixArrayState.h \
 ../../../../src/kernel/PrecisionState.h \
 ../../../../src/kernel/FloatArrayState.h \
 ../../../../src/kernel/IntArrayState.h \
 ../../../../src/domains/mdsdf/kernel/MDSDFGeodesic.h \
 ../../../../src/kernel/Geodesic.h \
 ../../../../src/kernel/ParticleStack.h \
 ../../../../src/kernel/IntState.h \
 ../../../../src/kernel/StringState.h \
 ../../../../src/kernel/KnownState.h
MDSDFReadImg2.o: MDSDFReadImg2.cc MDSDFReadImg2.h \
 ../../../../src/domains/mdsdf/kernel/MDSDFStar.h \
 ../../../../src/domains/sdf/kernel/SDFStar.h \
 ../../../../src/kernel/DataFlowStar.h ../../../../src/kernel/Star.h \
 ../../../../src/kernel/Block.h ../../../../src/kernel/NamedObj.h \
 ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/logNew.h \
 ../../../../src/kernel/Error.h ../../../../src/kernel/DataStruct.h \
 ../../../../src/kernel/type.h ../../../../src/kernel/isa.h \
 ../../../../src/kernel/FlagArray.h ../../../../src/kernel/PortHole.h \
 ../../../../src/kernel/dataType.h ../../../../src/kernel/Particle.h \
 ../../../../src/kernel/Fix.h ../../../../src/kernel/ComplexSubset.h \
 ../../../../src/kernel/Attribute.h ../../../../src/kernel/State.h \
 ../../../../src/kernel/InvokeInterp.h \
 ../../../../src/kernel/Fraction.h \
 ../../../../src/domains/sdf/kernel/SDFPortHole.h \
 ../../../../src/kernel/DFPortHole.h \
 ../../../../src/domains/mdsdf/kernel/MDSDFPortHole.h \
 ../../../../src/kernel/MatrixParticle.h \
 ../../../../src/kernel/SubMatrix.h ../../../../src/kernel/Matrix.h \
 ../../../../src/kernel/Message.h \
 ../../../../src/kernel/ComplexArrayState.h \
 ../../../../src/kernel/FixArrayState.h \
 ../../../../src/kernel/PrecisionState.h \
 ../../../../src/kernel/FloatArrayState.h \
 ../../../../src/kernel/IntArrayState.h \
 ../../../../src/domains/mdsdf/kernel/MDSDFGeodesic.h \
 ../../../../src/kernel/Geodesic.h \
 ../../../../src/kernel/ParticleStack.h \
 ../../../../src/kernel/IntState.h \
 ../../../../src/kernel/StringState.h \
 ../../../../src/kernel/KnownState.h
MDSDFRepeat.o: MDSDFRepeat.cc MDSDFRepeat.h \
 ../../../../src/domains/mdsdf/kernel/MDSDFStar.h \
 ../../../../src/domains/sdf/kernel/SDFStar.h \
 ../../../../src/kernel/DataFlowStar.h ../../../../src/kernel/Star.h \
 ../../../../src/kernel/Block.h ../../../../src/kernel/NamedObj.h \
 ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/logNew.h \
 ../../../../src/kernel/Error.h ../../../../src/kernel/DataStruct.h \
 ../../../../src/kernel/type.h ../../../../src/kernel/isa.h \
 ../../../../src/kernel/FlagArray.h ../../../../src/kernel/PortHole.h \
 ../../../../src/kernel/dataType.h ../../../../src/kernel/Particle.h \
 ../../../../src/kernel/Fix.h ../../../../src/kernel/ComplexSubset.h \
 ../../../../src/kernel/Attribute.h ../../../../src/kernel/State.h \
 ../../../../src/kernel/InvokeInterp.h \
 ../../../../src/kernel/Fraction.h \
 ../../../../src/domains/sdf/kernel/SDFPortHole.h \
 ../../../../src/kernel/DFPortHole.h \
 ../../../../src/domains/mdsdf/kernel/MDSDFPortHole.h \
 ../../../../src/kernel/MatrixParticle.h \
 ../../../../src/kernel/SubMatrix.h ../../../../src/kernel/Matrix.h \
 ../../../../src/kernel/Message.h \
 ../../../../src/kernel/ComplexArrayState.h \
 ../../../../src/kernel/FixArrayState.h \
 ../../../../src/kernel/PrecisionState.h \
 ../../../../src/kernel/FloatArrayState.h \
 ../../../../src/kernel/IntArrayState.h \
 ../../../../src/domains/mdsdf/kernel/MDSDFGeodesic.h \
 ../../../../src/kernel/Geodesic.h \
 ../../../../src/kernel/ParticleStack.h \
 ../../../../src/kernel/IntState.h
MDSDFTranspose.o: MDSDFTranspose.cc MDSDFTranspose.h \
 ../../../../src/domains/mdsdf/kernel/MDSDFStar.h \
 ../../../../src/domains/sdf/kernel/SDFStar.h \
 ../../../../src/kernel/DataFlowStar.h ../../../../src/kernel/Star.h \
 ../../../../src/kernel/Block.h ../../../../src/kernel/NamedObj.h \
 ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/logNew.h \
 ../../../../src/kernel/Error.h ../../../../src/kernel/DataStruct.h \
 ../../../../src/kernel/type.h ../../../../src/kernel/isa.h \
 ../../../../src/kernel/FlagArray.h ../../../../src/kernel/PortHole.h \
 ../../../../src/kernel/dataType.h ../../../../src/kernel/Particle.h \
 ../../../../src/kernel/Fix.h ../../../../src/kernel/ComplexSubset.h \
 ../../../../src/kernel/Attribute.h ../../../../src/kernel/State.h \
 ../../../../src/kernel/InvokeInterp.h \
 ../../../../src/kernel/Fraction.h \
 ../../../../src/domains/sdf/kernel/SDFPortHole.h \
 ../../../../src/kernel/DFPortHole.h \
 ../../../../src/domains/mdsdf/kernel/MDSDFPortHole.h \
 ../../../../src/kernel/MatrixParticle.h \
 ../../../../src/kernel/SubMatrix.h ../../../../src/kernel/Matrix.h \
 ../../../../src/kernel/Message.h \
 ../../../../src/kernel/ComplexArrayState.h \
 ../../../../src/kernel/FixArrayState.h \
 ../../../../src/kernel/PrecisionState.h \
 ../../../../src/kernel/FloatArrayState.h \
 ../../../../src/kernel/IntArrayState.h \
 ../../../../src/domains/mdsdf/kernel/MDSDFGeodesic.h \
 ../../../../src/kernel/Geodesic.h \
 ../../../../src/kernel/ParticleStack.h \
 ../../../../src/kernel/IntState.h
MDSDFXprism3.o: MDSDFXprism3.cc MDSDFXprism3.h \
 ../../../../src/domains/mdsdf/kernel/MDSDFStar.h \
 ../../../../src/domains/sdf/kernel/SDFStar.h \
 ../../../../src/kernel/DataFlowStar.h ../../../../src/kernel/Star.h \
 ../../../../src/kernel/Block.h ../../../../src/kernel/NamedObj.h \
 ../../../../src/kernel/StringList.h \
 ../../../../src/kernel/miscFuncs.h ../../../../src/kernel/logNew.h \
 ../../../../src/kernel/Error.h ../../../../src/kernel/DataStruct.h \
 ../../../../src/kernel/type.h ../../../../src/kernel/isa.h \
 ../../../../src/kernel/FlagArray.h ../../../../src/kernel/PortHole.h \
 ../../../../src/kernel/dataType.h ../../../../src/kernel/Particle.h \
 ../../../../src/kernel/Fix.h ../../../../src/kernel/ComplexSubset.h \
 ../../../../src/kernel/Attribute.h ../../../../src/kernel/State.h \
 ../../../../src/kernel/InvokeInterp.h \
 ../../../../src/kernel/Fraction.h \
 ../../../../src/domains/sdf/kernel/SDFPortHole.h \
 ../../../../src/kernel/DFPortHole.h \
 ../../../../src/domains/mdsdf/kernel/MDSDFPortHole.h \
 ../../../../src/kernel/MatrixParticle.h \
 ../../../../src/kernel/SubMatrix.h ../../../../src/kernel/Matrix.h \
 ../../../../src/kernel/Message.h \
 ../../../../src/kernel/ComplexArrayState.h \
 ../../../../src/kernel/FixArrayState.h \
 ../../../../src/kernel/PrecisionState.h \
 ../../../../src/kernel/FloatArrayState.h \
 ../../../../src/kernel/IntArrayState.h \
 ../../../../src/domains/mdsdf/kernel/MDSDFGeodesic.h \
 ../../../../src/kernel/Geodesic.h \
 ../../../../src/kernel/ParticleStack.h \
 ../../../../src/kernel/IntState.h \
 ../../../../src/kernel/StringState.h \
 ../../../../src/kernel/KnownState.h
