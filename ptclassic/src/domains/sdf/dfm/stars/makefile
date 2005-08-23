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
# Makefile for Ptolemy DFM stars (SDF stars operating on files)
# @(#)make.template	1.6 	08/18/97
#
# If this file's name is "make.template" and there is no makefile in
# this directory, do
#	cp make.template makefile
#	chmod +w makefile
#	make depend
# This will add the dependencies to the makefile.

ROOT = ../../../../..
VPATH = $(ROOT)/src/domains/sdf/dfm/stars

# get configuration info
CONFIG=$(ROOT)/mk/config-$(PTARCH).mk
include $(CONFIG)

# SDFDIR is the directory containing SDF domain support
SDFDIR = $(ROOT)/src/domains/sdf/kernel
SDFTCLTKDIR = $(ROOT)/src/domains/sdf/tcltk/stars
TCLDIR = $(ROOT)/tcl/include
TYCHODIR = $(ROOT)/src/tysh
PTKLIB = $(ROOT)/src/ptklib
INCL = -I$(SDFDIR) -I$(KERNDIR) -I$(SDFTCLTKDIR) -I$(TYCHODIR) -I$(PTKLIB) \
 -I$(TCL_INCDIR) -I$(TK_INCDIR) $(X11_INCSPEC)

# Library name
PTLIB=libsdfdfmstars.$(LIBSUFFIX)

# source files for all stars, using preprocessor
# base classes/stars go in the front of the list
PL_SRCS= \
 SDFConstString.pl \
 SDFFileEval.pl \
 SDFFileEvalControl.pl \
 SDFFileEvalString.pl \
 SDFFileSource.pl \
 SDFFileShow.pl

TCL_SRCS= \
 fileeval.tcl \
 fileevalsupport.tcl \
 fileevalcontrolsupport.tcl \
 fileshow.tcl

# The .cc files
SRCS=$(PL_SRCS:.pl=.cc)

# object files for all stars
OBJS= $(SRCS:.cc=.o)

# header files for all stars
HDRS= $(SRCS:.cc=.h)

STAR_MK=sdfdfmstars
DOMAIN=SDF
EXTRA_SRCS=$(STAR_MK).c $(PL_SRCS) $(TCL_SRCS)

all:		makefile stars_all

install:	stars_install

# include common definitions, rules
include $(ROOT)/mk/common.mk


# Don't add anything after the next line; makedepend will zap it.
# DO NOT DELETE THIS LINE -- make depend depends on it.

SDFConstString.o: SDFConstString.cc SDFConstString.h \
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
 ../../../../../src/kernel/StringMessage.h \
 ../../../../../src/kernel/Message.h \
 ../../../../../src/kernel/InfString.h
SDFFileEval.o: SDFFileEval.cc SDFFileEval.h \
 ../../../../../src/domains/sdf/tcltk/stars/SDFTclScript.h \
 ../../../../../src/tysh/TclStarIfc.h \
 ../../../../../src/kernel/InfString.h \
 ../../../../../src/kernel/StringList.h \
 ../../../../../src/kernel/miscFuncs.h \
 ../../../../../src/kernel/logNew.h ../../../../../src/kernel/Error.h \
 ../../../../../src/kernel/DataStruct.h \
 ../../../../../src/kernel/type.h \
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
 ../../../../../src/kernel/StringState.h \
 ../../../../../src/kernel/KnownState.h \
 ../../../../../src/kernel/IntState.h \
 ../../../../../src/kernel/StringArrayState.h \
 ../../../../../src/kernel/FileMessage.h \
 ../../../../../src/kernel/Message.h ../../../../../src/tysh/ptk.h \
 ../../../../../tcltk/tcl/include/tcl.h \
 ../../../../../tcltk/tcl/include/tk.h \
 ../../../../../tcltk/tcl/include/itcl.h
SDFFileEvalControl.o: SDFFileEvalControl.cc SDFFileEvalControl.h \
 SDFFileEval.h \
 ../../../../../src/domains/sdf/tcltk/stars/SDFTclScript.h \
 ../../../../../src/tysh/TclStarIfc.h \
 ../../../../../src/kernel/InfString.h \
 ../../../../../src/kernel/StringList.h \
 ../../../../../src/kernel/miscFuncs.h \
 ../../../../../src/kernel/logNew.h ../../../../../src/kernel/Error.h \
 ../../../../../src/kernel/DataStruct.h \
 ../../../../../src/kernel/type.h \
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
 ../../../../../src/kernel/StringState.h \
 ../../../../../src/kernel/KnownState.h \
 ../../../../../src/kernel/IntState.h \
 ../../../../../src/kernel/StringArrayState.h \
 ../../../../../src/tysh/ptk.h ../../../../../tcltk/tcl/include/tcl.h \
 ../../../../../tcltk/tcl/include/tk.h \
 ../../../../../tcltk/tcl/include/itcl.h
SDFFileEvalString.o: SDFFileEvalString.cc SDFFileEvalString.h \
 SDFFileEval.h \
 ../../../../../src/domains/sdf/tcltk/stars/SDFTclScript.h \
 ../../../../../src/tysh/TclStarIfc.h \
 ../../../../../src/kernel/InfString.h \
 ../../../../../src/kernel/StringList.h \
 ../../../../../src/kernel/miscFuncs.h \
 ../../../../../src/kernel/logNew.h ../../../../../src/kernel/Error.h \
 ../../../../../src/kernel/DataStruct.h \
 ../../../../../src/kernel/type.h \
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
 ../../../../../src/kernel/StringState.h \
 ../../../../../src/kernel/KnownState.h \
 ../../../../../src/kernel/IntState.h \
 ../../../../../src/kernel/StringArrayState.h \
 ../../../../../src/kernel/FileMessage.h \
 ../../../../../src/kernel/Message.h ../../../../../src/tysh/ptk.h \
 ../../../../../tcltk/tcl/include/tcl.h \
 ../../../../../tcltk/tcl/include/tk.h \
 ../../../../../tcltk/tcl/include/itcl.h
SDFFileSource.o: SDFFileSource.cc SDFFileSource.h \
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
 ../../../../../src/kernel/FileMessage.h \
 ../../../../../src/kernel/Message.h
SDFFileShow.o: SDFFileShow.cc SDFFileShow.h \
 ../../../../../src/tysh/ptk.h ../../../../../tcltk/tcl/include/tcl.h \
 ../../../../../tcltk/tcl/include/tk.h \
 ../../../../../tcltk/tcl/include/itcl.h \
 ../../../../../src/domains/sdf/tcltk/stars/SDFTclScript.h \
 ../../../../../src/tysh/TclStarIfc.h \
 ../../../../../src/kernel/InfString.h \
 ../../../../../src/kernel/StringList.h \
 ../../../../../src/kernel/miscFuncs.h \
 ../../../../../src/kernel/logNew.h ../../../../../src/kernel/Error.h \
 ../../../../../src/kernel/DataStruct.h \
 ../../../../../src/kernel/type.h \
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
 ../../../../../src/kernel/StringState.h \
 ../../../../../src/kernel/KnownState.h
