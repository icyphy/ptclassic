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
# Makefile for Ptolemy MDSDF tcl stars
# @(#)make.template	1.7 02/17/99
#
# If this file's name is "make.template" and there is no makefile in
# this directory, do
#	cp make.template makefile
#	chmod +w makefile
#	make depend
# This will add the dependencies to the makefile.

ROOT = ../../../../..
VPATH = $(ROOT)/src/domains/mdsdf/tcltk/stars

# get configuration info
CONFIG=$(ROOT)/mk/config-$(PTARCH).mk
include $(CONFIG)

# MDSDFDIR is the directory containing MDSDF domain support
MDSDFDIR = $(ROOT)/src/domains/mdsdf/kernel
SDFDIR = $(ROOT)/src/domains/sdf/kernel
TCLDIR = $(ROOT)/tcl/include
PTKDIR = $(ROOT)/src/ptklib
INCL = -I$(SDFDIR) -I$(MDSDFDIR) -I$(KERNDIR) -I$(PTKDIR) \
 -I$(TCL_INCDIR) -I$(TK_INCDIR) $(X11_INCSPEC)

# Library name
PTLIB=libmdsdftclstars.$(LIBSUFFIX)

# source files for all stars, using preprocessor
PL_SRCS= \
 MDSDFTclScript.pl \
 MDSDFTkShowValues.pl \
 MDSDFTkText.pl

TCL_SRCS= \
 tkMeter.tcl \
 tkScript.tcl \
 tkShowValues.tcl \
 tkSlider.tcl \
 tkText.tcl

# The .cc files
SRCS=$(PL_SRCS:.pl=.cc)

# object files for all stars
OBJS= $(SRCS:.cc=.o)

# header files for all stars
HDRS= $(SRCS:.cc=.h)

# extra source files: tcl/tk script files
SCRIPTS= 

STAR_MK=mdsdftclstars
DOMAIN=MDSDF_TCL
DOMAIN_2=MDSDF
EXTRA_SRCS=$(STAR_MK).c $(PL_SRCS) $(SCRIPTS) $(TCL_SRCS)

all:		makefile stars_all

install:	stars_install


# include common definitions, rules
include $(ROOT)/mk/common.mk


# Don't add anything after the next line; makedepend will zap it.
# DO NOT DELETE THIS LINE -- make depend depends on it.

MDSDFTclScript.o: MDSDFTclScript.cc MDSDFTclScript.h \
 ../../../../../src/ptklib/TclStarIfc.h \
 ../../../../../src/kernel/InfString.h \
 ../../../../../src/kernel/StringList.h \
 ../../../../../src/kernel/miscFuncs.h \
 ../../../../../src/kernel/logNew.h ../../../../../src/kernel/Error.h \
 ../../../../../src/kernel/DataStruct.h \
 ../../../../../src/kernel/type.h \
 ../../../../../src/domains/mdsdf/kernel/MDSDFStar.h \
 ../../../../../src/domains/sdf/kernel/SDFStar.h \
 ../../../../../src/kernel/DataFlowStar.h \
 ../../../../../src/kernel/Star.h ../../../../../src/kernel/Block.h \
 ../../../../../src/kernel/NamedObj.h ../../../../../src/kernel/isa.h \
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
 ../../../../../src/domains/mdsdf/kernel/MDSDFPortHole.h \
 ../../../../../src/kernel/MatrixParticle.h \
 ../../../../../src/kernel/SubMatrix.h \
 ../../../../../src/kernel/Matrix.h \
 ../../../../../src/kernel/Message.h \
 ../../../../../src/kernel/ComplexArrayState.h \
 ../../../../../src/kernel/FixArrayState.h \
 ../../../../../src/kernel/PrecisionState.h \
 ../../../../../src/kernel/FloatArrayState.h \
 ../../../../../src/kernel/IntArrayState.h \
 ../../../../../src/domains/mdsdf/kernel/MDSDFGeodesic.h \
 ../../../../../src/kernel/Geodesic.h \
 ../../../../../src/kernel/ParticleStack.h \
 ../../../../../src/kernel/IntState.h \
 ../../../../../src/kernel/StringState.h \
 ../../../../../src/kernel/KnownState.h
MDSDFTkShowValues.o: MDSDFTkShowValues.cc MDSDFTkShowValues.h \
 ../../../../../src/ptklib/ptk.h MDSDFTclScript.h \
 ../../../../../src/ptklib/TclStarIfc.h \
 ../../../../../src/kernel/InfString.h \
 ../../../../../src/kernel/StringList.h \
 ../../../../../src/kernel/miscFuncs.h \
 ../../../../../src/kernel/logNew.h ../../../../../src/kernel/Error.h \
 ../../../../../src/kernel/DataStruct.h \
 ../../../../../src/kernel/type.h \
 ../../../../../src/domains/mdsdf/kernel/MDSDFStar.h \
 ../../../../../src/domains/sdf/kernel/SDFStar.h \
 ../../../../../src/kernel/DataFlowStar.h \
 ../../../../../src/kernel/Star.h ../../../../../src/kernel/Block.h \
 ../../../../../src/kernel/NamedObj.h ../../../../../src/kernel/isa.h \
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
 ../../../../../src/domains/mdsdf/kernel/MDSDFPortHole.h \
 ../../../../../src/kernel/MatrixParticle.h \
 ../../../../../src/kernel/SubMatrix.h \
 ../../../../../src/kernel/Matrix.h \
 ../../../../../src/kernel/Message.h \
 ../../../../../src/kernel/ComplexArrayState.h \
 ../../../../../src/kernel/FixArrayState.h \
 ../../../../../src/kernel/PrecisionState.h \
 ../../../../../src/kernel/FloatArrayState.h \
 ../../../../../src/kernel/IntArrayState.h \
 ../../../../../src/domains/mdsdf/kernel/MDSDFGeodesic.h \
 ../../../../../src/kernel/Geodesic.h \
 ../../../../../src/kernel/ParticleStack.h \
 ../../../../../src/kernel/IntState.h \
 ../../../../../src/kernel/StringState.h \
 ../../../../../src/kernel/KnownState.h
MDSDFTkText.o: MDSDFTkText.cc MDSDFTkText.h \
 ../../../../../src/ptklib/ptk.h MDSDFTkShowValues.h MDSDFTclScript.h \
 ../../../../../src/ptklib/TclStarIfc.h \
 ../../../../../src/kernel/InfString.h \
 ../../../../../src/kernel/StringList.h \
 ../../../../../src/kernel/miscFuncs.h \
 ../../../../../src/kernel/logNew.h ../../../../../src/kernel/Error.h \
 ../../../../../src/kernel/DataStruct.h \
 ../../../../../src/kernel/type.h \
 ../../../../../src/domains/mdsdf/kernel/MDSDFStar.h \
 ../../../../../src/domains/sdf/kernel/SDFStar.h \
 ../../../../../src/kernel/DataFlowStar.h \
 ../../../../../src/kernel/Star.h ../../../../../src/kernel/Block.h \
 ../../../../../src/kernel/NamedObj.h ../../../../../src/kernel/isa.h \
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
 ../../../../../src/domains/mdsdf/kernel/MDSDFPortHole.h \
 ../../../../../src/kernel/MatrixParticle.h \
 ../../../../../src/kernel/SubMatrix.h \
 ../../../../../src/kernel/Matrix.h \
 ../../../../../src/kernel/Message.h \
 ../../../../../src/kernel/ComplexArrayState.h \
 ../../../../../src/kernel/FixArrayState.h \
 ../../../../../src/kernel/PrecisionState.h \
 ../../../../../src/kernel/FloatArrayState.h \
 ../../../../../src/kernel/IntArrayState.h \
 ../../../../../src/domains/mdsdf/kernel/MDSDFGeodesic.h \
 ../../../../../src/kernel/Geodesic.h \
 ../../../../../src/kernel/ParticleStack.h \
 ../../../../../src/kernel/IntState.h \
 ../../../../../src/kernel/StringState.h \
 ../../../../../src/kernel/KnownState.h
