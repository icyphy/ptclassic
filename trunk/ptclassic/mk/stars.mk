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

CG56T = $(OBJDIR)/domains/cg56/targets
CG96T = $(OBJDIR)/domains/cg96/targets
CGCT = $(OBJDIR)/domains/cgc/targets
CGCTCL = $(OBJDIR)/domains/cgc/tcltk/targets
CGT = $(OBJDIR)/domains/cg/targets
SDFT = $(OBJDIR)/domains/sdf/targets

# Tcl/Tk stars can be used in pigiRpc but not ptcl, for this we define TK

ifdef CG96
	CG = 1
	PALETTES += PTOLEMY/src/domains/cg96/icons/cg96.pal
	STARS += $(LIBDIR)/cg96dspstars.o $(LIBDIR)/cg96stars.o
	TARGETS += $(CG96T)/Sim96Target.o
	LIBS += -lcg96dspstars -lcg96stars -lcg96
	LIBFILES += $(LIBDIR)/libcg96dspstars.$(LIBSUFFIX) \
		$(LIBDIR)/libcg96stars.$(LIBSUFFIX) \
		$(LIBDIR)/libcg96.$(LIBSUFFIX)
endif

ifdef CG56
	CG = 1
	PALETTES += PTOLEMY/src/domains/cg56/icons/main.pal
	STARS += $(LIBDIR)/cg56dspstars.o $(LIBDIR)/cg56stars.o
	LIBS += -lcg56dspstars -lcg56stars -lcg56
	ifeq ($(USE_SHARED_LIBS),yes) 
		LIBS += -lcg56targets
		LIBFILES += $(LIBDIR)/libcg56targets.$(LIBSUFFIX)
	else
		TARGETS += $(CG56T)/Sim56Target.o $(CG56T)/S56XTarget.o \
			$(CG56T)/S56XTargetWH.o \
			$(CG56T)/Sub56Target.o $(CG56T)/CG56MultiSimTarget.o \
			$(CG56T)/CG56MultiSimSend.o \
			$(CG56T)/CG56MultiSimReceive.o \
			$(CG56T)/CG56XCReceive.o $(CG56T)/CG56XCSend.o \
			$(CG56T)/CG56XCPeek.o $(CG56T)/CG56XCPoke.o \
			$(CG56T)/CGCXReceive.o $(CG56T)/CGCXSend.o \
			$(CG56T)/CGCXPeek.o $(CG56T)/CGCXPoke.o \
	 		$(CG56T)/CGCXSynchComm.o $(CG56T)/CG56XCSynchComm.o \
			$(CG56T)/CGCXAsynchComm.o $(CG56T)/CG56XCAsynchComm.o \
			$(CG56T)/CGCXBase.o  $(CG56T)/CGCS56XTarget.o
	endif
endif

ifdef SILAGE
	PALETTES += PTOLEMY/src/domains/silage/icons/silage.pal
	STARS += $(LIBDIR)/silagestars.o
	LIBS += -lsilagestars -lsilage
	LIBFILES += $(LIBDIR)/libsilagestars.$(LIBSUFFIX) \
		$(LIBDIR)/libsilage.$(LIBSUFFIX)
endif

ifdef DDF
	PALETTES += PTOLEMY/src/domains/ddf/icons/ddf.pal
	STARS += $(LIBDIR)/ddfstars.o
	LIBS += -lddfstars -lddf
	LIBFILES += $(LIBDIR)/libddfstars.$(LIBSUFFIX) \
		$(LIBDIR)/libddf.$(LIBSUFFIX)
endif

ifdef THOR
	PALETTES += PTOLEMY/src/domains/thor/icons/thor.pal
	STARS += $(LIBDIR)/thorstars.o
	LIBS += -lthorstars -lthor
	LIBFILES += $(LIBDIR)/libthorstars.$(LIBSUFFIX) \
		$(LIBDIR)/libthor.$(LIBSUFFIX)
endif

ifdef CGCFULL
	CGC = 1
	CGCTK = 1
endif

ifdef CGFULL
	CG = 1
	CGPAR = 1
endif

ifdef CGDDF
	CGC = 1
	STARS += $(LIBDIR)/cgddfstars.o
	TARGETS += $(CGCT)/main/CGCDDFTarget.o
	LIBS += -lcgddf
	LIBFILES += $(LIBDIR)/libddfstars.$(LIBSUFFIX) \
		$(LIBDIR)/libddf.$(LIBSUFFIX)
else
	#FIXME!!  We shouldn't have to include CGDDF if we want CGPAR!
	#This is for a CGMultiTarget dependency
	ifdef CGPAR
		LIBS += -lcgddf
	endif
endif

ifdef VHDL
	PALETTES += PTOLEMY/src/domains/vhdlf/icons/vhdlf.pal
	PALETTES += PTOLEMY/src/domains/vhdlb/icons/vhdlb.pal
	CG = 1
	STARS += $(LIBDIR)/vhdlfstars.o $(LIBDIR)/vhdlbstars.o
	LIBS += -lvhdlfstars -lvhdlf -lvhdlbstars -lvhdlb
	LIBFILES += $(LIBDIR)/libvhdlfstars.$(LIBSUFFIX) \
		$(LIBDIR)/libvhdlf.$(LIBSUFFIX) \
		$(LIBDIR)/libvhdlbstars.$(LIBSUFFIX) \
		$(LIBDIR)/libvhdlb.$(LIBSUFFIX)
endif

ifdef MDSDF
	PALETTES += PTOLEMY/src/domains/mdsdf/icons/mdsdf.pal
	SDF = 1
	STARS +=  $(LIBDIR)/mdsdfstars.o
	LIBS += -lmdsdfstars -lmdsdf
	LIBFILES += $(LIBDIR)/libmdsdfstars.$(LIBSUFFIX) \
		$(LIBDIR)/libmdsdf.$(LIBSUFFIX)
	ifdef TK 
		STARS += $(LIBDIR)/mdsdftclstars.o
		LIBFILES += $(LIBDIR)/libmdsdftclstars.$(LIBSUFFIX)
		LIBS += -lmdsdftclstars
	endif
endif

ifdef CP
	PALETTES += PTOLEMY/src/domains/cp/icons/cp.pal
	LPW = 1
	STARS += $(LIBDIR)/cpstars.o $(LIBDIR)/cpipstars.o
	LIBS += -lcpstars -lcpipstars -lcp -laudio
	LIBFILES += $(LIBDIR)/libcpstars.$(LIBSUFFIX) \
		$(LIBDIR)/libcpipstars.$(LIBSUFFIX) \
		$(LIBDIR)/libcp.$(LIBSUFFI
endif

ifdef LPW
	LIBS += -llwpthread -llwp
	LIBFILES += $(LIBDIR)/liblwpthread.$(LIBSUFFIX)
endif

ifdef PN
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

ifndef TK
	IPUS=
endif

ifdef IPUS
	PALETTES += PTOLEMY/src/domains/ipus/icons/ipus.pal
	ifdef TK
		STARS += $(LIBDIR)/ipustclstars.o
		LIBFILES += $(LIBDIR)/libipustclstars.$(LIBSUFFIX) \
			$(LIBDIR)/libipustcltk.$(LIBSUFFIX)
		LIBS += -lipustclstars -lipustcltk
	endif
	STARS += $(LIBDIR)/ipusstars.o
	LIBS += -lipusstars -lipus -licp
	LIBFILES += $(LIBDIR)/libipusstars.$(LIBSUFFIX) \
			$(LIBDIR)/libipus.$(LIBSUFFIX) \
			$(LIBDIR)/libicp.$(LIBSUFFIX)
endif

ifdef MQ
	SDF = 1
	DE = 1
	ATMSTARS = $(LIBDIR)/mqstars.o $(LIBDIR)/deatmstars.o \
		$(LIBDIR)/sdfatmstars.o
	ATM_LIBFILES = $(LIBDIR)/libmq.$(LIBSUFFIX) \
		$(LIBDIR)/libmqstars.$(LIBSUFFIX) \
		$(LIBDIR)/libdeatmstars.$(LIBSUFFIX) \
		$(LIBDIR)/libsdfatmstars.$(LIBSUFFIX) \
		$(LIBDIR)/libatm.$(LIBSUFFIX)
	ATM_LIBS= -lmqstars -lmq -ldeatmstars -lsdfatmstars -latm
endif

ifdef DE
	PALETTES += PTOLEMY/src/domains/de/icons/de.pal
	STARS += $(LIBDIR)/destars.o
	LIBS += -ldestars -lde
	LIBFILES += $(LIBDIR)/libdestars.$(LIBSUFFIX) \
		$(LIBDIR)/libde.$(LIBSUFFIX)
	ifdef TK
		STARS += $(LIBDIR)/detclstars.o
		LIBS += -ldetclstars
		LIBFILES += $(LIBDIR)/libdetclstars.$(LIBSUFFIX)
	endif
	#FIXME - This is ugly, there are DE stars that depend on the image lib
	ifndef SDFIMAGE
		LIBS += -lImage
		LIBFILES += $(LIBDIR)/libImage.$(LIBSUFFIX)
	endif
endif

ifdef CM5
	CGC = 1
	TARGETS += $(CGCT)/cm5/CGCcm5Send.o $(CGCT)/cm5/CGCcm5Recv.o \
		$(CGCT)/cm5/CGCcm5Target.o $(CGCT)/cm5/CGCcm5peTarget.o
endif

ifdef CGC
	PALETTES += PTOLEMY/src/domains/cgc/icons/cgc.pal
	ifdef CGCTK
		STARS += $(LIBDIR)/cgctcltkstars.o
		LIBS += -lcgctcltk
		LIBFILES += $(LIBDIR)/libcgctcltk.$(LIBSUFFIX)
	endif
	CG = 1
	STARS += $(LIBDIR)/cgcstars.o
	ifeq ($(USE_SHARED_LIBS),yes) 
		TARGETS += $(CGCTCL)/CGCTclTkTarget.o
		LIBS += -lcgctargets
		LIBFILES += $(LIBDIR)/libcgctargets.$(LIBSUFFIX)
	else
		TARGETS += $(CGCT)/main/CGCUnixSend.o \
			$(CGCT)/main/CGCUnixReceive.o \
			$(CGCT)/main/CGCMultiTarget.o \
			$(CGCTCL)/CGCTclTkTarget.o \
			$(CGCT)/main/CGCMacroStar.o $(CGCT)/main/CGCDDFCode.o \
			$(CGCT)/main/CGCSDFSend.o \
			$(CGCT)/main/CGCSDFReceive.o \
			$(CGCT)/main/CGCSDFBase.o \
			$(CGCT)/main/CGCTargetWH.o
	endif
	LIBS += -lcgcstars -lcgc
	LIBFILES += $(LIBDIR)/libcgcstars.$(LIBSUFFIX) \
		$(LIBDIR)/libcgc.$(LIBSUFFIX)
endif

ifdef BDF
	PALETTES += PTOLEMY/src/domains/bdf/icons/bdf.pal
	SDF = 1
	STARS += $(LIBDIR)/bdfstars.o
	ifdef CG
		TARGETS += $(CGT)/CGBDFTarget.o 
	endif
	ifdef CGC
		TARGETS += $(CGCT)/main/CGCBDFTarget.o
	endif
	LIBS += -lbdfstars -lbdf
	LIBFILES += $(LIBDIR)/libbdfstars.$(LIBSUFFIX) \
		$(LIBDIR)/libbdf.$(LIBSUFFIX)
else
	#FIXME - you need bdf in CGCBDFTarget.... maybe it shouldn't be
	#part of the cgc target shared library?
	ifdef CGC
		LIBS += -lbdf
	endif
endif

ifdef CG
	PALETTES += PTOLEMY/src/domains/cg/icons/cg.pal
	ifdef CGPAR
		LIBS += -lMacro -lDC -lHu -lDL -lMSH -lPar -lcg
		TARGETS += $(CGT)/CGMultiTarget.o $(CGT)/CGSharedBus.o 
		LIBFILES += $(LIBDIR)/libDC.$(LIBSUFFIX) \
			$(LIBDIR)/libHu.$(LIBSUFFIX) \
			$(LIBDIR)/libMacro.$(LIBSUFFIX) \
			$(LIBDIR)/libcgddf.$(LIBSUFFIX) \
			$(LIBDIR)/libDL.$(LIBSUFFIX) \
			$(LIBDIR)/libMSH.$(LIBSUFFIX) \
			$(LIBDIR)/libPar.$(LIBSUFFIX) \
			$(LIBDIR)/libcgstars.$(LIBSUFFIX) \
			$(LIBDIR)/libcg.$(LIBSUFFIX)
	endif
	SDF = 1
	CGSTARS = $(LIBDIR)/cgstars.o
	TARGETS += $(SDFT)/CompileTarget.o
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
	endif
endif

ifndef TK
	SDFTK=
endif 

ifdef SDF 
	PALETTES += PTOLEMY/src/domains/sdf/icons/sdf.pal
	ifdef SDFTK
		SDFDSP=1
		STARS += $(LIBDIR)/sdftclstars.o
		LIBFILES += $(LIBDIR)/libsdftclstars.$(LIBSUFFIX)
		LIBS += -lsdftclstars
	endif
	ifdef SDFDSP 
		STARS += $(LIBDIR)/sdfdspstars.o
		LIBS += -lsdfdspstars
		LIBFILES += $(LIBDIR)/libsdfdspstars.$(LIBSUFFIX)
	endif
	ifdef SDFMATRIX 
		STARS += $(LIBDIR)/sdfmatrixstars.o
		LIBS += -lsdfmatrixstars
		LIBFILES += $(LIBDIR)/libsdfmatrixstars.$(LIBSUFFIX)
	endif
	ifdef SDFIMAGE
		STARS += $(LIBDIR)/sdfimagestars.o
		LIBS += -lsdfimagestars -lImage
		LIBFILES += $(LIBDIR)/libsdfimagestars.$(LIBSUFFIX) \
			 $(LIBDIR)/libImage.$(LIBSUFFIX)
	endif
	ifdef SDFMATLAB
		STARS += $(MATLABSTARS_DOT_O)
		LIBS += $(MATLABSTAR_LIB) $(MATLABEXT_LIB)
		LIBFILES += $(MATLABSTAR_LIBFILE)
	endif
	ifdef SDFCONTRIB
		STARS += $(LIBDIR)/sdfcontribstars.o
		LIBFILES += $(LIBDIR)/libsdfcontribstars.$(LIBSUFFIX)
		LIBS += -lsdfcontribstars
	endif
	STARS += $(LIBDIR)/sdfstars.o 
	TARGETS += $(OBJDIR)/domains/sdf/loopScheduler/LoopTarget.o \
		
	LIBS += -lLS -lsdfstars -lsdf
	LIBFILES += $(LIBDIR)/libsdfstars.$(LIBSUFFIX) \
		$(LIBDIR)/libLS.$(LIBSUFFIX) \
		$(LIBDIR)/libsdf.$(LIBSUFFIX)
endif

# HOF stars can be used in pigiRpc but not ptcl.
# The HOF base classes call Tk_DoOneEvent so that if you accidentally
# specify an infinite recursion (easy to do with HOF), you can hit the
# STOP button and abort the run.  But Tk is not linked into ptcl, so
# this call cannot be done.
ifdef HOF
	STARS += $(LIBDIR)/hofstars.o
	LIBS +=	-lhofstars -lhof
	LIBFILES += $(LIBDIR)/libhofstars.$(LIBSUFFIX) \
		$(LIBDIR)/libhof.$(LIBSUFFIX)
endif

ifeq ($(USE_SHARED_LIBS),yes) 
	STARS =
endif

ifeq ($(NEED_PALETTES),yes)
	PALETTES =		defpalettes-pigi.o
endif

ifdef TK
	LIBS += -L$(OCTLIBDIR) -lrpc -lpigi -lptk -lgantt \
		-loh -lvov -lrpc -llist -ltr -lutility -lst \
		-lerrtrap -luprintf -lport \
		 $(ITK_LIBSPEC) $(TK_LIBSPEC) $(X11_LIBSPEC) 
	PT_DEPEND += $(OCTLIBDIR)/librpc.$(LIBSUFFIX) \
		$(LIBDIR)/libpigi.$(LIBSUFFIX) \
		$(LIBDIR)/libptk.a \
		$(LIBDIR)/libgantt.$(LIBSUFFIX) \
		$(OCTLIBDIR)/liboh.$(LIBSUFFIX) \
		$(OCTLIBDIR)/libvov.$(LIBSUFFIX) \
		$(OCTLIBDIR)/liblist.$(LIBSUFFIX) \
		$(OCTLIBDIR)/libtr.$(LIBSUFFIX) \
		$(OCTLIBDIR)/libutility.$(LIBSUFFIX) \
		$(OCTLIBDIR)/libst.$(LIBSUFFIX) \
		$(OCTLIBDIR)/liberrtrap.$(LIBSUFFIX) \
		$(OCTLIBDIR)/libuprintf.$(LIBSUFFIX) \
		$(OCTLIBDIR)/libport.$(LIBSUFFIX)

endif

PT_DEPEND += $(LIBDIR)/libptcl.$(LIBSUFFIX) $(LIBDIR)/libptolemy.a \
	$(LIBFILES) $(STARS)

LIBS += -lptolemy $(ITCL_LIBSPEC) $(TCL_LIBSPEC) $(SYSLIBS) $(LIB_FLUSH_CACHE)
