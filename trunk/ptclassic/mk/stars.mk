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
PTINY_SDFSTARS = $(LIBDIR)/sdfstars.o $(LIBDIR)/sdfdspstars.o \
	$(LIBDIR)/sdfmatrixstars.o
SDFSTARS = $(PTINY_SDFSTARS) $(LIBDIR)/sdfimagestars.o
CGCSTARS = $(LIBDIR)/cgcstars.o $(LIBDIR)/cgctcltkstars.o
CG96STARS = $(LIBDIR)/cg96dspstars.o $(LIBDIR)/cg96stars.o
CG56STARS = $(LIBDIR)/cg56dspstars.o $(LIBDIR)/cg56stars.o
SilageSTARS = $(LIBDIR)/silagestars.o
CGSTARS = $(LIBDIR)/cgstars.o
DDFSTARS = $(LIBDIR)/ddfstars.o
THORSTARS = $(LIBDIR)/thorstars.o
DESTARS = $(LIBDIR)/destars.o
CGDDFSTARS = $(LIBDIR)/cgddfstars.o 
BDFSTARS = $(LIBDIR)/bdfstars.o
VHDLFSTARS = $(LIBDIR)/vhdlfstars.o
VHDLBSTARS = $(LIBDIR)/vhdlbstars.o
ATMSTARS = $(LIBDIR)/mqstars.o $(LIBDIR)/deatmstars.o $(LIBDIR)/sdfatmstars.o
MDSDFSTARS = $(LIBDIR)/mdsdfstars.o

# parallel scheduler libraries.
PARLIBFILES = $(LIBDIR)/libDC.$(LIBSUFFIX) $(LIBDIR)/libHu.$(LIBSUFFIX) \
	$(LIBDIR)/libDL.$(LIBSUFFIX) $(LIBDIR)/libMSH.$(LIBSUFFIX) \
	$(LIBDIR)/libPar.$(LIBSUFFIX) \
	$(LIBDIR)/libcgstars.$(LIBSUFFIX) $(LIBDIR)/libcg.$(LIBSUFFIX)

# If S56DIR is defined, then compile in the S56X Wormhole target.
# This is defined in config-sun4.mk.
ifdef S56DIR
S56WH_LIB= -lqckMon
S56WH_LIBDIR= -L$(S56DIR)/lib
S56WH_O= $(CG56T)/S56XTargetWH.o 
endif

# if MATLABDIR is defined, then compile with Matlab external interface library
ifdef MATLABLIBDIR
MATLABEXT_LIB= -lmat
endif

# Library files reqd by stars.  Note that libptolemy.$(LIBSUFFIX) is not included.
PTINY_LIBFILES=\
$(LIBDIR)/libdestars.$(LIBSUFFIX) $(LIBDIR)/libde.$(LIBSUFFIX) \
$(LIBDIR)/libsdfdspstars.$(LIBSUFFIX) \
$(LIBDIR)/libsdfmatrixstars.$(LIBSUFFIX) \
$(LIBDIR)/libsdfstars.$(LIBSUFFIX) $(LIBDIR)/libLS.$(LIBSUFFIX) \
$(LIBDIR)/libsdf.$(LIBSUFFIX) \

PTRIM_LIBFILES=\
$(LIBDIR)/libcgcstars.$(LIBSUFFIX) $(LIBDIR)/libcgctcltk.$(LIBSUFFIX) \
$(LIBDIR)/libcgc.$(LIBSUFFIX) \
$(LIBDIR)/libcgstars.$(LIBSUFFIX) $(LIBDIR)/libcg.$(LIBSUFFIX) \
$(LIBDIR)/libddfstars.$(LIBSUFFIX) $(LIBDIR)/libddf.$(LIBSUFFIX) \
$(LIBDIR)/libdestars.$(LIBSUFFIX) $(LIBDIR)/libde.$(LIBSUFFIX) \
$(LIBDIR)/libsdfimagestars.$(LIBSUFFIX) $(LIBDIR)/libImage.$(LIBSUFFIX) \
$(LIBDIR)/libsdfdspstars.$(LIBSUFFIX) \
$(LIBDIR)/libsdfmatrixstars.$(LIBSUFFIX) \
$(LIBDIR)/libbdfstars.$(LIBSUFFIX) $(LIBDIR)/libbdf.$(LIBSUFFIX) \
$(LIBDIR)/libsdfstars.$(LIBSUFFIX) $(LIBDIR)/libLS.$(LIBSUFFIX) \
$(LIBDIR)/libsdf.$(LIBSUFFIX)

STAR_LIBFILES=\
$(ATM_LIBFILES) \
$(LIBDIR)/libcgcstars.$(LIBSUFFIX) $(LIBDIR)/libcgctcltk.$(LIBSUFFIX) \
$(LIBDIR)/libcgc.$(LIBSUFFIX) \
$(LIBDIR)/libcg96dspstars.$(LIBSUFFIX) \
$(LIBDIR)/libcg96stars.$(LIBSUFFIX) $(LIBDIR)/libcg96.$(LIBSUFFIX) \
$(LIBDIR)/libcg56dspstars.$(LIBSUFFIX) \
$(LIBDIR)/libcg56stars.$(LIBSUFFIX) $(LIBDIR)/libcg56.$(LIBSUFFIX) \
$(LIBDIR)/libsilagestars.$(LIBSUFFIX) $(LIBDIR)/libsilage.$(LIBSUFFIX) \
$(LIBDIR)/libcgstars.$(LIBSUFFIX) $(PARLIBFILES) $(LIBDIR)/libcg.$(LIBSUFFIX) \
$(LIBDIR)/libddfstars.$(LIBSUFFIX) $(LIBDIR)/libddf.$(LIBSUFFIX) \
$(LIBDIR)/libthorstars.$(LIBSUFFIX) $(LIBDIR)/libthor.$(LIBSUFFIX) \
$(LIBDIR)/libdestars.$(LIBSUFFIX) $(LIBDIR)/libde.$(LIBSUFFIX) \
$(LIBDIR)/libsdfimagestars.$(LIBSUFFIX) $(LIBDIR)/libImage.$(LIBSUFFIX) \
$(LIBDIR)/libsdfdspstars.$(LIBSUFFIX) \
$(LIBDIR)/libsdfmatrixstars.$(LIBSUFFIX) \
$(LIBDIR)/libbdfstars.$(LIBSUFFIX) $(LIBDIR)/libbdf.$(LIBSUFFIX) \
$(LIBDIR)/libsdfstars.$(LIBSUFFIX) $(LIBDIR)/libLS.$(LIBSUFFIX) \
$(LIBDIR)/libsdf.$(LIBSUFFIX) \
$(LIBDIR)/libvhdlfstars.$(LIBSUFFIX) $(LIBDIR)/libvhdlf.$(LIBSUFFIX) \
$(LIBDIR)/libvhdlbstars.$(LIBSUFFIX) $(LIBDIR)/libvhdlb.$(LIBSUFFIX) \
$(LIBDIR)/libmdsdfstars.$(LIBSUFFIX) $(LIBDIR)/libmdsdf.$(LIBSUFFIX) \
$(PN_LIBFILES)

ATM_LIBFILES = $(LIBDIR)/libmq.$(LIBSUFFIX) $(LIBDIR)/libmqstars.$(LIBSUFFIX) \
	$(LIBDIR)/libdeatmstars.$(LIBSUFFIX) \
	$(LIBDIR)/libsdfatmstars.$(LIBSUFFIX) $(LIBDIR)/libatm.$(LIBSUFFIX)

# CG-DDF no longer supported
#$(LIBDIR)/libcgddfstars.$(LIBSUFFIX) $(LIBDIR)/libcgddf.$(LIBSUFFIX)  \


# Library switches reqd by stars for a ptiny ptolemy. Note that we don't
# include the sdf image stars.  Need -lImage for the de ATM stars
PTINY_STAR_LIBS=\
-ldestars -lde \
-lImage -lsdfdspstars -lsdfstars -lLS -lsdf -lsdfmatrixstars \
$(MATLABLIBDIR) $(MATLABEXT_LIB) \

PTRIM_STAR_LIBS=\
-lcgcstars -lcgc -lcgctcltk \
-lcgstars -lcg \
-lddfstars -lddf \
-ldestars -lde \
-lbdfstars -lbdf \
-lsdfimagestars -lImage -lsdfdspstars -lsdfstars -lLS -lsdf -lsdfmatrixstars \
$(MATLABLIBDIR) $(MATLABEXT_LIB) \

# Library switches reqd by stars.  Note that -lptolemy is not included.
STAR_LIBS=\
$(ATM_LIBS) \
-lcgcstars -lcgc -lcgctcltk \
-lcg96dspstars -lcg96stars -lcg96 \
-lcg56dspstars -lcg56stars -lcg56 \
$(S56WH_LIBDIR) $(S56WH_LIB) \
-lsilagestars -lsilage \
-lcgstars -lDC -lHu -lDL -lMSH -lPar -lcg \
-lddfstars -lddf \
-lthorstars -lthor \
-ldestars -lde \
-lbdfstars -lbdf \
-lsdfimagestars -lImage -lsdfdspstars -lsdfstars -lLS -lsdf -lsdfmatrixstars \
$(MATLABLIBDIR) $(MATLABEXT_LIB) \
-lvhdlfstars -lvhdlf \
-lvhdlbstars -lvhdlb \
-lmdsdfstars -lmdsdf \
$(PN_LIBS)

ATM_LIBS= -lmqstars -lmq -ldeatmstars -lsdfatmstars -latm

# Extra targets
SDFT = $(OBJDIR)/domains/sdf/targets
CGT = $(OBJDIR)/domains/cg/targets
CGCT = $(OBJDIR)/domains/cgc/targets
CGCTCL = $(OBJDIR)/domains/cgc/tcltk/targets
CG56T = $(OBJDIR)/domains/cg56/targets
CG96T = $(OBJDIR)/domains/cg96/targets

SDFTARGETS =	$(OBJDIR)/domains/sdf/loopScheduler/LoopTarget.o 
CGTARGETS =	$(CGT)/CGMultiTarget.o $(CGT)/CGSharedBus.o \
		$(SDFT)/CompileTarget.o 
CGCTARGETS =	$(CGCT)/main/CGCUnixSend.o $(CGCT)/main/CGCUnixReceive.o \
		$(CGCT)/main/CGCMultiTarget.o $(CGCTCL)/CGCTclTkTarget.o
CGCDDFTARGETS =	$(CGCT)/main/CGCDDFTarget.o
BDFTARGETS =	$(CGT)/CGBDFTarget.o $(CGCT)/main/CGCBDFTarget.o
CG56TARGETS =	$(CG56T)/Sim56Target.o $(CG56T)/S56XTarget.o $(S56WH_O)\
		$(CG56T)/Sub56Target.o $(CG56T)/CG56MultiSimTarget.o \
		$(CG56T)/CG56MultiSimSend.o $(CG56T)/CG56MultiSimReceive.o \
		$(CG56T)/CG56XCReceive.o $(CG56T)/CG56XCSend.o \
		$(CG56T)/CG56XCPeek.o $(CG56T)/CG56XCPoke.o \
		$(CG56T)/CGCXReceive.o $(CG56T)/CGCXSend.o \
		$(CG56T)/CGCXPeek.o $(CG56T)/CGCXPoke.o \
	 	$(CG56T)/CGCXSynchComm.o $(CG56T)/CG56XCSynchComm.o \
		$(CG56T)/CGCXAsynchComm.o $(CG56T)/CG56XCAsynchComm.o \
		$(CG56T)/CGCXBase.o  $(CG56T)/CGCS56XTarget.o
CG96TARGETS =	$(CG96T)/Sim96Target.o
CGCcm5TARGETS =	$(CGCT)/cm5/CGCcm5Send.o $(CGCT)/cm5/CGCcm5Recv.o \
		$(CGCT)/cm5/CGCcm5Target.o $(CGCT)/cm5/CGCcm5peTarget.o

#
# Architecture dependent definitions.
#

# PN domain and Awesime supported on sun4 and mips architectures.
ifneq (,$(filter sun% mips,$(ARCH)))
PNSTARS = $(LIBDIR)/pnstars.o
PN_LIBS= -lpnstars -lpn
PN_LIBFILES= $(LIBDIR)/libpnstars.$(LIBSUFFIX) $(LIBDIR)/libpn.$(LIBSUFFIX)
AWE_LIBS= -lawethread -lawe2
AWE_LIBFILES= $(LIBDIR)/libawethread.$(LIBSUFFIX) \
	$(LIBDIR)/libawe2.$(LIBSUFFIX)
endif

# CP domain and lwp supported on sun4 architectures.
ifneq (,$(filter sun% cfront,$(ARCH)))
# -laudio is for the Infopad stars in the CP domain
CPSTARS = $(LIBDIR)/cpstars.o $(LIBDIR)/cpipstars.o
CP_LIBS= -lcpstars -lcpipstars -lcp -laudio
CP_LIBFILES= $(LIBDIR)/libcpstars.$(LIBSUFFIX) \
	$(LIBDIR)/libcpipstars.$(LIBSUFFIX) $(LIBDIR)/libcp.$(LIBSUFFIX)
LWP_LIBS= -llwpthread -llwp
LWP_LIBFILES= $(LIBDIR)/liblwpthread.$(LIBSUFFIX)
endif
