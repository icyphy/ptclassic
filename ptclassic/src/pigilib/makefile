# Makefile for Ptolemy graphical interface library
# Version identification:
# @(#)make.template	2.71   02/16/99
#
# Copyright (c) 1990-1999 The Regents of the University of California.
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
# Date of creation: 5/23/90
# Current version by J. Buck
#
# If this file's name is "make.template" and there is no makefile in
# this directory, do
#	cp make.template makefile
#	make depend
# This will add the dependencies to the makefile.
#
# root of source tree
ROOT	      = ../..
# VPATH points to the "real" source directory
VPATH         = $(ROOT)/src/pigilib

# get configuration info
CONFIG=$(ROOT)/mk/config-$(PTARCH).mk
include $(CONFIG)

PTCLHDRS = $(ROOT)/src/ptcl

CHDRS	      = compile.h \
		edit.h \
		err.h \
		exec.h \
		ganttIfc.h \
		handle.h \
		icon.h \
		kernelCalls.h \
		local.h \
		main.h \
		misc.h \
		mkIcon.h \
		mkTerm.h \
		octIfc.h \
		octMacros.h \
		options.h \
		palette.h \
		paramStructs.h \
		pigiLoader.h \
		pthreadsCompat.h \
		ptkNet.h \
		ptkRegisterCmds.h \
		ptkTclIfc.h \
		ptkTkSetup.h \
		sol2compat.h \
		util.h \
		utilities.h \
		vemInterface.h \
		xfunctions.h

CCHDRS        = octObjectClass.h \
		TclObj.h \
		POct.h \
		PVem.h

LIBCSRCS      = main.c \
		util.c \
		utilities.c \
		compile.c \
		paramStructs.c \
		vemInterface.c \
		edit.c \
		exec.c \
		err.c \
		ganttIfc.c \
		octIfc.c \
		icon.c \
		misc.c \
		mkTerm.c \
		mkIcon.c \
		palette.c \
		xfunctions.c \
		options.c \
		ptkNet.c \
		ptkTemplate.c \
		ptkTclIfc.c \
		ptkTkSetup.c \

LIBCPPSRCS    = kernelCalls.cc \
		XError.cc \
		pigiLoader.cc \
		ptkConsoleWindow.cc \
		TclObj.cc \
		POct.cc \
		PVem.cc \
		ganttChart.cc \
		ptkRegisterCmds.cc 

HDRS = $(CHDRS) $(CCHDRS) 

SRCS = $(LIBCSRCS) $(LIBCPPSRCS)

OBJS = $(LIBCSRCS:.c=.o) $(LIBCPPSRCS:.cc=.o)

# HPUX has a hard time handling symbols that are defined both in a static
# library and in a shared library, so, since we have different Error::
# functions for pigiRpc, ptcl and tycho, we need to be sure that
# libpigi, libptcl and libtycho are all static on the hppa.
ifdef USE_CORE_STATIC_LIBS
LIBNONSHARED =	libpigi.a

all:	$(LIBNONSHARED) $(OTHER_OBJS)

install: all $(LIBDIR)/$(LIBNONSHARED)
else
PTLIB =	libpigi.$(LIBSUFFIX)

all:	$(PTLIB) $(OTHER_OBJS)

install: all $(LIBDIR)/$(PTLIB)
endif

# Directory containing utilities for BarGraph
PTKDIR = $(ROOT)/src/ptklib

# include the X11, oct and tcl headers for C++ as well as C
# Note that some X11 installs have bogus (old versions) of the tcl/tk
# headers, thus we must search the tcl/tk includes before the X11 includes
INCL= -I$(KERNDIR) $(PTCOMPAT_INCSPEC) -I$(PTCL_INCDIR) -I$(OCTTOOLS)/include \
 -I$(TCL_INCDIR) -I$(TK_INCDIR) $(X11_INCSPEC) -I$(PTCLHDRS) -I$(PTKDIR)

C_INCL=-I$(OCTTOOLS)/include -I$(PTCL_INCDIR) \
	-I$(ITCL_INCDIR) -I$(ITK_INCDIR) -I$(TCL_INCDIR) -I$(TK_INCDIR) \
	$(PTCOMPAT_INCSPEC) $(X11_INCSPEC) -I$(PTKDIR)

lint:	$(LIBCSRCS)
	lint -u $(INCL) $^ > lint

# include common definitions, rules
include $(ROOT)/mk/common.mk

# Don't add anything after the next line; makedepend will zap it.
# DO NOT DELETE THIS LINE -- make depend depends on it.

main.o: main.c local.h sol2compat.h pthreadsCompat.h \
 ../../src/compat/ptolemy/compat.h ../../octtools/include/ansi.h \
 ../../octtools/include/oct.h ../../octtools/include/port.h \
 ../../octtools/include/list.h ../../octtools/include/copyright.h \
 ../../octtools/include/rpc.h ../../octtools/include/region.h main.h \
 vemInterface.h utilities.h compile.h edit.h exec.h util.h icon.h \
 misc.h ganttIfc.h options.h err.h xfunctions.h pigiLoader.h
util.o: util.c local.h sol2compat.h pthreadsCompat.h \
 ../../src/compat/ptolemy/compat.h ../../octtools/include/ansi.h \
 ../../tcltk/tcl/include/tcl.h ../../octtools/include/oct.h \
 ../../octtools/include/port.h ../../octtools/include/rpc.h \
 ../../octtools/include/list.h ../../octtools/include/copyright.h \
 ../../octtools/include/region.h err.h util.h octIfc.h paramStructs.h \
 vemInterface.h kernelCalls.h icon.h
utilities.o: utilities.c local.h sol2compat.h pthreadsCompat.h \
 ../../src/compat/ptolemy/compat.h ../../octtools/include/ansi.h \
 ../../octtools/include/oct.h ../../octtools/include/port.h \
 ../../octtools/include/list.h ../../octtools/include/copyright.h \
 ../../octtools/include/rpc.h ../../octtools/include/region.h exec.h \
 utilities.h
compile.o: compile.c local.h sol2compat.h pthreadsCompat.h \
 ../../src/compat/ptolemy/compat.h ../../octtools/include/ansi.h \
 ../../octtools/include/oct.h ../../octtools/include/port.h \
 ../../octtools/include/list.h ../../octtools/include/copyright.h \
 ../../octtools/include/rpc.h ../../octtools/include/region.h \
 ../../octtools/include/oh.h compile.h paramStructs.h vemInterface.h \
 util.h err.h main.h octIfc.h octMacros.h ../../src/ptklib/ptk.h \
 kernelCalls.h handle.h
paramStructs.o: paramStructs.c local.h sol2compat.h pthreadsCompat.h \
 ../../src/compat/ptolemy/compat.h ../../octtools/include/ansi.h \
 util.h ../../octtools/include/list.h \
 ../../octtools/include/copyright.h ../../octtools/include/oct.h \
 ../../octtools/include/port.h ../../octtools/include/rpc.h \
 ../../octtools/include/region.h err.h paramStructs.h
vemInterface.o: vemInterface.c local.h sol2compat.h pthreadsCompat.h \
 ../../src/compat/ptolemy/compat.h ../../octtools/include/ansi.h \
 ../../octtools/include/oct.h ../../octtools/include/port.h \
 ../../octtools/include/list.h ../../octtools/include/copyright.h \
 ../../octtools/include/rpc.h ../../octtools/include/region.h \
 ../../octtools/include/rpcApp.h ../../octtools/include/rpcInternal.h \
 ../../octtools/include/errtrap.h ../../octtools/include/st.h \
 vemInterface.h ../../src/ptklib/ptk.h err.h octMacros.h xfunctions.h
edit.o: edit.c local.h sol2compat.h pthreadsCompat.h \
 ../../src/compat/ptolemy/compat.h ../../octtools/include/ansi.h \
 ../../octtools/include/oct.h ../../octtools/include/port.h \
 ../../octtools/include/list.h ../../octtools/include/copyright.h \
 ../../octtools/include/rpc.h ../../octtools/include/region.h \
 ../../octtools/include/oh.h edit.h util.h paramStructs.h \
 vemInterface.h err.h octIfc.h exec.h ../../src/ptklib/ptk.h handle.h \
 kernelCalls.h ganttIfc.h
exec.o: exec.c local.h sol2compat.h pthreadsCompat.h \
 ../../src/compat/ptolemy/compat.h ../../octtools/include/ansi.h \
 ../../octtools/include/oct.h ../../octtools/include/port.h \
 ../../octtools/include/list.h ../../octtools/include/copyright.h \
 ../../octtools/include/rpc.h ../../octtools/include/region.h \
 ../../octtools/include/oh.h exec.h vemInterface.h compile.h octIfc.h \
 paramStructs.h edit.h err.h util.h ganttIfc.h kernelCalls.h \
 ../../src/ptklib/ptk.h handle.h
err.o: err.c local.h sol2compat.h pthreadsCompat.h \
 ../../src/compat/ptolemy/compat.h ../../octtools/include/ansi.h err.h
ganttIfc.o: ganttIfc.c local.h sol2compat.h pthreadsCompat.h \
 ../../src/compat/ptolemy/compat.h ../../octtools/include/ansi.h \
 ../../octtools/include/oct.h ../../octtools/include/port.h \
 ../../octtools/include/rpc.h ../../octtools/include/list.h \
 ../../octtools/include/copyright.h ../../octtools/include/region.h \
 err.h ganttIfc.h ../../octtools/include/oh.h octIfc.h paramStructs.h \
 ../../src/ptklib/ptk.h handle.h
octIfc.o: octIfc.c local.h sol2compat.h pthreadsCompat.h \
 ../../src/compat/ptolemy/compat.h ../../octtools/include/ansi.h \
 ../../octtools/include/oct.h ../../octtools/include/port.h \
 ../../octtools/include/rpc.h ../../octtools/include/list.h \
 ../../octtools/include/copyright.h ../../octtools/include/region.h \
 ../../octtools/include/oh.h octIfc.h paramStructs.h err.h util.h \
 octMacros.h mkIcon.h mkTerm.h vemInterface.h pigiLoader.h \
 kernelCalls.h
icon.o: icon.c local.h sol2compat.h pthreadsCompat.h \
 ../../src/compat/ptolemy/compat.h ../../octtools/include/ansi.h \
 ../../octtools/include/oct.h ../../octtools/include/port.h \
 ../../octtools/include/list.h ../../octtools/include/copyright.h \
 ../../octtools/include/rpc.h ../../octtools/include/region.h \
 ../../octtools/include/utility.h icon.h util.h err.h vemInterface.h \
 palette.h main.h ../../src/ptklib/ptk.h octIfc.h paramStructs.h \
 kernelCalls.h pigiLoader.h handle.h ../../src/ptcl/displayFile.h
misc.o: misc.c local.h sol2compat.h pthreadsCompat.h \
 ../../src/compat/ptolemy/compat.h ../../octtools/include/ansi.h \
 ../../octtools/include/oct.h ../../octtools/include/port.h \
 ../../octtools/include/list.h ../../octtools/include/copyright.h \
 ../../octtools/include/rpc.h ../../octtools/include/region.h \
 ../../octtools/include/utility.h misc.h vemInterface.h util.h \
 octIfc.h paramStructs.h err.h main.h compile.h kernelCalls.h icon.h \
 xfunctions.h ../../src/ptklib/ptk.h handle.h
mkTerm.o: mkTerm.c local.h sol2compat.h pthreadsCompat.h \
 ../../src/compat/ptolemy/compat.h ../../octtools/include/ansi.h \
 ../../octtools/include/rpc.h ../../octtools/include/port.h \
 ../../octtools/include/oct.h ../../octtools/include/list.h \
 ../../octtools/include/copyright.h ../../octtools/include/region.h \
 err.h octMacros.h ../../octtools/include/oh.h mkTerm.h mkIcon.h
mkIcon.o: mkIcon.c local.h sol2compat.h pthreadsCompat.h \
 ../../src/compat/ptolemy/compat.h ../../octtools/include/ansi.h \
 ../../octtools/include/oct.h ../../octtools/include/port.h \
 ../../octtools/include/list.h ../../octtools/include/copyright.h \
 ../../octtools/include/rpc.h ../../octtools/include/region.h \
 ../../octtools/include/oh.h mkIcon.h err.h mkTerm.h octIfc.h \
 paramStructs.h octMacros.h util.h vemInterface.h compile.h \
 kernelCalls.h
palette.o: palette.c local.h sol2compat.h pthreadsCompat.h \
 ../../src/compat/ptolemy/compat.h ../../octtools/include/ansi.h \
 palette.h ../../octtools/include/oct.h ../../octtools/include/port.h \
 ../../octtools/include/rpc.h ../../octtools/include/list.h \
 ../../octtools/include/copyright.h ../../octtools/include/region.h \
 ../../octtools/include/oh.h octMacros.h err.h mkIcon.h util.h \
 octIfc.h paramStructs.h
xfunctions.o: xfunctions.c local.h sol2compat.h pthreadsCompat.h \
 ../../src/compat/ptolemy/compat.h ../../octtools/include/ansi.h \
 ../../tcltk/tcl/include/tcl.h xfunctions.h ../../src/ptklib/ptk.h \
 vemInterface.h ../../octtools/include/oct.h \
 ../../octtools/include/port.h ../../octtools/include/rpc.h \
 ../../octtools/include/list.h ../../octtools/include/copyright.h \
 ../../octtools/include/region.h err.h
options.o: options.c local.h sol2compat.h pthreadsCompat.h \
 ../../src/compat/ptolemy/compat.h ../../octtools/include/ansi.h \
 ../../octtools/include/oct.h ../../octtools/include/port.h \
 ../../octtools/include/list.h ../../octtools/include/copyright.h \
 ../../octtools/include/rpc.h ../../octtools/include/region.h \
 options.h vemInterface.h edit.h exec.h util.h icon.h misc.h err.h \
 kernelCalls.h paramStructs.h
ptkNet.o: ptkNet.c pthreadsCompat.h ../../octtools/include/copyright.h \
 ../../octtools/include/port.h ../../octtools/include/ansi.h \
 ../../src/compat/ptolemy/compat.h ../../octtools/include/utility.h \
 ../../octtools/include/list.h ../../octtools/include/rpc.h \
 ../../octtools/include/oct.h ../../octtools/include/region.h \
 ../../octtools/include/oh.h ../../octtools/include/rpcApp.h \
 ../../octtools/include/rpcInternal.h ../../octtools/include/errtrap.h \
 ../../octtools/include/st.h ptkNet.h
ptkTemplate.o: ptkTemplate.c local.h sol2compat.h pthreadsCompat.h \
 ../../src/compat/ptolemy/compat.h ../../octtools/include/ansi.h \
 ptkTkSetup.h ../../octtools/include/rpcApp.h \
 ../../octtools/include/rpc.h ../../octtools/include/port.h \
 ../../octtools/include/oct.h ../../octtools/include/list.h \
 ../../octtools/include/copyright.h ../../octtools/include/region.h \
 ../../octtools/include/rpcInternal.h ../../octtools/include/errtrap.h \
 ../../octtools/include/st.h main.h
ptkTclIfc.o: ptkTclIfc.c local.h sol2compat.h pthreadsCompat.h \
 ../../src/compat/ptolemy/compat.h ../../octtools/include/ansi.h \
 ptkTclIfc.h ../../tcltk/tcl/include/tcl.h \
 ../../octtools/include/oct.h ../../octtools/include/port.h \
 ../../octtools/include/rpc.h ../../octtools/include/list.h \
 ../../octtools/include/copyright.h ../../octtools/include/region.h \
 ../../src/ptklib/ptk.h exec.h ganttIfc.h handle.h octIfc.h \
 paramStructs.h vemInterface.h
ptkTkSetup.o: ptkTkSetup.c pthreadsCompat.h \
 ../../tcltk/tcl/include/itcl.h ../../tcltk/tcl/include/tcl.h \
 ../../tcltk/tcl/include/itk.h ../../tcltk/tcl/include/itclInt.h \
 ../../tcltk/tcl/include/tclInt.h ../../tcltk/tcl/include/tclRegexp.h \
 ../../tcltk/tcl/include/tk.h local.h sol2compat.h \
 ../../src/compat/ptolemy/compat.h ../../octtools/include/ansi.h \
 ../../octtools/include/rpc.h ../../octtools/include/port.h \
 ../../octtools/include/oct.h ../../octtools/include/list.h \
 ../../octtools/include/copyright.h ../../octtools/include/region.h \
 ../../octtools/include/rpcApp.h ../../octtools/include/rpcInternal.h \
 ../../octtools/include/errtrap.h ../../octtools/include/st.h \
 ../../src/ptklib/ptk.h ptkRegisterCmds.h ptkNet.h err.h xfunctions.h \
 vemInterface.h kernelCalls.h paramStructs.h
kernelCalls.o: kernelCalls.cc ../../src/compat/ptolemy/compat.h \
 ../../src/kernel/InterpUniverse.h ../../src/kernel/Universe.h \
 ../../src/kernel/Galaxy.h ../../src/kernel/Star.h \
 ../../src/kernel/Block.h ../../src/kernel/NamedObj.h \
 ../../src/kernel/StringList.h ../../src/kernel/miscFuncs.h \
 ../../src/kernel/logNew.h ../../src/kernel/Error.h \
 ../../src/kernel/DataStruct.h ../../src/kernel/type.h \
 ../../src/kernel/isa.h ../../src/kernel/FlagArray.h \
 ../../src/kernel/PortHole.h ../../src/kernel/dataType.h \
 ../../src/kernel/Particle.h ../../src/kernel/Fix.h \
 ../../src/kernel/ComplexSubset.h ../../src/kernel/Attribute.h \
 ../../src/kernel/State.h ../../src/kernel/InvokeInterp.h \
 ../../src/kernel/Target.h ../../src/kernel/InterpGalaxy.h \
 ../../src/kernel/DynamicGalaxy.h ../../src/kernel/KnownBlock.h \
 ../../src/kernel/Linker.h ../../src/kernel/Scheduler.h \
 ../../src/kernel/SimControl.h ../../src/kernel/Domain.h \
 ../../src/kernel/KnownTarget.h ../../src/kernel/KnownState.h \
 ../../src/kernel/ConstIters.h ../../src/kernel/pt_fstream.h \
 ../../src/ptcl/PTcl.h err.h icon.h sol2compat.h pthreadsCompat.h \
 ../../octtools/include/list.h ../../octtools/include/ansi.h \
 ../../octtools/include/copyright.h ../../octtools/include/oct.h \
 ../../octtools/include/port.h ../../octtools/include/rpc.h \
 ../../octtools/include/region.h xfunctions.h kernelCalls.h \
 paramStructs.h ../../src/ptklib/ptk.h mkIcon.h
XError.o: XError.cc sol2compat.h pthreadsCompat.h \
 ../../src/kernel/StringList.h ../../src/kernel/miscFuncs.h \
 ../../src/kernel/logNew.h ../../src/kernel/Error.h \
 ../../src/kernel/DataStruct.h ../../src/kernel/type.h \
 ../../src/kernel/Scheduler.h ../../src/kernel/SimControl.h \
 ../../src/kernel/NamedObj.h ../../src/kernel/isa.h \
 ../../src/kernel/FlagArray.h ../../src/kernel/PtGate.h vemInterface.h \
 ../../octtools/include/oct.h ../../octtools/include/port.h \
 ../../octtools/include/ansi.h ../../src/compat/ptolemy/compat.h \
 ../../octtools/include/rpc.h ../../octtools/include/list.h \
 ../../octtools/include/copyright.h ../../octtools/include/region.h \
 exec.h ganttIfc.h xfunctions.h
pigiLoader.o: pigiLoader.cc ../../src/compat/ptolemy/compat.h \
 sol2compat.h pthreadsCompat.h ../../src/kernel/Linker.h \
 ../../src/kernel/Error.h ../../src/kernel/Domain.h \
 ../../src/kernel/StringList.h ../../src/kernel/miscFuncs.h \
 ../../src/kernel/DataStruct.h ../../src/kernel/type.h \
 ../../src/kernel/logNew.h ../../src/kernel/Block.h \
 ../../src/kernel/NamedObj.h ../../src/kernel/isa.h \
 ../../src/kernel/FlagArray.h ../../src/kernel/PortHole.h \
 ../../src/kernel/dataType.h ../../src/kernel/Particle.h \
 ../../src/kernel/Fix.h ../../src/kernel/ComplexSubset.h \
 ../../src/kernel/Attribute.h ../../src/kernel/State.h \
 ../../src/kernel/InvokeInterp.h ../../src/kernel/pt_fstream.h \
 ../../src/ptklib/ptk.h ../../octtools/include/utility.h \
 ../../octtools/include/ansi.h vemInterface.h \
 ../../octtools/include/oct.h ../../octtools/include/port.h \
 ../../octtools/include/rpc.h ../../octtools/include/list.h \
 ../../octtools/include/copyright.h ../../octtools/include/region.h \
 util.h icon.h xfunctions.h err.h kernelCalls.h paramStructs.h \
 pigiLoader.h
ptkConsoleWindow.o: ptkConsoleWindow.cc sol2compat.h pthreadsCompat.h \
 ../../src/ptklib/ptk.h
TclObj.o: TclObj.cc TclObj.h ../../tcltk/tcl/include/tcl.h \
 ../../src/kernel/StringList.h ../../src/kernel/miscFuncs.h \
 ../../src/kernel/logNew.h ../../src/kernel/Error.h \
 ../../src/kernel/DataStruct.h ../../src/kernel/type.h
POct.o: POct.cc POct.h local.h sol2compat.h pthreadsCompat.h \
 ../../src/compat/ptolemy/compat.h ../../octtools/include/ansi.h \
 ../../tcltk/tcl/include/tcl.h TclObj.h ../../src/kernel/InfString.h \
 ../../src/kernel/StringList.h ../../src/kernel/miscFuncs.h \
 ../../src/kernel/logNew.h ../../src/kernel/Error.h \
 ../../src/kernel/DataStruct.h ../../src/kernel/type.h \
 ../../octtools/include/oct.h ../../octtools/include/port.h \
 paramStructs.h ganttIfc.h ../../src/ptcl/PTcl.h \
 ../../src/kernel/NamedObj.h ../../src/kernel/isa.h \
 ../../src/kernel/FlagArray.h ../../src/kernel/SimControl.h \
 vemInterface.h ../../octtools/include/rpc.h \
 ../../octtools/include/list.h ../../octtools/include/copyright.h \
 ../../octtools/include/region.h octIfc.h palette.h util.h exec.h \
 err.h icon.h compile.h xfunctions.h handle.h kernelCalls.h \
 octObjectClass.h
PVem.o: PVem.cc pthreadsCompat.h PVem.h sol2compat.h \
 ../../tcltk/tcl/include/tcl.h TclObj.h ../../octtools/include/oct.h \
 ../../octtools/include/port.h ../../octtools/include/ansi.h \
 ../../src/compat/ptolemy/compat.h ../../octtools/include/rpc.h \
 ../../octtools/include/list.h ../../octtools/include/copyright.h \
 ../../octtools/include/region.h local.h vemInterface.h octIfc.h \
 paramStructs.h handle.h octMacros.h ../../src/kernel/isa.h
ganttChart.o: ganttChart.cc ../../src/compat/ptolemy/compat.h \
 sol2compat.h pthreadsCompat.h ../../src/ptcl/PTcl.h \
 ../../src/kernel/NamedObj.h ../../src/kernel/StringList.h \
 ../../src/kernel/miscFuncs.h ../../src/kernel/logNew.h \
 ../../src/kernel/Error.h ../../src/kernel/DataStruct.h \
 ../../src/kernel/type.h ../../src/kernel/isa.h \
 ../../src/kernel/FlagArray.h ../../src/kernel/SimControl.h \
 ../../src/ptcl/ganttChart.h
ptkRegisterCmds.o: ptkRegisterCmds.cc ../../src/ptcl/PTcl.h \
 ../../src/kernel/NamedObj.h ../../src/kernel/StringList.h \
 ../../src/kernel/miscFuncs.h ../../src/kernel/logNew.h \
 ../../src/kernel/Error.h ../../src/kernel/DataStruct.h \
 ../../src/kernel/type.h ../../src/kernel/isa.h \
 ../../src/kernel/FlagArray.h ../../src/kernel/SimControl.h POct.h \
 local.h sol2compat.h pthreadsCompat.h \
 ../../src/compat/ptolemy/compat.h ../../octtools/include/ansi.h \
 TclObj.h ../../src/kernel/InfString.h ../../octtools/include/oct.h \
 ../../octtools/include/port.h paramStructs.h ganttIfc.h PVem.h \
 ../../octtools/include/rpc.h ../../octtools/include/list.h \
 ../../octtools/include/copyright.h ../../octtools/include/region.h \
 ptkRegisterCmds.h ../../tcltk/tcl/include/tk.h ptkTclIfc.h \
 ../../src/ptklib/ptk.h
