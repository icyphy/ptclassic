# Makefile for Ptolemy Tycho system, used to produce a tysh binary
#
# @(#)make.template	1.45    02/22/99
# Authors: E. A. Lee, Christopher Hylands, Jose Luis Pino
#
# Copyright (c) 1991-1999 The Regents of the University of California.
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
##########################################################################

# If this file's name is "make.template" and there is no makefile in
# this directory, do
#	cp make.template makefile
#	make depend
# This will add the dependencies to the makefile.

# Root of Ptolemy directory
ROOT =		../..
# Path (absolute or relative) to the object directory root
OBJDIR =	..
# True source directory
VPATH =		$(ROOT)/src/tysh

# get configuration info
CONFIG =	$(ROOT)/mk/config-$(PTARCH).mk
include $(CONFIG)

# $PTOLEMY/mk/ptbin.mk checks $(PITCL)
# If PITCL is set, then we use libpitcl instead of libptcl
PITCL = 1

-include override.mk

# Directory containing utilities for BarGraph and XYPlot
PTKDIR = 	$(ROOT)/src/ptklib

INCL = 		-I$(KERNDIR) -I$(PTCL_INCDIR) \
		-I$(ITCL_INCDIR) \
		-I$(TCL_INCDIR) -I$(TK_INCDIR) $(X11_INCSPEC) -I$(PTKDIR) \
		-I$(ROOT)/src/kernel \
		-I$(ROOT)/src/compat/ptolemy

# For development period, disable optimization
OPTIMIZER =

# Sources
SRCS = \
  BarGraph.cc \
  TyConsole.cc \
  TyError.cc \
  TyMain.cc \
  TyTcl.cc \
  TclObj.cc \
  TclStarIfc.cc \
  XYPlot.cc \
  SetSigHandle.cc \
  SigHandle.cc

# Header files
HDRS = \
  BarGraph.h \
  TyConsole.h \
  TyTcl.h \
  pthreadsCompat.h \
  ptk.h \
  sol2compat.h \
  TclObj.h \
  TclStarIfc.h \
  XYPlot.h \
  SetSigHandle.h \
  SigHandle.h 

# Used by make checkjunk
OTHERSRCS = \
  README

# Objects
OBJS =		$(SRCS:.cc=.o)

# HPUX has a hard time handling symbols that are defined both in a static
# library and in a shared library, so, since we have different Error::
# functions for pigiRpc, ptcl and tycho, we need to be sure that
# libpigi, libptcl and libtycho are all static on the hppa.
ifdef USE_CORE_STATIC_LIBS
LIBNONSHARED =		libtysh.a
PT_DEPEND += 	$(LIBDIR)/$(LIBNONSHARED)
INSTALL += 	$(LIBDIR)/$(LIBNONSHARED)
else
PTLIB =		libtysh.$(LIBSUFFIX)
PT_DEPEND += 	$(LIBDIR)/$(PTLIB)
INSTALL += 	$(LIBDIR)/$(PTLIB)
endif

ifndef BASENAME
ifndef PIGI
	ALLBINARIES =	1
	BASENAME =	tysh
	PIGI =		tysh
endif
endif

ifndef VERSION
	VERSION =	0.2.1devel.$(BASENAME)_Ptolemy0.7.1devel
endif

TK = 		1

PIGI_OBJS += 	-ltysh


 
# ptbin.mk has the default rule that creates a binary and the all: rule
include $(ROOT)/mk/ptbin.mk

# include common definitions, rules
include $(ROOT)/mk/common.mk

# Don't add anything after the next line; makedepend will zap it.
# DO NOT DELETE THIS LINE -- make depend depends on it.

BarGraph.o: BarGraph.cc BarGraph.h ../../src/kernel/Block.h \
 ../../src/kernel/NamedObj.h ../../src/kernel/StringList.h \
 ../../src/kernel/miscFuncs.h ../../src/kernel/logNew.h \
 ../../src/kernel/Error.h ../../src/kernel/DataStruct.h \
 ../../src/kernel/type.h ../../src/kernel/isa.h \
 ../../src/kernel/FlagArray.h ../../src/kernel/PortHole.h \
 ../../src/kernel/dataType.h ../../src/kernel/Particle.h \
 ../../src/kernel/Fix.h ../../src/kernel/ComplexSubset.h \
 ../../src/kernel/Attribute.h ../../src/kernel/State.h \
 ../../src/kernel/InvokeInterp.h ../../src/kernel/InfString.h ptk.h \
 ../../tcltk/tcl/include/tcl.h ../../tcltk/tcl/include/tk.h \
 ../../tcltk/tcl/include/itcl.h ../../src/ptklib/ptkBarGraph.h \
 ../../src/kernel/ieee.h ../../src/ptklib/ptkTclCommands.h
TyConsole.o: TyConsole.cc TyConsole.h ptk.h \
 ../../tcltk/tcl/include/tcl.h ../../tcltk/tcl/include/tk.h \
 ../../tcltk/tcl/include/itcl.h ../../src/ptcl/PTcl.h \
 ../../src/kernel/NamedObj.h ../../src/kernel/StringList.h \
 ../../src/kernel/miscFuncs.h ../../src/kernel/logNew.h \
 ../../src/kernel/Error.h ../../src/kernel/DataStruct.h \
 ../../src/kernel/type.h ../../src/kernel/isa.h \
 ../../src/kernel/FlagArray.h ../../src/kernel/SimControl.h TyTcl.h \
 sol2compat.h pthreadsCompat.h TclObj.h ../../src/kernel/SimAction.h \
 ../../src/kernel/InfString.h
TyError.o: TyError.cc ../../src/kernel/Error.h \
 ../../src/kernel/Scheduler.h ../../src/kernel/StringList.h \
 ../../src/kernel/miscFuncs.h ../../src/kernel/logNew.h \
 ../../src/kernel/DataStruct.h ../../src/kernel/type.h \
 ../../src/kernel/SimControl.h ../../src/kernel/NamedObj.h \
 ../../src/kernel/isa.h ../../src/kernel/FlagArray.h \
 ../../src/kernel/PtGate.h ../../src/kernel/InfString.h ptk.h \
 ../../tcltk/tcl/include/tcl.h ../../tcltk/tcl/include/tk.h \
 ../../tcltk/tcl/include/itcl.h
TyMain.o: TyMain.cc ../../src/ptcl/PTcl.h ../../src/kernel/NamedObj.h \
 ../../src/kernel/StringList.h ../../src/kernel/miscFuncs.h \
 ../../src/kernel/logNew.h ../../src/kernel/Error.h \
 ../../src/kernel/DataStruct.h ../../src/kernel/type.h \
 ../../src/kernel/isa.h ../../src/kernel/FlagArray.h \
 ../../src/kernel/SimControl.h ../../src/kernel/Linker.h \
 SetSigHandle.h ../../src/kernel/InfString.h TyConsole.h ptk.h \
 ../../tcltk/tcl/include/tk.h ../../tcltk/tcl/include/itcl.h TyTcl.h \
 sol2compat.h pthreadsCompat.h TclObj.h ../../src/kernel/SimAction.h
TyTcl.o: TyTcl.cc TyTcl.h sol2compat.h pthreadsCompat.h \
 ../../tcltk/tcl/include/tcl.h ../../tcltk/tcl/include/tk.h TclObj.h \
 ../../src/kernel/isa.h
TclObj.o: TclObj.cc TclObj.h ../../tcltk/tcl/include/tcl.h \
 ../../src/kernel/StringList.h ../../src/kernel/miscFuncs.h \
 ../../src/kernel/logNew.h ../../src/kernel/Error.h \
 ../../src/kernel/DataStruct.h ../../src/kernel/type.h
TclStarIfc.o: TclStarIfc.cc ../../src/kernel/Block.h \
 ../../src/kernel/NamedObj.h ../../src/kernel/StringList.h \
 ../../src/kernel/miscFuncs.h ../../src/kernel/logNew.h \
 ../../src/kernel/Error.h ../../src/kernel/DataStruct.h \
 ../../src/kernel/type.h ../../src/kernel/isa.h \
 ../../src/kernel/FlagArray.h ../../src/kernel/PortHole.h \
 ../../src/kernel/dataType.h ../../src/kernel/Particle.h \
 ../../src/kernel/Fix.h ../../src/kernel/ComplexSubset.h \
 ../../src/kernel/Attribute.h ../../src/kernel/State.h \
 ../../src/kernel/InvokeInterp.h TclStarIfc.h \
 ../../src/kernel/InfString.h ptk.h ../../tcltk/tcl/include/tcl.h \
 ../../tcltk/tcl/include/tk.h ../../tcltk/tcl/include/itcl.h \
 ../../src/ptklib/ptkTclCommands.h
XYPlot.o: XYPlot.cc ../../src/kernel/InfString.h \
 ../../src/kernel/StringList.h ../../src/kernel/miscFuncs.h \
 ../../src/kernel/logNew.h ../../src/kernel/Error.h \
 ../../src/kernel/DataStruct.h ../../src/kernel/type.h XYPlot.h \
 ../../src/kernel/Block.h ../../src/kernel/NamedObj.h \
 ../../src/kernel/isa.h ../../src/kernel/FlagArray.h \
 ../../src/kernel/PortHole.h ../../src/kernel/dataType.h \
 ../../src/kernel/Particle.h ../../src/kernel/Fix.h \
 ../../src/kernel/ComplexSubset.h ../../src/kernel/Attribute.h \
 ../../src/kernel/State.h ../../src/kernel/InvokeInterp.h ptk.h \
 ../../tcltk/tcl/include/tcl.h ../../tcltk/tcl/include/tk.h \
 ../../tcltk/tcl/include/itcl.h ../../src/ptklib/ptkPlot_defs.h \
 ../../src/ptklib/ptkPlot.h ../../src/kernel/ieee.h \
 ../../src/ptklib/ptkTclCommands.h
SetSigHandle.o: SetSigHandle.cc ../../src/compat/ptolemy/compat.h \
 SigHandle.h ../../src/kernel/ptsignals.h SetSigHandle.h
SigHandle.o: SigHandle.cc ../../src/compat/ptolemy/compat.h \
 ../../tcltk/tcl/include/tcl.h SigHandle.h \
 ../../src/kernel/ptsignals.h ../../src/kernel/InfString.h \
 ../../src/kernel/StringList.h ../../src/kernel/miscFuncs.h \
 ../../src/kernel/logNew.h ../../src/kernel/Error.h \
 ../../src/kernel/DataStruct.h ../../src/kernel/type.h
