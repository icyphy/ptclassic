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
#
# stars.mk :: Common definitions of star and target subsets.
# Version: $Id$
#
# Eventually this file will be automatically generated.
# But for now, we do it by hand.
#
# The following makefile symbols should be defined:
#
# ROOT		the root of the Ptolemy tree (e.g., $PTOLEMY).  Typically
#		a relative path.
# CROOT		the root of the custom Ptolemy tree used to define
#		CUSTOM_DIRS, which is used by the mkPtolemyTree script
# OBJDIR	the root of the object file tree for this arch.
#		it can be either relative to current directory or absolute.
# PTARCH	architecture of the current computer system returned
#		by the ptarch script
#
# Problem with this file: it does not make clear which domains need
# which libraries, so there may be a bit too much searching and remaking
# when subset Ptolemies are built.  The symbols STAR_LIBFILES and
# STAR_LIBS include ALL libraries.  It is OK to make subset Ptolemy builds
# depend on these symbols and search all libraries, but it slows things
# down a bit.
#
# CGC tcl stars are included because they don't pull in tk.
# SDF tcl stars are omitted because they do.

# Note that you must explicitly include any target .o files in TARGETS
# for architectures that don't use shared libraries.  If you don't, then
# you won't see these targets in non-shared library architectures.  For
# an example, see the CG56 target section below

# Stub files that pull in the stars.

#STARS =
#TARGETS =
#LIBS =
#LIBFILES =
#PALETTES =
#PT_DEPEND = 

ifndef DEFAULT_DOMAIN
	DEFAULT_DOMAIN = SDF
endif 

# Matlab settings
include $(ROOT)/mk/matlab.mk

# Mathematica settings
include $(ROOT)/mk/mathematica.mk

C50T = $(OBJDIR)/domains/c50/targets
CG56T = $(OBJDIR)/domains/cg56/targets
CGCT = $(OBJDIR)/domains/cgc/targets
CGCTCLT = $(OBJDIR)/domains/cgc/tcltk/targets
CGCVIST = $(OBJDIR)/domains/cgc/vis/targets
CGT = $(OBJDIR)/domains/cg/targets
SDFT = $(OBJDIR)/domains/sdf/targets
VHDLT = $(OBJDIR)/domains/vhdl/targets

BDFDIR = $(CROOT)/src/domains/bdf
CGDDFDIR = $(CROOT)/src/domains/cg
CGDIR = $(CROOT)/src/domains/cg
CGCDIR = $(CROOT)/src/domains/cgc
C50DIR = $(CROOT)/src/domains/c50
CG56DIR = $(CROOT)/src/domains/cg56
DDFDIR = $(CROOT)/src/domains/ddf
DEDIR = $(CROOT)/src/domains/de
HOFDIR = $(CROOT)/src/domains/hof
IPUSDIR = $(CROOT)/src/domains/ipus
MDSDFDIR = $(CROOT)/src/domains/mdsdf
PNDIR = $(CROOT)/src/domains/pn
SDFDIR = $(CROOT)/src/domains/sdf
SRDIR = $(CROOT)/src/domains/sr
VHDLBDIR = $(CROOT)/src/domains/vhdlb
VHDLDIR = $(CROOT)/src/domains/vhdl

# Tcl/Tk stars can be used in pigiRpc but not ptcl, for this we define TK

ifndef TK
	# We can't include the ipus domain since the non-tk ipus stars need 
	# Blackboard* symbols, which are defined in the tk ipus stars.
	# Set up definitions for stars.mk
	IPUS=

	CGCTK=
	CODESIGN=
	HOF=
	SDFTK=
	SDFDFM=
endif

# Synchronous/Reactive domain

ifdef SR
	PALETTES += PTOLEMY/src/domains/sr/icons/sr.pal
	STARS += $(LIBDIR)/srstars.o
	LIBS += -lsrstars -lsr
	LIBFILES += $(LIBDIR)/libsrstars.$(LIBSUFFIX) \
		$(LIBDIR)/libsr.$(LIBSUFFIX)
	CUSTOM_DIRS += $(SRDIR)/stars $(SRDIR)/kernel 
endif

# Motorola DSP assembly code generation domain

ifdef CG56
	CUSTOM_DIRS += $(CG56DIR)/kernel $(CG56DIR)/stars \
		$(CG56DIR)/targets $(CG56DIR)/dsp/stars
	# must bring in the parallel schedulers for multi-cg56 targets
	CGFULL = 1
	PALETTES += PTOLEMY/src/domains/cg56/icons/main.pal
	STARS += $(LIBDIR)/cg56dspstars.o $(LIBDIR)/cg56stars.o
	LIBS += -lcg56dspstars -lcg56stars -lcg56
	LIBFILES += $(LIBDIR)/libcg56dspstars.$(LIBSUFFIX) \
		$(LIBDIR)/libcg56stars.$(LIBSUFFIX) \
		$(LIBDIR)/libcg56.$(LIBSUFFIX)
	ifeq ($(USE_SHARED_LIBS),yes) 
		LIBS += -lcg56targets
		LIBFILES += $(LIBDIR)/libcg56targets.$(LIBSUFFIX)
	else
		TARGETS += $(CG56T)/Sim56Target.o $(CG56T)/S56XTarget.o \
			$(CG56T)/Sub56Target.o \
			$(CG56T)/CG56MultiSimTarget.o \
			$(CG56T)/CG56MultiSimSend.o \
			$(CG56T)/CG56MultiSimReceive.o \
			$(CG56T)/CG56XCReceive.o $(CG56T)/CG56XCSend.o \
			$(CG56T)/CG56XCPeek.o $(CG56T)/CG56XCPoke.o \
			$(CG56T)/CGCXReceive.o $(CG56T)/CGCXSend.o \
			$(CG56T)/CGCXPeek.o $(CG56T)/CGCXPoke.o \
	 		$(CG56T)/CGCXSynchComm.o \
			$(CG56T)/CG56XCSynchComm.o \
			$(CG56T)/CGCXAsynchComm.o \
			$(CG56T)/CG56XCAsynchComm.o \
			$(CG56T)/CGCXBase.o  
	endif
	# Window and RaisedCosine stars in cg56/dsp/stars need Cephes Library
	CEPHESLIB = 1
	# CG56 targets need CGCStar
	CGCLIB = 1
endif

# Texas Instruments TMS320C50 Code Generation Domain

ifdef C50
	CUSTOM_DIRS += $(C50DIR)/kernel $(C50DIR)/stars \
		$(C50DIR)/targets $(C50DIR)/dsp/stars
	# must bring in the parallel schedulers for multi-c50 targets
	# CGFULL = 1
	CG = 1
	PALETTES += PTOLEMY/src/domains/c50/icons/main.pal
	STARS += $(LIBDIR)/c50dspstars.o $(LIBDIR)/c50stars.o
	LIBS += -lc50dspstars -lc50stars -lc50
	LIBFILES += $(LIBDIR)/libc50dspstars.$(LIBSUFFIX) \
		$(LIBDIR)/libc50stars.$(LIBSUFFIX) \
		$(LIBDIR)/libc50.$(LIBSUFFIX)
	ifeq ($(USE_SHARED_LIBS),yes) 
		LIBS += -lc50targets
		LIBFILES += $(LIBDIR)/libc50targets.$(LIBSUFFIX)
	else
		TARGETS += $(CG56T)/DSKC50Target.o \
			$(CG56T)/SubC50Target.o
	endif
	# Window and RaisedCosine star in c50/dsp/stars need Cephes Library
	CEPHESLIB = 1
	# C50 targets need CGCStar
	# CGCLIB = 1
endif

ifdef CGDDF
	CUSTOM_DIRS += $(CGDDFDIR)/ddfScheduler
	CGDDFLIB = 1
	DDF = 1
	CGC = 1
	CGFULL = 1
endif

ifdef CGCFULL
	CGC = 1
	CGFULL = 1
	NOWAM = 1
	ifdef TK
		CGCTK = 1
	endif
	CGCVIS = 1
endif

# CGC VIS demonstrations will only run under Solaris 2.5 under cfront,
# but the CGC VIS subdomain will build under any architecture.
ifdef CGCVIS
	# Don't build the CGC VIS subdomain on a non-Solaris machine
	ifeq ("$(filter sol% ,$(PTARCH))","")
		CGCVIS =
	endif
endif

ifdef CGFULL
	CG = 1
	CGPAR = 1
endif

ifdef DDF
	CUSTOM_DIRS += $(DDFDIR)/kernel $(DDFDIR)/stars 
	SDFLIB = 1
	# There are many DDF code generators and schedulers
	# (HuScheduler, dcScheduler, ddfScheduler, dlScheduler, etc.)
	# but none are currently being built.  When they are, add
	# ifdef CG
	#	LIBS +=
	#	LIBFILES +=
	# endif
	PALETTES += PTOLEMY/src/domains/ddf/icons/ddf.pal
	STARS += $(LIBDIR)/ddfstars.o
	LIBS += -lddfstars -lddf
	LIBFILES += $(LIBDIR)/libddfstars.$(LIBSUFFIX) \
		$(LIBDIR)/libddf.$(LIBSUFFIX)
endif

ifdef VHDL
	CUSTOM_DIRS += $(VHDLDIR)/kernel $(VHDLDIR)/stars  $(VHDLDIR)/targets
	CG = 1
	SDFLIB = 1
	CGCLIB = 1
	PALETTES += PTOLEMY/src/domains/vhdl/icons/vhdl.pal
	STARS += $(LIBDIR)/vhdlstars.o
	LIBS += -lvhdlstars -lvhdl
	LIBFILES += $(LIBDIR)/libvhdlstars.$(LIBSUFFIX) \
		$(LIBDIR)/libvhdl.$(LIBSUFFIX) 
	ifeq ($(USE_SHARED_LIBS),yes) 
		LIBS += -lvhdltargets
		LIBFILES += $(LIBDIR)/libvhdltargets.$(LIBSUFFIX)
	else
		TARGETS += $(VHDLT)/CGCVReceive.o \
			$(VHDLT)/CGCVSend.o \
			$(VHDLT)/CGCVSynchComm.o \
			$(VHDLT)/SimMTTarget.o \
			$(VHDLT)/SimVSSTarget.o \
			$(VHDLT)/StructTarget.o \
			$(VHDLT)/SynthTarget.o \
			$(VHDLT)/VHDLCReceive.o \
			$(VHDLT)/VHDLCSend.o \
			$(VHDLT)/VHDLCSynchComm.o
	endif

	# Window star in vhdl/stars needs the Cephes Library
	CEPHESLIB = 1
endif

ifdef VHDLB
	CUSTOM_DIRS += $(VHDLBDIR)/kernel $(VHDLBDIR)/stars
	CG = 1
	SDFLIB = 1
	PALETTES += PTOLEMY/src/domains/vhdlb/icons/vhdlb.pal
	STARS += $(LIBDIR)/vhdlbstars.o
	LIBS += -lvhdlbstars -lvhdlb
	LIBFILES += $(LIBDIR)/libvhdlbstars.$(LIBSUFFIX) \
		$(LIBDIR)/libvhdlb.$(LIBSUFFIX)
endif

ifdef MDSDF
	CUSTOM_DIRS += $(MDSDFDIR)/kernel $(MDSDFDIR)/stars
	PALETTES += PTOLEMY/src/domains/mdsdf/icons/mdsdf.pal
	SDFLIB = 1
	STARS +=  $(LIBDIR)/mdsdfstars.o
	LIBS += -lmdsdfstars -lmdsdf
	LIBFILES += $(LIBDIR)/libmdsdfstars.$(LIBSUFFIX) \
		$(LIBDIR)/libmdsdf.$(LIBSUFFIX)
	ifdef TK 
		CUSTOM_DIRS += $(MDSDFDIR)/tcltk/stars
		STARS += $(LIBDIR)/mdsdftclstars.o
		LIBFILES += $(LIBDIR)/libmdsdftclstars.$(LIBSUFFIX)
		LIBS += -lmdsdftclstars
	endif
endif

ifdef PN
	CUSTOM_DIRS += $(PNDIR)/kernel $(PNDIR)/stars 
	SDFLIB = 1
	PALETTES += PTOLEMY/src/domains/pn/icons/pn.pal
	# PN is only supported under Sun, Solaris and HP-UX operating systems,
	# matched by patterns sun% and sol% and hppa% respectively
	ifneq ("$(filter sun% sol% hppa% ,$(PTARCH))","")
		STARS += $(LIBDIR)/pnstars.o
		LIBS += -lpnstars -lpn
		LIBFILES += $(LIBDIR)/libpnstars.$(LIBSUFFIX) \
			$(LIBDIR)/libpn.$(LIBSUFFIX)
		ifneq ("$(filter sun% sol% ,$(PTARCH))","")
			# POSIX thread library from Florida State University.
			LIBFILES += $(ROOT)/thread/lib.$(PTARCH)/libgthreads.a
			LIBS += -lposixthread \
				-L$(ROOT)/thread/lib.$(PTARCH) -lgthreads
		else
			# HPUX 
			LIBS += -lposixthread \
				-L/opt/dce/lib/libcma.a	-lcma
		endif
		LIBFILES += $(LIBDIR)/libposixthread.$(LIBSUFFIX)
	endif
endif

# We can't include the ipus domain since the non-tk ipus stars need 
# Blackboard* symbols, which are defined in the tk ipus stars.
# Set up definitions for stars.mk

ifdef IPUS
	ifdef TK
		CUSTOM_DIRS += $(IPUSDIR)/icp/kernel $(IPUSDIR)/kernel \
				$(IPUSDIR)/stars $(IPUSDIR)/tcltk/stars 
		PALETTES += PTOLEMY/src/domains/ipus/icons/ipus.pal
		STARS += $(LIBDIR)/ipustclstars.o $(LIBDIR)/ipusstars.o
		LIBS += -lipustclstars -lipusstars -lipus -licp
		LIBFILES += $(LIBDIR)/libipustclstars.$(LIBSUFFIX) \
				$(LIBDIR)/libipusstars.$(LIBSUFFIX) \
				$(LIBDIR)/libipus.$(LIBSUFFIX) \
				$(LIBDIR)/libicp.$(LIBSUFFIX)
	endif
endif

ifdef DE
	ifdef ATM
		CUSTOM_DIRS += $(DEDIR)/atm
		LIBS += -ldeatmstars
		LIBFILES += $(LIBDIR)/libdeatmstars.$(LIBSUFFIX)
	endif
	SDFLIB = 1
	PALETTES += PTOLEMY/src/domains/de/icons/de.pal
	STARS += $(LIBDIR)/destars.o
	LIBS += -ldestars -lde
	LIBFILES += $(LIBDIR)/libdestars.$(LIBSUFFIX) \
		$(LIBDIR)/libde.$(LIBSUFFIX)
	CUSTOM_DIRS += $(DEDIR)/stars $(DEDIR)/kernel 
	ifdef TK
		CUSTOM_DIRS += $(DEDIR)/tcltk/stars
		STARS += $(LIBDIR)/detclstars.o
		LIBS += -ldetclstars
		LIBFILES += $(LIBDIR)/libdetclstars.$(LIBSUFFIX)
	endif
endif

# Networks Of Workstations Active Messages
ifdef NOWAM
	# NOWam only supported under the Solaris operating system,
	# matched by pattern sol%
	ifneq ("$(filter sol%,$(PTARCH))","")
		CUSTOM_DIRS += $(CGCDIR)/targets/NOWam/NOWam \
			$(CGCDIR)/targets/NOWam/libudpam
		CGC = 1
		CGPAR = 1
		ifeq ($(USE_SHARED_LIBS),yes) 
			LIBS += -lNOWam
			LIBFILES += $(LIBDIR)/libNOWam.$(LIBSUFFIX)
		else
			TARGETS += $(CGCT)/NOWam/NOWam/CGCNOWamRecv.o \
				$(CGCT)/NOWam/NOWam/CGCNOWamSend.o \
				$(CGCT)/NOWam/NOWam/CGCNOWamTarget.o
		endif
	endif
endif

ifdef CGC
	ifdef CGCTK
		# CGC Tcl/Tk stars and library for the stars
		CUSTOM_DIRS += $(CGCDIR)/tcltk/lib $(CGCDIR)/tcltk/stars
		STARS += $(LIBDIR)/cgctcltkstars.o
		LIBS += -lcgctcltk
		LIBFILES += $(LIBDIR)/libcgctcltk.$(LIBSUFFIX)
		# CGC Tcl/Tk targets
		CUSTOM_DIRS += $(CGCDIR)/tcltk/targets
		TARGETS += $(CGCTCLT)/CGCTclTkTarget.o
	endif
	ifdef CGCVIS
		# CGC VIS stars
		CUSTOM_DIRS += $(CGCDIR)/vis/stars
		STARS += $(LIBDIR)/cgcvisstars.o
		LIBS += -lcgcvisstars
		LIBFILES += $(LIBDIR)/libcgcvisstars.$(LIBSUFFIX)
		# CGC VIS targets
		CUSTOM_DIRS += $(CGCDIR)/vis/targets
		ifeq ($(USE_SHARED_LIBS),yes)
			LIBS += -lcgcvistargets
			LIBFILES += $(LIBDIR)/libcgcvistargets.$(LIBSUFFIX)
		else
			TARGETS += $(CGCVIST)/CGCVISTarget.o
		endif
	endif
	CGCLIB = 1
	CG = 1
	SDFLIB = 1
	BDFLIB = 1
	# Uniprocessor targets are included by default
	CUSTOM_DIRS += $(CGCDIR)/targets/uniprocessor
	ifeq ($(USE_SHARED_LIBS),yes) 
		LIBS += -lcgctargets
		LIBFILES += $(LIBDIR)/libcgctargets.$(LIBSUFFIX)
	else
		TARGETS += $(CGCT)/uniprocessor/CGCMakefileTarget.o \
			   $(CGCT)/uniprocessor/CGCSDFBase.o \
			   $(CGCT)/uniprocessor/CGCSDFSend.o \
			   $(CGCT)/uniprocessor/CGCSDFReceive.o \
			   $(CGCT)/uniprocessor/CreateSDFStar.o
	endif
	# Multiprocessor targets are included if CGPAR is defined
	ifdef CGPAR
		CUSTOM_DIRS += $(CGCDIR)/targets/multiprocessor
		ifeq ($(USE_SHARED_LIBS),yes) 
			LIBS += -lcgcmultitargets
			LIBFILES += $(LIBDIR)/libcgcmultitargets.$(LIBSUFFIX)
		else
			TARGETS += $(CGCT)/multiprocessor/CGCUnixSend.o \
				$(CGCT)/multiprocessor/CGCUnixReceive.o \
				$(CGCT)/multiprocessor/CGCMultiTarget.o \
				$(CGCT)/multiprocessor/CGCMacroStar.o \
				$(CGCT)/multiprocessor/CGCDDFCode.o \
				$(CGCT)/multiprocessor/CompileCGSubsystems.o
		endif
	endif
	PALETTES += PTOLEMY/src/domains/cgc/icons/cgc.pal
	STARS += $(LIBDIR)/cgcdspstars.o $(LIBDIR)/cgcstars.o
	CUSTOM_DIRS += $(CGCDIR)/dsp/stars $(CGCDIR)/stars
	LIBS += -lcgcdspstars -lcgcstars
	LIBFILES += $(LIBDIR)/libcgcdspstars.$(LIBSUFFIX) \
		$(LIBDIR)/libcgcstars.$(LIBSUFFIX)
	# Window and RaisedCos DSP stars need the Cephes Library
	CEPHESLIB = 1
endif

ifdef CGCLIB
	SDFLIB = 1
	CUSTOM_DIRS += $(CGCDIR)/kernel
	LIBS += -lcgc
	LIBFILES += $(LIBDIR)/libcgc.$(LIBSUFFIX)
endif

ifdef BDF
	CUSTOM_DIRS += $(BDFDIR)/stars
	BDFLIB = 1
	PALETTES += PTOLEMY/src/domains/bdf/icons/bdf.pal
	SDF = 1
	STARS += $(LIBDIR)/bdfstars.o
	ifdef CG
		TARGETS += $(CGT)/CGBDFTarget.o 
	endif
	ifdef CGC
		TARGETS += $(CGCT)/uniprocessor/CGCBDFTarget.o
	endif
	LIBS += -lbdfstars
	LIBFILES += $(LIBDIR)/libbdfstars.$(LIBSUFFIX)
endif

ifdef CG
	CUSTOM_DIRS += $(CGDIR)/kernel $(CGDIR)/targets $(CGDIR)/stars
	PALETTES += PTOLEMY/src/domains/cg/icons/cg.pal
	ifdef CGPAR
		CUSTOM_DIRS += $(CGDIR)/HuScheduler $(CGDIR)/ddfScheduler \
			$(CGDIR)/macroScheduler $(CGDIR)/dcScheduler  \
			$(CGDIR)/dlScheduler $(CGDIR)/hierScheduler \
			$(CGDIR)/parScheduler
		TARGETS += $(CGT)/CGMultiTarget.o $(CGT)/CGSharedBus.o 
		LIBS += -lMacro -lDC -lHu -lDL -lHier -lcgddf -lPar
		LIBFILES += $(LIBDIR)/libMacro.$(LIBSUFFIX) \
			$(LIBDIR)/libDC.$(LIBSUFFIX) \
			$(LIBDIR)/libHu.$(LIBSUFFIX) \
			$(LIBDIR)/libDL.$(LIBSUFFIX) \
			$(LIBDIR)/libHier.$(LIBSUFFIX) \
			$(LIBDIR)/libcgddf.$(LIBSUFFIX) \
			$(LIBDIR)/libPar.$(LIBSUFFIX)
	endif
	SDFLIB = 1
	STARS += $(LIBDIR)/cgstars.o
	LIBS += -lcgstars -lcg
	LIBFILES += $(LIBDIR)/libcgstars.$(LIBSUFFIX) \
		$(LIBDIR)/libcg.$(LIBSUFFIX)
endif

ifdef SDFFULL
	SDF = 1
	SDFIMAGE = 1
	SDFMATRIX = 1
	SDFMATLAB = 1
	SDFCONTRIB = 1
	SDFDSP = 1
	SDFVIS = 1
	ifdef TK
		SDFTK = 1
		SDFDFM = 1
	endif
endif

# We only build the VIS subdomain under the cfront compiler on Solaris
# 2.5 and higher.  We test for sol2 and cfront in the architecture name
# and the presence of the VSDKHOME environment variable.
ifdef SDFVIS
	ifeq ("$(findstring sol2,$(PTARCH))","")
		SDFVIS =
	endif
	ifeq ("$(findstring cfront,$(PTARCH))","")
		SDFVIS =
	endif
	ifndef VSDKHOME
		SDFVIS =
	endif
endif

ifdef ATM
	SDF=1
endif

ifdef SDF 
	PALETTES += PTOLEMY/src/domains/sdf/icons/sdf.pal
	ifdef ATM
		CUSTOM_DIRS += $(SDFDIR)/atm/stars
		LIBS += -lsdfatmstars
		LIBFILES += $(LIBDIR)/libsdfatmstars.$(LIBSUFFIX)
	endif
	ifdef SDFDFM
		CUSTOM_DIRS += $(SDFDIR)/dfm/stars
		STARS += $(LIBDIR)/sdfdfmstars.o
		LIBS += -lsdfdfmstars
		LIBFILES += $(LIBDIR)/libsdfdfmstars.$(LIBSUFFIX)
		SDFTK = 1
	endif
	ifdef SDFTK
		CUSTOM_DIRS += $(SDFDIR)/tcltk/stars
		STARS += $(LIBDIR)/sdftclstars.o
		LIBS += -lsdftclstars
		LIBFILES += $(LIBDIR)/libsdftclstars.$(LIBSUFFIX)
		SDFDSP = 1
	endif
	ifdef SDFDSP 
		CUSTOM_DIRS += $(SDFDIR)/dsp/stars
		STARS += $(LIBDIR)/sdfdspstars.o
		LIBS += -lsdfdspstars
		LIBFILES += $(LIBDIR)/libsdfdspstars.$(LIBSUFFIX)
		# Cephes library is used by the Window and RaiseCosine stars
		CEPHESLIB = 1
	endif
	ifdef SDFMATRIX 
		CUSTOM_DIRS += $(SDFDIR)/matrix/stars
		STARS += $(LIBDIR)/sdfmatrixstars.o
		LIBS += -lsdfmatrixstars
		LIBFILES += $(LIBDIR)/libsdfmatrixstars.$(LIBSUFFIX)
	endif
	ifdef SDFIMAGE
		CUSTOM_DIRS += $(SDFDIR)/image/stars
		STARS += $(LIBDIR)/sdfimagestars.o
		LIBS += -lsdfimagestars
		LIBFILES += $(LIBDIR)/libsdfimagestars.$(LIBSUFFIX) 
	endif
	ifdef SDFMATLAB
		CUSTOM_DIRS += $(SDFDIR)/matlab/stars
		STARS += $(LIBDIR)/sdfmatlabstars.o
		LIBS += -lsdfmatlabstars
		LIBFILES += $(LIBDIR)/libsdfmatlabstars.$(LIBSUFFIX)
		EXTERNALTOOLSLIB = 1
		MATLABENGINE = 1
	endif
	ifdef SDFCONTRIB
		CUSTOM_DIRS += $(SDFDIR)/contrib/kernel $(SDFDIR)/contrib/stars
		STARS += $(LIBDIR)/sdfcontribstars.o
		LIBS += -lsdfcontribstars -lsdfcontrib 
		LIBFILES += $(LIBDIR)/libsdfcontribstars.$(LIBSUFFIX) \
			$(LIBDIR)/libsdfcontrib.$(LIBSUFFIX)
	endif
	ifdef SDFVIS
		# Ultrasparc Visual Instruction Set (VIS) stars
		CUSTOM_DIRS += $(SDFDIR)/vis/stars
		STARS += $(LIBDIR)/sdfvisstars.o
		LIBS += -lsdfvisstars
		LIBS += $(VSDKHOME)/util/vis.il
	endif
	CUSTOM_DIRS += $(SDFDIR)/stars
	STARS += $(LIBDIR)/sdfstars.o 
	ifdef CG
		# FIXME: The only thing in sdf/targets also depends on 
		# cg... should this be in the cg directory?
		CUSTOM_DIRS += $(SDFDIR)/targets
		TARGETS += $(SDFT)/CompileTarget.o
	endif
	LIBS += -lsdfstars
	LIBFILES += $(LIBDIR)/libsdfstars.$(LIBSUFFIX)	
	SDFLIB = 1
endif

ifdef ATM
	CUSTOM_DIRS += $(SDFDIR)/atm/kernel
	LIBS += -latm
	LIBFILES += $(LIBDIR)/libatm.$(LIBSUFFIX)
endif

ifdef CEPHESLIB
	LIBS += -lcephes
	LIBFILES += $(LIBDIR)/libcephes.$(LIBSUFFIX)
endif

ifdef BDFLIB
	CUSTOM_DIRS += $(BDFDIR)/kernel
	LIBS += -lbdf
	LIBFILES += $(LIBDIR)/libbdf.$(LIBSUFFIX)
endif

ifdef SDFLIB
	CUSTOM_DIRS += $(SDFDIR)/kernel $(SDFDIR)/loopScheduler

		LIBS += -lLS -lsdf
		LIBFILES += $(LIBDIR)/libLS.$(LIBSUFFIX) \
			$(LIBDIR)/libsdf.$(LIBSUFFIX)
	ifneq ($(USE_SHARED_LIBS),yes) 
		TARGETS += $(OBJDIR)/domains/sdf/loopScheduler/LoopTarget.o
	endif
endif

# HOF stars can be used in pigiRpc but not ptcl.
# The HOF base classes call Tk_DoOneEvent so that if you accidentally
# specify an infinite recursion (easy to do with HOF), you can hit the
# STOP button and abort the run.  But Tk is not linked into ptcl, so
# this call cannot be done.
ifdef HOF
	CUSTOM_DIRS += $(HOFDIR)/kernel $(HOFDIR)/stars
	STARS += $(LIBDIR)/hofstars.o
	LIBS +=	-lhofstars
	LIBFILES += $(LIBDIR)/libhofstars.$(LIBSUFFIX)
	HOFLIB = 1
endif

ifdef HOFLIB
	LIBS +=	-lhof
	LIBFILES += $(LIBDIR)/libhof.$(LIBSUFFIX)
endif

ifeq ($(USE_SHARED_LIBS),yes) 
	STARS =
endif

ifdef OCT
    CUSTOM_DIRS += $(CROOT)/src/pigilib
    MISC_OBJS += $(ROOT)/obj.$(PTARCH)/pigilib/ptkRegisterCmds.o
    ifeq ($(USE_SHARED_LIBS),yes) 
	# We need to include -lvov if we are using shared libraries, as
	# the oh library has references to vov symbols in it.
	LIBVOV = -lvov
    endif
	# We need -loh because pigilib/compile.c calls ohFormatName()
	# We need -llist because of a few calls in pigilib and librpc
	# We need -lutility because a few file in pigilib call util_csystem()
	# We need -lst because of calls in pigilib and liboh, librpc,libutility
	# We need -lerrtrap and -luprintf because of the other oct libs
    LIBS += 	-L$(OCTLIBDIR) -lpigi \
		-lrpc -loh $(LIBVOV) -llist \
		-lutility -lst -lerrtrap -luprintf #-lport

    ifdef USE_CORE_STATIC_LIBS
	LIBPIGI = $(LIBDIR)/libpigi.a
    else
	LIBPIGI = $(LIBDIR)/libpigi.$(LIBSUFFIX)
    endif

    LIBFILES += $(LIBPIGI) \
	$(OCTLIBDIR)/librpc.$(LIBSUFFIX)  \
	$(OCTLIBDIR)/liboh.$(LIBSUFFIX) $(OCTLIBDIR)/libvov.$(LIBSUFFIX) \
	$(OCTLIBDIR)/liblist.$(LIBSUFFIX) \
	$(OCTLIBDIR)/libutility.$(LIBSUFFIX) \
	$(OCTLIBDIR)/libst.$(LIBSUFFIX) $(OCTLIBDIR)/liberrtrap.$(LIBSUFFIX) \
	$(OCTLIBDIR)/libuprintf.$(LIBSUFFIX)
endif

ifdef TK
	CUSTOM_DIRS += $(CROOT)/src/ptklib 
	LIBS += -lptk $(ITK_LIBSPEC) $(TK_LIBSPEC) $(X11_LIBSPEC) 
	PT_DEPEND += $(LIBDIR)/libptk.a
endif

CUSTOM_DIRS += $(CROOT)/src/kernel $(CROOT)/src/pigiRpc $(CROOT)/src/ptcl \
	$(CROOT)/mk $(CROOT)/src/tysh $(CROOT)/src/pigiExample

# Under some architectures, we need to use static libraries or we get
# GateKeeper errors.
ifdef USE_CORE_STATIC_LIBS
	LIBPTCL = $(LIBDIR)/libptcl.a
else
	LIBPTCL = $(LIBDIR)/libptcl.$(LIBSUFFIX)
endif

PT_DEPEND += $(LIBPTCL) $(LIBDIR)/libptolemy.a \
	$(LIBFILES) $(STARS) $(TARGETS)

# this would not be defined if we are making a small stand-alone 
# program to test the ptolemy libraries, see standalone.mk
ifdef PIGI
	LIBS += version.o -lptcl 
endif

# External interface support - we need to expand libexttools, because it
# depends upon the Ptolemy kernel.  Matlab and mathematica are always pulled in
CUSTOM_DIRS += $(CROOT)/src/utils/libexttools \
	$(CROOT)/src/utils/libptmatlab \
	$(CROOT)/src/utils/libptmathematica
LIBS += -lexttools $(MATLABEXT_LIB) $(MATHEMATICAEXT_LIB)
LIBFILES += $(LIBDIR)/libexttools.$(LIBSUFFIX)

# Now, pull in the Ptolemy kernel support, system libraries and tcl
LIBS += -lptolemy $(ITCL_LIBSPEC) $(TCL_LIBSPEC) \
	$(SYSLIBS) $(LIB_FLUSH_CACHE)

