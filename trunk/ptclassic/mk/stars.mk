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
# ROOT		the root of the ptolemy tree (e.g., $PTOLEMY).  Typically
#		a relative path.
# OBJDIR	the root of the object file tree for this arch.
#		it can be either relative to current directory or absolute.
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

CG56T = $(OBJDIR)/domains/cg56/targets
CG96T = $(OBJDIR)/domains/cg96/targets
CGCT = $(OBJDIR)/domains/cgc/targets
CGCTCL = $(OBJDIR)/domains/cgc/tcltk/targets
CGT = $(OBJDIR)/domains/cg/targets
SDFT = $(OBJDIR)/domains/sdf/targets
VHDLT = $(OBJDIR)/domains/vhdl/targets

BDFDIR = $(CROOT)/src/domains/bdf
CGCDIR = $(CROOT)/src/domains/cgc
CG96DIR = $(CROOT)/src/domains/cg96
CG56DIR = $(CROOT)/src/domains/cg56
SILAGEDIR = $(CROOT)/src/domains/silage
THORDIR = $(CROOT)/src/domains/thor
CGDDFDIR = $(CROOT)/src/domains/cg
DDFDIR = $(CROOT)/src/domains/ddf
VHDLFDIR = $(CROOT)/src/domains/vhdlf
VHDLBDIR = $(CROOT)/src/domains/vhdlb
VHDLDIR = $(CROOT)/src/domains/vhdl
MDSDFDIR = $(CROOT)/src/domains/mdsdf
CPDIR = $(CROOT)/src/domains/cp
PNDIR = $(CROOT)/src/domains/pn
IPUSDIR = $(CROOT)/src/domains/ipus
MQDIR = $(CROOT)/src/domains/mq
DEDIR = $(CROOT)/src/domains/de
CGDIR = $(CROOT)/src/domains/cg
SDFDIR = $(CROOT)/src/domains/sdf
HOFDIR = $(CROOT)/src/domains/hof

# Tcl/Tk stars can be used in pigiRpc but not ptcl, for this we define TK

ifndef TK
	# We can't include the ipus domain since the non-tk ipus stars need 
	# Blackboard* symbols, which are defined in the tk ipus stars.
	# Set up definitions for stars.mk
	IPUS=

	SDFTK=
	HOF=
	CODESIGN=
endif 

# Motorola DSP assembly code generation domains

ifdef CG96
	CUSTOM_DIRS += $(CG96DIR)/kernel $(CG96DIR)/stars \
		$(CG96DIR)/targets $(CG96DIR)/dsp/stars
	CG = 1
	PALETTES += PTOLEMY/src/domains/cg96/icons/cg96.pal
	STARS += $(LIBDIR)/cg96dspstars.o $(LIBDIR)/cg96stars.o
	LIBS += -lcg96targets -lcg96dspstars -lcg96stars -lcg96
	LIBFILES += $(LIBDIR)/libcg96targets.$(LIBSUFFIX) \
		$(LIBDIR)/libcg96dspstars.$(LIBSUFFIX) \
		$(LIBDIR)/libcg96stars.$(LIBSUFFIX) \
		$(LIBDIR)/libcg96.$(LIBSUFFIX)
endif

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
	# Window star in cg56/dsp/stars needs the Cephes Library
	CEPHESLIB = 1
	# CG56 targets need CGCStar
	CGCLIB = 1
endif

ifdef SILAGE
	CUSTOM_DIRS += $(SILAGEDIR)/kernel $(SILAGEDIR)/stars \
		$(SILAGEDIR)/targets
	CG = 1
	SDFLIB = 1
	PALETTES += PTOLEMY/src/domains/silage/icons/silage.pal
	STARS += $(LIBDIR)/silagestars.o
	LIBS += -lsilagestars -lsilage
	LIBFILES += $(LIBDIR)/libsilagestars.$(LIBSUFFIX) \
		$(LIBDIR)/libsilage.$(LIBSUFFIX)
endif

ifdef THOR
	CUSTOM_DIRS += $(THORDIR)/kernel $(THORDIR)/stars \
		$(THORDIR)/pepp $(THORDIR)/analyzerX11
	PALETTES += PTOLEMY/src/domains/thor/icons/thor.pal
	STARS += $(LIBDIR)/thorstars.o
	LIBS += -lthorstars -lthor
	LIBFILES += $(LIBDIR)/libthorstars.$(LIBSUFFIX) \
		$(LIBDIR)/libthor.$(LIBSUFFIX)
endif

ifdef CGDDF
	CUSTOM_DIRS += $(CGDDFDIR)/ddfScheduler
	CGDDFLIB = 1
	DDF = 1
	CGC = 1
	CGFULL = 1
	TARGETS += $(CGCT)/main/CGCDDFTarget.o
endif

ifdef CGCFULL
	CGC = 1
	CGFULL = 1
	CM5 = 1
	NOWAM = 1
	ifdef TK
		CGCTK = 1
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
	LIBS += -lvhdlstars -lvhdl -lvhdltargets
	LIBFILES += $(LIBDIR)/libvhdlstars.$(LIBSUFFIX) \
		$(LIBDIR)/libvhdl.$(LIBSUFFIX) \
		$(LIBDIR)/libvhdltargets.$(LIBSUFFIX)
endif

ifdef VHDLF
	CUSTOM_DIRS += $(VHDLFDIR)/kernel $(VHDLFDIR)/stars
	CG = 1
	SDFLIB = 1
	PALETTES += PTOLEMY/src/domains/vhdlf/icons/vhdlf.pal
	STARS += $(LIBDIR)/vhdlfstars.o
	LIBS += -lvhdlfstars -lvhdlf
	LIBFILES += $(LIBDIR)/libvhdlfstars.$(LIBSUFFIX) \
		$(LIBDIR)/libvhdlf.$(LIBSUFFIX)
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

ifdef CP
	# The CP domain is only supported under sun4
	ifneq (,$(filter sun%, $(PTARCH)))
		CUSTOM_DIRS += $(CPDIR)/kernel $(CPDIR)/stars \
			$(CPDIR)/infopad/kernel $(CPDIR)/infopad/stars
		PALETTES += PTOLEMY/src/domains/cp/icons/cp.pal
		LWP = 1
		STARS += $(LIBDIR)/cpstars.o $(LIBDIR)/cpipstars.o
		LIBS += -lcpstars -lcpipstars -lcp -laudio
		LIBFILES += $(LIBDIR)/libcpstars.$(LIBSUFFIX) \
			$(LIBDIR)/libcpipstars.$(LIBSUFFIX) \
			$(LIBDIR)/libcp.$(LIBSUFFIX)
	else
		# This line will not echo, but it will produce an error
		echo "The CP domain is only supported under sun4"
	endif
endif

ifdef LWP
	LIBS += -llwpthread -llwp
	LIBFILES += $(LIBDIR)/liblwpthread.$(LIBSUFFIX)
endif

ifdef PN
	CUSTOM_DIRS += $(PNDIR)/kernel $(PNDIR)/stars 
	SDFLIB = 1
	PALETTES += PTOLEMY/src/domains/pn/icons/pn.pal
	ifneq (,$(filter sun% sol%,$(PTARCH)))
		STARS += $(LIBDIR)/pnstars.o
		LIBS += -lpnstars -lpn
		LIBFILES += $(LIBDIR)/libpnstars.$(LIBSUFFIX) \
			$(LIBDIR)/libpn.$(LIBSUFFIX)
		# POSIX thread library from Florida State University.
		LIBS += -lposixthread -L$(ROOT)/thread/lib.$(PTARCH) -lgthreads
		LIBFILES += $(LIBDIR)/libposixthread.$(LIBSUFFIX)
	endif
endif

# We can't include the ipus domain since the non-tk ipus stars need 
# Blackboard* symbols, which are defined in the tk ipus stars.
# Set up definitions for stars.mk

ifdef IPUS
	ifdef TK
		CUSTOM_DIRS += $(IPUSDIR)/kernel $(IPUSDIR)/stars \
				 $(IPUSDIR)/tcltk/kernel \
				 $(IPUSDIR)/tcltk/stars 
		PALETTES += PTOLEMY/src/domains/ipus/icons/ipus.pal
		STARS += $(LIBDIR)/ipustclstars.o $(LIBDIR)/ipusstars.o
		LIBS += -lipustclstars -lipusstars -lipus \
			-lipustcltk -licp
		LIBFILES += $(LIBDIR)/libipustclstars.$(LIBSUFFIX) \
			$(LIBDIR)/libipustcltk.$(LIBSUFFIX) \
			$(LIBDIR)/libipusstars.$(LIBSUFFIX) \
			$(LIBDIR)/libipus.$(LIBSUFFIX) \
			$(LIBDIR)/libicp.$(LIBSUFFIX)
	endif
endif

ifdef MQ
	ATM = 1
	CUSTOM_DIRS += $(MQDIR)/kernel $(MQDIR)/stars
	ATMSTARS = $(LIBDIR)/mqstars.o
	LIBS += -lmqstars -lmq 
	LIBFILES += $(LIBDIR)/libmqstars.$(LIBSUFFIX) \
		$(LIBDIR)/libmq.$(LIBSUFFIX)
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

ifdef CM5
	CUSTOM_DIRS += $(ROOT)/src/domains/cgc/targets/cm5
	CGC = 1
	TARGETS += $(CGCT)/cm5/CGCcm5Send.o $(CGCT)/cm5/CGCcm5Recv.o \
		$(CGCT)/cm5/CGCcm5Target.o $(CGCT)/cm5/CGCcm5peTarget.o
endif

ifdef NOWAM # Networks Of Workstations Active Messages
     # NOWam only supported under sol2 and sol2.cfront
     ifneq (,$(filter sol%,$(PTARCH)))
	CUSTOM_DIRS += $(ROOT)/src/domains/cgc/targets/NOWam/NOWam
	CGC = 1
	ifeq ($(USE_SHARED_LIBS),yes) 
		LIBS += -lNOWam
		LIBFILES += $(LIBDIR)/libNOWam.$(LIBSUFFIX)
	else
		TARGETS += $(CGCT)/NOWam/NOWam/CGCNOWamRecv.o \
			$(CGCT)/NOWam/NOWam/CGCNOWamSend.o \
			$(CGCT)/NOWam/NOWam/CGCNOWamTarget.o
	endif
     endif #sol2 and sol2.cfront
endif


ifdef CGC
	CUSTOM_DIRS += $(CGCDIR)/stars
	ifdef CGCTK
		CUSTOM_DIRS += $(CGCDIR)/tcltk/stars $(CGCDIR)/tcltk/targets
		STARS += $(LIBDIR)/cgctcltkstars.o
		LIBS += -lcgctcltk
		LIBFILES += $(LIBDIR)/libcgctcltk.$(LIBSUFFIX)
		TARGETS += $(CGCTCL)/CGCTclTkTarget.o
	endif
	CGCLIB = 1
	CG = 1
	SDFLIB = 1
	BDFLIB = 1
	ifdef CGPAR
		CUSTOM_DIRS += $(CGCDIR)/targets/main
		ifeq ($(USE_SHARED_LIBS),yes) 
			LIBS += -lcgctargets
			LIBFILES += $(LIBDIR)/libcgctargets.$(LIBSUFFIX)
		else
			TARGETS += $(CGCT)/main/CGCUnixSend.o \
				$(CGCT)/main/CGCUnixReceive.o \
				$(CGCT)/main/CGCMultiTarget.o \
				$(CGCT)/main/CGCMacroStar.o \
				$(CGCT)/main/CGCDDFCode.o \
				$(CGCT)/main/CGCSDFSend.o \
				$(CGCT)/main/CGCSDFReceive.o \
				$(CGCT)/main/CGCSDFBase.o \
				$(CGCT)/main/CreateSDFStar.o \
				$(CGCT)/main/CompileCGSubsystems.o
		endif
	endif
	PALETTES += PTOLEMY/src/domains/cgc/icons/cgc.pal
	STARS += $(LIBDIR)/cgcstars.o
	CUSTOM_DIRS += $(CGCDIR)/stars $(CGCDIR)/kernel
	LIBS += -lcgcstars
	LIBFILES += $(LIBDIR)/libcgcstars.$(LIBSUFFIX)
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
		TARGETS += $(CGCT)/main/CGCBDFTarget.o
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
	ifdef TK
		SDFTK = 1
		SDFDFM = 1
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
		# Cephes library is used by the Window star
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
	CUSTOM_DIRS += $(SDFDIR)/stars
	STARS += $(LIBDIR)/sdfstars.o 
	ifdef CG
		# The only thing in sdf/targets also depends on 
		# cg... should this be in the cg directory
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
    LIBS += 	-L$(OCTLIBDIR) -lpigi \
		-lrpc -loh -lvov -lrpc -llist \
		-ltr -lutility -lst -lerrtrap -luprintf -lport

    LIBFILES += $(LIBDIR)/libpigi.$(LIBSUFFIX) \
	$(OCTLIBDIR)/librpc.$(LIBSUFFIX)  \
	$(OCTLIBDIR)/liboh.$(LIBSUFFIX) $(OCTLIBDIR)/libvov.$(LIBSUFFIX) \
	$(OCTLIBDIR)/liblist.$(LIBSUFFIX) \
	$(OCTLIBDIR)/libtr.$(LIBSUFFIX) $(OCTLIBDIR)/libutility.$(LIBSUFFIX) \
	$(OCTLIBDIR)/libst.$(LIBSUFFIX) $(OCTLIBDIR)/liberrtrap.$(LIBSUFFIX) \
	$(OCTLIBDIR)/libuprintf.$(LIBSUFFIX) $(OCTLIBDIR)/libport.$(LIBSUFFIX)
endif

ifdef TK
	CUSTOM_DIRS += $(CROOT)/src/ptklib 
	LIBS += -lptk $(ITK_LIBSPEC) $(TK_LIBSPEC) $(X11_LIBSPEC) 
	PT_DEPEND += $(LIBDIR)/libptk.a
endif

CUSTOM_DIRS += $(CROOT)/src/kernel $(CROOT)/src/pigiRpc $(CROOT)/src/ptcl \
	$(CROOT)/mk $(CROOT)/src/tycho/tysh

ifdef USE_CORE_STATIC_LIBS
	LIBPTCL = $(LIBDIR)/libptcl.a
else
	LIBPTCL = $(LIBDIR)/libptcl.$(LIBSUFFIX)
endif

PT_DEPEND += $(LIBDIR)/libptcl.$(LIBSUFFIX) $(LIBDIR)/libptolemy.a \
	$(LIBFILES) $(STARS) $(TARGETS)

# this would not be defined if we are making a small stand-alone 
# program to test the ptolemy libraries, see standalone.mk
ifdef PIGI
	LIBS += version.o -lptcl 
endif

# External interface support - we need to expand libexttools, because it
# depends upon the Ptolemy kernel.  Matlab always is pulled in

CUSTOM_DIRS += $(CROOT)/src/utils/libexttools
LIBS += -lexttools $(MATLABEXT_LIB) $(MATHEMATICAEXT_LIB)
LIBFILES += $(LIBDIR)/libexttools.$(LIBSUFFIX)

# Now, pull in the Ptolemy kernel support, system libraries and tcl
LIBS += -lptolemy $(ITCL_LIBSPEC) $(TCL_LIBSPEC) \
	$(SYSLIBS) $(LIB_FLUSH_CACHE)

