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
#
# Note that you must explicitly include any target .o files in TARGETS
# for architectures that don't use shared libraries.  If you don't, then
# you won't see these targets in non-shared library architectures.  For
# an example, see the CG56 target section below.
#
# Stub files that pull in the stars.
#
# This file depends on make variables defined by the config.$(PTARCH).mk
# and config.default makefiles, e.g. ITCL_LIBSPEC, ITK_LIBSPEC,
# LIB_FLUSH_CACHE, LIBSUFFIX, SYSLIBS, TCL_LIBSPEC, TK_LIBSPEC,
# USE_SHARED_LIBS, and X11_LIBSPEC.

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
VHDLTCLT = $(OBJDIR)/domains/vhdl/tcltk/targets

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
FSMDIR = $(CROOT)/src/domains/fsm

# Check make variables for including/excluding experimental domains

ifneq ($(INCLUDE_IPUS_DOMAIN),yes)
	# Non G++ compilers fail to compile IPUS' templates
	IPUS =
endif

ifneq ($(INCLUDE_PN_DOMAIN),yes)
	PN =
endif

# Tcl/Tk stars can be used in pigiRpc but not ptcl, for this we define TK

ifndef TK
	# We can't include the ipus domain since the non-tk ipus stars need 
	# Blackboard* symbols, which are defined in the tk ipus stars.
	# Set up definitions for stars.mk
	IPUS=

	CGCTK=
	CGCVISTK=
	CODESIGN=
	FSM=
	SDFTK=
	SDFDFM=
	VHDLTK=
endif

# Synchronous/Reactive domain

ifdef SR
	# star icons
	PALETTES += PTOLEMY/src/domains/sr/icons/sr.pal
	# domain kernel and stars
	STARS += $(LIBDIR)/srstars.o
	CUSTOM_DIRS += $(SRDIR)/stars $(SRDIR)/kernel 
	LIBS += -lsrstars -lsr
	LIBFILES += $(LIBDIR)/libsrstars.$(LIBSUFFIX) \
		$(LIBDIR)/libsr.$(LIBSUFFIX)
endif

# Motorola DSP assembly code generation domain

ifdef CG56
	# star icons
	PALETTES += PTOLEMY/src/domains/cg56/icons/main.pal
	# special targets
	CUSTOM_DIRS += $(CG56DIR)/targets
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
	# domain kernel and stars
	CUSTOM_DIRS += $(CG56DIR)/kernel $(CG56DIR)/stars $(CG56DIR)/dsp/stars
	STARS += $(LIBDIR)/cg56dspstars.o $(LIBDIR)/cg56stars.o
	LIBS += -lcg56dspstars -lcg56stars -lcg56
	LIBFILES += $(LIBDIR)/libcg56dspstars.$(LIBSUFFIX) \
		$(LIBDIR)/libcg56stars.$(LIBSUFFIX) \
		$(LIBDIR)/libcg56.$(LIBSUFFIX)
	# dependencies
	# must bring in the parallel schedulers for multi-cg56 targets
	CGFULL = 1
	# Window and RaisedCosine stars in cg56/dsp/stars need ptdsp Library
	PTDSPLIB = 1
	# CG56 CGC targets need CGCStar
	CGCLIB = 1
	# CG56 CGC targets need CGCFix star
	CGC = 1
endif

# Texas Instruments TMS320C50 Code Generation Domain

ifdef C50
	# star icons
	PALETTES += PTOLEMY/src/domains/c50/icons/main.pal
	# special targets
	CUSTOM_DIRS += $(C50DIR)/targets
	ifeq ($(USE_SHARED_LIBS),yes) 
		LIBS += -lc50targets
		LIBFILES += $(LIBDIR)/libc50targets.$(LIBSUFFIX)
	else
		TARGETS += $(C50T)/DSKC50Target.o \
			$(C50T)/SubC50Target.o
	endif
	# kernel and stars 
	CUSTOM_DIRS += $(C50DIR)/kernel $(C50DIR)/stars $(C50DIR)/dsp/stars
	STARS += $(LIBDIR)/c50dspstars.o $(LIBDIR)/c50stars.o
	LIBS += -lc50dspstars -lc50stars -lc50
	LIBFILES += $(LIBDIR)/libc50dspstars.$(LIBSUFFIX) \
		$(LIBDIR)/libc50stars.$(LIBSUFFIX) \
		$(LIBDIR)/libc50.$(LIBSUFFIX)
	# dependencies
	# must bring in the parallel schedulers for multi-c50 targets
	# CGFULL = 1
	CG = 1
	# Window and RaisedCosine star in c50/dsp/stars need ptdsp Library
	PTDSPLIB = 1
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
		CGCVISTK = 1
	endif
	CGCVIS = 1
endif

# CGC VIS demonstrations will only run under Solaris 2.5 under cfront,
# but the CGC VIS subdomain will build under any architecture.
ifdef CGCVIS
	# Don't build the CGC VIS subdomain on a non-Solaris machine
	ifeq ("$(filter sol% ,$(PTARCH))","")
		CGCVIS =
		CGCVISTK =
	endif
endif

ifdef CGFULL
	CG = 1
	CGPAR = 1
endif

ifdef VHDLFULL
	VHDL = 1
	ifdef TK
		VHDLTK = 1
	endif
endif

ifdef VHDL
	# star icons
	PALETTES += PTOLEMY/src/domains/vhdl/icons/vhdl.pal
	# special targets
	CUSTOM_DIRS += $(VHDLDIR)/targets $(CROOT)/src/utils/ptvhdlsim
	ifeq ($(USE_SHARED_LIBS),yes) 
		LIBS += -lvhdltargets
		LIBFILES += $(LIBDIR)/libvhdltargets.$(LIBSUFFIX)
	else
		TARGETS += $(VHDLT)/ArchTarget.o \
			$(VHDLT)/CGCVReceive.o \
			$(VHDLT)/CGCVSend.o \
			$(VHDLT)/CGCVSynchComm.o \
			$(VHDLT)/SimLFTarget.o \
			$(VHDLT)/SimMTTarget.o \
			$(VHDLT)/SimVSSTarget.o \
			$(VHDLT)/StructTarget.o \
			$(VHDLT)/SynthArchTarget.o \
			$(VHDLT)/SynthTarget.o \
			$(VHDLT)/VHDLCReceive.o \
			$(VHDLT)/VHDLCSend.o \
			$(VHDLT)/VHDLLFCReceive.o \
			$(VHDLT)/VHDLLFCSend.o \
			$(VHDLT)/VHDLCSynchComm.o
	endif
	# Tcl/Tk
	ifdef VHDLTK
		# special targets
		CUSTOM_DIRS += $(VHDLDIR)/tcltk/targets
		ifeq ($(USE_SHARED_LIBS),yes) 
			LIBS += -lvhdltcltktargets
			LIBFILES += $(LIBDIR)/libvhdltcltktargets.$(LIBSUFFIX)
		else
			TARGETS += $(VHDLTCLT)/TkSchedTarget.o
		endif
	endif
	# kernel and stars
	CUSTOM_DIRS += $(VHDLDIR)/kernel $(VHDLDIR)/stars
	STARS += $(LIBDIR)/vhdlstars.o
	LIBS += -lvhdlstars -lvhdl
	LIBFILES += $(LIBDIR)/libvhdlstars.$(LIBSUFFIX) \
		$(LIBDIR)/libvhdl.$(LIBSUFFIX) 
	# dependencies
	CG = 1
	SDFLIB = 1
	CGCLIB = 1
	# Window star in vhdl/stars needs the ptdsp Library
	PTDSPLIB = 1
endif

ifdef VHDLB
	# star icons
	PALETTES += PTOLEMY/src/domains/vhdlb/icons/vhdlb.pal
	# kernel and stars
	CUSTOM_DIRS += $(VHDLBDIR)/kernel $(VHDLBDIR)/stars
	STARS += $(LIBDIR)/vhdlbstars.o
	LIBS += -lvhdlbstars -lvhdlb
	LIBFILES += $(LIBDIR)/libvhdlbstars.$(LIBSUFFIX) \
		$(LIBDIR)/libvhdlb.$(LIBSUFFIX)
	# dependencies
	CG = 1
	SDFLIB = 1
endif

ifdef MDSDF
	# star icons
	PALETTES += PTOLEMY/src/domains/mdsdf/icons/mdsdf.pal
	# Tcl/Tk stars
	ifdef TK 
		CUSTOM_DIRS += $(MDSDFDIR)/tcltk/stars
		STARS += $(LIBDIR)/mdsdftclstars.o
		LIBFILES += $(LIBDIR)/libmdsdftclstars.$(LIBSUFFIX)
		LIBS += -lmdsdftclstars
	endif
	# kernel and stars
	CUSTOM_DIRS += $(MDSDFDIR)/kernel $(MDSDFDIR)/stars
	STARS +=  $(LIBDIR)/mdsdfstars.o
	LIBS += -lmdsdfstars -lmdsdf
	LIBFILES += $(LIBDIR)/libmdsdfstars.$(LIBSUFFIX) \
		$(LIBDIR)/libmdsdf.$(LIBSUFFIX)
	# dependencies
	SDFLIB = 1
endif

ifdef PN
	# star icons
	PALETTES += PTOLEMY/src/domains/pn/icons/pn.pal
	# kernel and stars
	CUSTOM_DIRS += $(PNDIR)/kernel $(PNDIR)/stars 
	# PN is only supported under Sun, Solaris and HP-UX operating systems,
	# matched by patterns sun% and sol% and hppa% respectively
	ifneq ("$(filter sun% sol% hppa% ,$(PTARCH))","")
		STARS += $(LIBDIR)/pnstars.o
		LIBS += -lpnstars -lpn
		LIBFILES += $(LIBDIR)/libpnstars.$(LIBSUFFIX) \
			$(LIBDIR)/libpn.$(LIBSUFFIX)
		ifneq ("$(filter sun% sol% ,$(PTARCH))","")
			CUSTOM_DIRS += $(CROOT)/src/thread/gthreads \
				$(CROOT)/src/thread/posix \
				$(CROOT)/thread/include \
				$(CROOT)/thread/include.$(PTARCH) \
				$(CROOT)/thread/lib.$(PTARCH)
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
	# dependencies
	SDFLIB = 1
endif

ifdef DDF
	# star icons
	PALETTES += PTOLEMY/src/domains/ddf/icons/ddf.pal
	# There are many DDF code generators and schedulers
	# (HuScheduler, dcScheduler, ddfScheduler, dlScheduler, etc.)
	# but none are currently being built.  When they are, add
	# ifdef CG
	#	LIBS +=
	#	LIBFILES +=
	# endif

	# kernel and stars
	CUSTOM_DIRS += $(DDFDIR)/kernel $(DDFDIR)/stars 
	STARS += $(LIBDIR)/ddfstars.o
	LIBS += -lddfstars -lddf
	LIBFILES += $(LIBDIR)/libddfstars.$(LIBSUFFIX) \
		$(LIBDIR)/libddf.$(LIBSUFFIX)
	# dependencies
	SDFLIB = 1
endif

# We must have TK present to include the IPUS domain since the non-tk IPUS
# stars need Blackboard* symbols, which are defined in the Tk IPUS stars.

ifdef IPUS
	ifdef TK
		# star icons
		PALETTES += PTOLEMY/src/domains/ipus/icons/ipus.pal
		# kernel and stars
		CUSTOM_DIRS += $(IPUSDIR)/icp/kernel $(IPUSDIR)/kernel \
				$(IPUSDIR)/stars $(IPUSDIR)/tcltk/stars 
		STARS += $(LIBDIR)/ipustclstars.o $(LIBDIR)/ipusstars.o
		LIBS += -lipustclstars -lipusstars -lipus -licp
		LIBFILES += $(LIBDIR)/libipustclstars.$(LIBSUFFIX) \
				$(LIBDIR)/libipusstars.$(LIBSUFFIX) \
				$(LIBDIR)/libipus.$(LIBSUFFIX) \
				$(LIBDIR)/libicp.$(LIBSUFFIX)
	endif
endif

ifdef DE
	# star icons
	PALETTES += PTOLEMY/src/domains/de/icons/de.pal
	# Asynchronous Transfer Mode stars
	ifdef ATM
		CUSTOM_DIRS += $(DEDIR)/atm
		STARS += $(LIBDIR)/deatmstars.o
		LIBS += -ldeatmstars
		LIBFILES += $(LIBDIR)/libdeatmstars.$(LIBSUFFIX)
	endif
	# Tcl/Tk stars
	ifdef TK
		CUSTOM_DIRS += $(DEDIR)/tcltk/stars
		STARS += $(LIBDIR)/detclstars.o
		LIBS += -ldetclstars
		LIBFILES += $(LIBDIR)/libdetclstars.$(LIBSUFFIX)
	endif
	# kernel and stars
	CUSTOM_DIRS += $(DEDIR)/stars $(DEDIR)/kernel 
	STARS += $(LIBDIR)/destars.o
	LIBS += -ldestars -lde
	LIBFILES += $(LIBDIR)/libdestars.$(LIBSUFFIX) \
		$(LIBDIR)/libde.$(LIBSUFFIX)
	# dependencies
	SDFLIB = 1
endif

# Networks Of Workstations Active Messages
ifdef NOWAM
	# NOWam only supported under the Solaris operating system,
	# matched by pattern sol%
	ifneq ("$(filter sol%,$(PTARCH))","")
		CUSTOM_DIRS += $(CGCDIR)/targets/NOWam/NOWam \
			$(CGCDIR)/targets/NOWam/libudpam
		ifeq ($(USE_SHARED_LIBS),yes) 
			LIBS += -lNOWam
			LIBFILES += $(LIBDIR)/libNOWam.$(LIBSUFFIX)
		else
			TARGETS += $(CGCT)/NOWam/NOWam/CGCNOWamRecv.o \
				$(CGCT)/NOWam/NOWam/CGCNOWamSend.o \
				$(CGCT)/NOWam/NOWam/CGCNOWamTarget.o
		endif
		# dependencies
		CGC = 1
		CGPAR = 1
	endif
endif

ifdef CGC
	# star icons
	PALETTES += PTOLEMY/src/domains/cgc/icons/cgc.pal
	# UltraSparc Visual Instruction Set stars
	ifdef CGCVIS
	        # CGC VIS stars
		CUSTOM_DIRS += $(CGCDIR)/vis/stars 
		STARS += $(LIBDIR)/cgcvisstars.o
		LIBS += -lcgcvisstars
		LIBFILES += $(LIBDIR)/libcgcvisstars.$(LIBSUFFIX)

		# CGC VIS target (no shared library version)
		CUSTOM_DIRS += $(CGCDIR)/vis/targets
		TARGETS += $(CGCVIST)/CGCVISSimTarget.o

		# CGC VIS TK
		ifdef CGCVISTK
			CUSTOM_DIRS += $(CGCDIR)/vis/tcltk/stars
			LIBS += -lcgcvistcltk
			LIBFILES += $(LIBDIR)/libcgctcltk.$(LIBSUFFIX)
		endif
	endif
	# Tcl/Tk
	ifdef CGCTK
		# stars and library for generated code
		CUSTOM_DIRS += $(CGCDIR)/tcltk/lib $(CGCDIR)/tcltk/stars
		STARS += $(LIBDIR)/cgctcltkstars.o
		LIBS += -lcgctcltk
		LIBFILES += $(LIBDIR)/libcgctcltk.$(LIBSUFFIX)

		# special target (no shared library version)
		CUSTOM_DIRS += $(CGCDIR)/tcltk/targets
		TARGETS += $(CGCTCLT)/CGCTclTkTarget.o \
			$(CGCTCLT)/CGCTychoTarget.o
	endif
	# Multiprocessor targets are included if CGPAR is defined
	ifdef CGPAR
		CUSTOM_DIRS += $(CGCDIR)/targets/multiproc
		ifeq ($(USE_SHARED_LIBS),yes) 
			LIBS += -lcgcmultitargets
			LIBFILES += $(LIBDIR)/libcgcmultitargets.$(LIBSUFFIX)
		else
			TARGETS += $(CGCT)/multiproc/CGCUnixSend.o \
				$(CGCT)/multiproc/CGCUnixReceive.o \
				$(CGCT)/multiproc/CGCMultiTarget.o \
				$(CGCT)/multiproc/CGCMacroStar.o \
				$(CGCT)/multiproc/CGCDDFCode.o \
				$(CGCT)/multiproc/CompileCGSubsystems.o
		endif
	endif
	# Uniprocessor targets are included by default
	CUSTOM_DIRS += $(CGCDIR)/targets/uniproc
	ifeq ($(USE_SHARED_LIBS),yes) 
		LIBS += -lcgctargets
		LIBFILES += $(LIBDIR)/libcgctargets.$(LIBSUFFIX)
	else
		TARGETS += $(CGCT)/uniproc/CGCMakefileTarget.o \
			   $(CGCT)/uniproc/CGCSDFBase.o \
			   $(CGCT)/uniproc/CGCSDFSend.o \
			   $(CGCT)/uniproc/CGCSDFReceive.o \
			   $(CGCT)/uniproc/CreateSDFStar.o
	endif
	# kernel and stars
	STARS += $(LIBDIR)/cgcdspstars.o $(LIBDIR)/cgcstars.o
	CUSTOM_DIRS += $(CGCDIR)/dsp/stars $(CGCDIR)/stars
	LIBS += -lcgcdspstars -lcgcstars
	LIBFILES += $(LIBDIR)/libcgcdspstars.$(LIBSUFFIX) \
		$(LIBDIR)/libcgcstars.$(LIBSUFFIX)
	# dependencies
	SDFLIB = 1
	BDFLIB = 1
	CGCLIB = 1
	CG = 1
	# Window and RaisedCos DSP stars need the ptdsp Library
	PTDSPLIB = 1
endif

ifdef CGCLIB
	SDFLIB = 1
	CUSTOM_DIRS += $(CGCDIR)/kernel
	LIBS += -lcgc
	LIBFILES += $(LIBDIR)/libcgc.$(LIBSUFFIX)
endif

ifdef BDF
	# star icons
	PALETTES += PTOLEMY/src/domains/bdf/icons/bdf.pal
	# special targets
	ifdef CG
		TARGETS += $(CGT)/CGBDFTarget.o 
	endif
	ifdef CGC
		TARGETS += $(CGCT)/uniproc/CGCBDFTarget.o
	endif
	# kernel and stars
	CUSTOM_DIRS += $(BDFDIR)/stars
	STARS += $(LIBDIR)/bdfstars.o
	LIBS += -lbdfstars
	LIBFILES += $(LIBDIR)/libbdfstars.$(LIBSUFFIX)
	# dependencies
	BDFLIB = 1
	SDF = 1
endif

ifdef CG
	# star icons
	PALETTES += PTOLEMY/src/domains/cg/icons/cg.pal
	# special targets
	TARGETS += $(CGT)/CGCostTarget.o
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
	# kernel and stars
	CUSTOM_DIRS += $(CGDIR)/kernel $(CGDIR)/targets $(CGDIR)/stars
	STARS += $(LIBDIR)/cgstars.o
	LIBS += -lcgstars -lcg
	LIBFILES += $(LIBDIR)/libcgstars.$(LIBSUFFIX) \
		$(LIBDIR)/libcg.$(LIBSUFFIX)
	# dependencies
	SDFLIB = 1
endif

ifdef FSM
	# stars icons (none)
	# kernel and stars
	CUSTOM_DIRS += $(FSMDIR)/kernel $(FSMDIR)/stars
	STARS += $(LIBDIR)/fsmstars.o
	LIBS += -lfsmstars -lfsm
	LIBFILES += $(LIBDIR)/libfsmstars.$(LIBSUFFIX) \
		$(LIBDIR)/libfsm.$(LIBSUFFIX)
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
	SDF = 1
endif

ifdef SDF 
	# star icons
	PALETTES += PTOLEMY/src/domains/sdf/icons/sdf.pal
	# Asynchronous Transfer Mode stars
	ifdef ATM
		CUSTOM_DIRS += $(SDFDIR)/atm/stars
		STARS += $(LIBDIR)/sdfatmstars.o
		LIBS += -lsdfatmstars
		LIBFILES += $(LIBDIR)/libsdfatmstars.$(LIBSUFFIX)
	endif
	# Design Flow Management stars
	ifdef SDFDFM
		CUSTOM_DIRS += $(SDFDIR)/dfm/stars
		STARS += $(LIBDIR)/sdfdfmstars.o
		LIBS += -lsdfdfmstars
		LIBFILES += $(LIBDIR)/libsdfdfmstars.$(LIBSUFFIX)
		# dependencies
		SDFTK = 1
	endif
	# Tcl/Tk stars
	ifdef SDFTK
		CUSTOM_DIRS += $(SDFDIR)/tcltk/stars
		STARS += $(LIBDIR)/sdftclstars.o
		LIBS += -lsdftclstars
		LIBFILES += $(LIBDIR)/libsdftclstars.$(LIBSUFFIX)
		# dependencies
		SDFDSP = 1
	endif
	# DSP stars
	ifdef SDFDSP 
		CUSTOM_DIRS += $(SDFDIR)/dsp/stars
		STARS += $(LIBDIR)/sdfdspstars.o
		LIBS += -lsdfdspstars
		LIBFILES += $(LIBDIR)/libsdfdspstars.$(LIBSUFFIX)
	endif
	# Image processing stars
	ifdef SDFIMAGE
		CUSTOM_DIRS += $(SDFDIR)/image/stars
		STARS += $(LIBDIR)/sdfimagestars.o
		LIBS += -lsdfimagestars
		LIBFILES += $(LIBDIR)/libsdfimagestars.$(LIBSUFFIX) 
	endif
	# Matrix stars
	ifdef SDFMATRIX 
		CUSTOM_DIRS += $(SDFDIR)/matrix/stars
		STARS += $(LIBDIR)/sdfmatrixstars.o
		LIBS += -lsdfmatrixstars
		LIBFILES += $(LIBDIR)/libsdfmatrixstars.$(LIBSUFFIX)
	endif
	# Matlab interface stars
	ifdef SDFMATLAB
		CUSTOM_DIRS += $(SDFDIR)/matlab/stars
		STARS += $(LIBDIR)/sdfmatlabstars.o
		LIBS += -lsdfmatlabstars
		LIBFILES += $(LIBDIR)/libsdfmatlabstars.$(LIBSUFFIX)
		# dependencies
		EXTERNALTOOLSLIB = 1
		MATLABENGINE = 1
	endif
	# Contributed stars by third-party users
	ifdef SDFCONTRIB
		CUSTOM_DIRS += $(SDFDIR)/contrib/kernel $(SDFDIR)/contrib/stars
		STARS += $(LIBDIR)/sdfcontribstars.o
		LIBS += -lsdfcontribstars -lsdfcontrib 
		LIBFILES += $(LIBDIR)/libsdfcontribstars.$(LIBSUFFIX) \
			$(LIBDIR)/libsdfcontrib.$(LIBSUFFIX)
	endif
	# Ultrasparc Visual Instruction Set (VIS) stars
	ifdef SDFVIS
		CUSTOM_DIRS += $(SDFDIR)/vis/stars
		STARS += $(LIBDIR)/sdfvisstars.o
		LIBS += -lsdfvisstars
		LIBS += $(VSDKHOME)/util/vis.il
	endif
	ifdef CG
		# FIXME: The only thing in sdf/targets also depends on 
		# cg... should this be in the cg directory?
		CUSTOM_DIRS += $(SDFDIR)/targets
		TARGETS += $(SDFT)/CompileTarget.o
	endif
	# kernel and stars
	CUSTOM_DIRS += $(SDFDIR)/stars
	STARS += $(LIBDIR)/sdfstars.o 
	LIBS += -lsdfstars
	LIBFILES += $(LIBDIR)/libsdfstars.$(LIBSUFFIX)	
	# dependencies
	SDFLIB = 1
	# ptdsp library is used by the Play star as well as the SDF/DSP stars
	PTDSPLIB = 1
endif

# Dependent libraries

ifdef ATM
	CUSTOM_DIRS += $(SDFDIR)/atm/kernel
	LIBS += -latm
	LIBFILES += $(LIBDIR)/libatm.$(LIBSUFFIX)
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
	# dependencies
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
	$(CROOT)/src/pitcl \
	$(CROOT)/mk $(CROOT)/src/tysh $(CROOT)/src/pigiExample

ifdef PITCL
	# Under some architectures, we need to use static libraries or we get
	# GateKeeper errors.
	ifdef USE_CORE_STATIC_LIBS
		LIBPTCL = $(LIBDIR)/libpitcl.a
	else
		LIBPTCL = $(LIBDIR)/libpitcl.$(LIBSUFFIX)
	endif
else
	# Use libptcl, not libpitcl
	ifdef USE_CORE_STATIC_LIBS
		LIBPTCL = $(LIBDIR)/libptcl.a
	else
		LIBPTCL = $(LIBDIR)/libptcl.$(LIBSUFFIX)
	endif
endif

PT_DEPEND += $(LIBPTCL) $(LIBDIR)/libptolemy.a \
	$(LIBFILES) $(STARS) $(TARGETS)

# unfortunately, if we are building a stand-alone program and
# link in libPar we must also get gantt function definitions in ptcl
ifndef PIGI
	ifdef CGPAR
		PIGI=1
	endif
endif

# this would not be defined if we are making a small stand-alone 
# Program to test the ptolemy libraries, see standalone.mk
ifdef PIGI
	ifdef PITCL
		LIBS += version.o -lpitcl
	else
		LIBS += version.o -lptcl
	endif
endif

# External C library of Ptolemy DSP C routines
ifdef PTDSPLIB
	CUSTOM_DIRS += $(CROOT)/src/utils/libptdsp
	LIBS += -lptdsp
	LIBFILES += $(LIBDIR)/libptdsp.a
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

