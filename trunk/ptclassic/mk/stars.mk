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
PTINY_SDFSTARS = $(LIBDIR)/sdfstars.o $(LIBDIR)/sdfdspstars.o \
	$(LIBDIR)/sdfmatrixstars.o $(MATLABSTARS_DOT_O)
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
MDSDFSTARS = $(LIBDIR)/mdsdfstars.o
CPSTARS = $(LIBDIR)/cpstars.o $(LIBDIR)/cpipstars.o
PNSTARS = $(LIBDIR)/pnstars.o
IPUSSTARS = $(LIBDIR)/ipusstars.o

# Optional star libraries
ATMSTARS = $(LIBDIR)/mqstars.o $(LIBDIR)/deatmstars.o $(LIBDIR)/sdfatmstars.o
ATM_LIBFILES = $(LIBDIR)/libmq.$(LIBSUFFIX) $(LIBDIR)/libmqstars.$(LIBSUFFIX) \
	$(LIBDIR)/libdeatmstars.$(LIBSUFFIX) \
	$(LIBDIR)/libsdfatmstars.$(LIBSUFFIX) $(LIBDIR)/libatm.$(LIBSUFFIX)
ATM_LIBS= -lmqstars -lmq -ldeatmstars -lsdfatmstars -latm
CONTRIBSTARS = $(LIBDIR)/sdfcontribstars.o
CONTRIB_LIBFILES = $(LIBDIR)/libsdfcontribstars.$(LIBSUFFIX)
CONTRIB_LIBS = -lsdfcontribstars
IPUS_STAR_LIBFILES = $(LIBDIR)/libipusstars.$(LIBSUFFIX) \
			$(LIBDIR)/libipus.$(LIBSUFFIX) \
			$(LIBDIR)/libicp.$(LIBSUFFIX)
OPTIONAL_STARS = $(ATMSTARS) $(CONTRIBSTARS)
OPTIONAL_LIBFILES = $(ATM_LIBFILES) $(CONTRIB_LIBFILES)
OPTIONAL_LIBS = $(ATM_LIBS) $(CONTRIB_LIBS)

# parallel scheduler libraries.
PARLIBFILES = $(LIBDIR)/libDC.$(LIBSUFFIX) $(LIBDIR)/libHu.$(LIBSUFFIX) \
	$(LIBDIR)/libMacro.$(LIBSUFFIX) $(LIBDIR)/libcgddf.$(LIBSUFFIX) \
	$(LIBDIR)/libDL.$(LIBSUFFIX) $(LIBDIR)/libMSH.$(LIBSUFFIX) \
	$(LIBDIR)/libPar.$(LIBSUFFIX) \
	$(LIBDIR)/libcgstars.$(LIBSUFFIX) $(LIBDIR)/libcg.$(LIBSUFFIX)

# Library files reqd by stars.
# Note that libptolemy.$(LIBSUFFIX) is not included.
PTINY_STAR_LIBFILES=\
$(LIBDIR)/libdestars.$(LIBSUFFIX) $(LIBDIR)/libde.$(LIBSUFFIX) \
$(LIBDIR)/libsdfdspstars.$(LIBSUFFIX) \
$(LIBDIR)/libsdfmatrixstars.$(LIBSUFFIX) \
$(MATLABSTAR_LIBFILE) \
$(LIBDIR)/libsdfstars.$(LIBSUFFIX) $(LIBDIR)/libLS.$(LIBSUFFIX) \
$(LIBDIR)/libsdf.$(LIBSUFFIX) \

PTRIM_STAR_LIBFILES=\
$(LIBDIR)/libcgcstars.$(LIBSUFFIX) $(LIBDIR)/libcgctcltk.$(LIBSUFFIX) \
$(LIBDIR)/libcgc.$(LIBSUFFIX) \
$(LIBDIR)/libcgstars.$(LIBSUFFIX) $(LIBDIR)/libcg.$(LIBSUFFIX) \
$(LIBDIR)/libddfstars.$(LIBSUFFIX) $(LIBDIR)/libddf.$(LIBSUFFIX) \
$(LIBDIR)/libdestars.$(LIBSUFFIX) $(LIBDIR)/libde.$(LIBSUFFIX) \
$(LIBDIR)/libsdfimagestars.$(LIBSUFFIX) $(LIBDIR)/libImage.$(LIBSUFFIX) \
$(LIBDIR)/libsdfdspstars.$(LIBSUFFIX) \
$(LIBDIR)/libsdfmatrixstars.$(LIBSUFFIX) \
$(MATLABSTAR_LIBFILE) \
$(LIBDIR)/libbdfstars.$(LIBSUFFIX) $(LIBDIR)/libbdf.$(LIBSUFFIX) \
$(LIBDIR)/libsdfstars.$(LIBSUFFIX) $(LIBDIR)/libLS.$(LIBSUFFIX) \
$(LIBDIR)/libsdf.$(LIBSUFFIX) \

STAR_LIBFILES=\
$(OPTIONAL_LIBFILES) \
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
$(IPUS_STAR_LIBFILES) \
$(THREAD_STAR_LIBFILES) \
$(MATLABSTARS_LIBFILE)

# HOF stars can be used in pigiRpc but not ptcl.
# The HOF base classes call Tk_DoOneEvent so that if you accidentally
# specify an infinite recursion (easy to do with HOF), you can hit the
# STOP button and abort the run.  But Tk is not linked into ptcl, so
# this call cannot be done.
HOFSTARS =	$(LIBDIR)/hofstars.o
HOF_LIBS =	-lhofstars -lhof
HOF_LIBFILES =	$(LIBDIR)/libhofstars.$(LIBSUFFIX) \
		$(LIBDIR)/libhof.$(LIBSUFFIX)

# Tcl/Tk stars can be used in pigiRpc but not ptcl.
PTINY_TCLSTARS = $(LIBDIR)/sdftclstars.o $(LIBDIR)/detclstars.o
PTRIM_TCLSTARS = $(PTINY_TCLSTARS)
IPUS_TCLSTARS =	$(LIBDIR)/ipustclstars.o
TCLSTARS =	$(PTINY_TCLSTARS) $(LIBDIR)/mdsdftclstars.o \
		$(IPUS_TCLSTARS)

PTINY_TCL_STAR_LIBS = -lsdftclstars -ldetclstars
PTRIM_TCL_STAR_LIBS = $(PTINY_TCL_STAR_LIBS)
IPUS_TCL_STAR_LIBS = -lipustclstars -lipustcltk
TCL_STAR_LIBS =	$(PTINY_TCL_STAR_LIBS) -lmdsdftclstars $(IPUS_TCL_STAR_LIBS)

PTINY_TCL_STAR_LIBFILES =	$(LIBDIR)/libsdftclstars.$(LIBSUFFIX) \
				$(LIBDIR)/libdetclstars.$(LIBSUFFIX)
PTRIM_TCL_STAR_LIBFILES =	$(PTINY_TCL_STAR_LIBFILES)
IPUS_TCL_STAR_LIBFILES =	$(LIBDIR)/libipustclstars.$(LIBSUFFIX) \
				$(LIBDIR)/libipustcltk.$(LIBSUFFIX)
TCL_STAR_LIBFILES =	$(PTINY_TCL_STAR_LIBFILES) \
			$(LIBDIR)/libmdsdftclstars.$(LIBSUFFIX) \
			$(IPUS_TCL_STAR_LIBFILES)

# Matlab settings
include $(ROOT)/mk/matlab.mk

# CG-DDF no longer supported
#$(LIBDIR)/libcgddfstars.$(LIBSUFFIX) $(LIBDIR)/libcgddf.$(LIBSUFFIX)  \

# Library switches reqd by stars for a ptiny ptolemy. Note that we don't
# include the sdf image stars.  Need -lImage for the de ATM stars
PTINY_STAR_LIBS=\
-ldestars -lde \
-lImage -lsdfdspstars -lsdfstars -lLS -lsdf -lsdfmatrixstars \
$(MATLABSTAR_LIB) $(MATLABEXT_LIB)

PTRIM_STAR_LIBS=\
-lcgcstars -lcgc -lcgctcltk \
-lcgstars -lcg \
-lddfstars -lddf \
-ldestars -lde \
-lbdfstars -lbdf \
-lsdfimagestars -lImage -lsdfdspstars -lsdfstars -lLS -lsdf -lsdfmatrixstars \
$(MATLABSTAR_LIB) $(MATLABEXT_LIB)

IPUS_STAR_LIBS = -lipusstars -lipus -licp
# Library switches reqd by stars.  Note that -lptolemy is not included.
STAR_LIBS=\
$(OPTIONAL_LIBS) \
-lcgcstars -lcgc -lcgctcltk \
-lcg96dspstars -lcg96stars -lcg96 \
-lcg56dspstars -lcg56stars -lcg56 \
-lsilagestars -lsilage \
-lcgstars -lcgddf -lMacro -lDC -lHu -lDL -lMSH -lPar -lcg \
-lddfstars -lddf \
-lthorstars -lthor \
-ldestars -lde \
-lbdfstars -lbdf \
-lsdfimagestars -lImage -lsdfdspstars -lsdfstars -lLS -lsdf -lsdfmatrixstars \
$(MATLABSTAR_LIB) $(MATLABEXT_LIB) \
-lvhdlfstars -lvhdlf \
-lvhdlbstars -lvhdlb \
-lmdsdfstars -lmdsdf \
$(IPUS_STAR_LIBS) \
$(THREAD_STAR_LIBS)

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
ifeq ($(USE_SHARED_LIBS),yes) 
CGCTARGETS =	$(CGCTCL)/CGCTclTkTarget.o
CGCTARGETS_LIBS = 	-lcgctargets
CGCTARGETS_LIBSFILES = $(LIBDIR)/libcgctargets.$(LIBSUFFIX)
else
CGCTARGETS =	$(CGCT)/main/CGCUnixSend.o $(CGCT)/main/CGCUnixReceive.o \
		$(CGCT)/main/CGCMultiTarget.o $(CGCTCL)/CGCTclTkTarget.o \
		$(CGCT)/main/CGCMacroStar.o $(CGCT)/main/CGCDDFCode.o \
		$(CGCT)/main/CGCSDFSend.o $(CGCT)/main/CGCSDFReceive.o \
		$(CGCT)/main/CGCSDFBase.o $(CGCT)/main/CGCTargetWH.o
CGCTARGETS_LIBS =
CGCTARGETS_LIBSFILES =
endif

CGCDDFTARGETS =	$(CGCT)/main/CGCDDFTarget.o
BDFTARGETS =	$(CGT)/CGBDFTarget.o $(CGCT)/main/CGCBDFTarget.o

ifeq ($(USE_SHARED_LIBS),yes) 
CG56TARGETS =
CG56TARGETS_LIBS = 	-lcg56targets
CG56TARGETS_LIBSFILES = $(LIBDIR)/libcg56targets.$(LIBSUFFIX)
else
CG56TARGETS =	$(CG56T)/Sim56Target.o $(CG56T)/S56XTarget.o \
		$(CG56T)/S56XTargetWH.o \
		$(CG56T)/Sub56Target.o $(CG56T)/CG56MultiSimTarget.o \
		$(CG56T)/CG56MultiSimSend.o $(CG56T)/CG56MultiSimReceive.o \
		$(CG56T)/CG56XCReceive.o $(CG56T)/CG56XCSend.o \
		$(CG56T)/CG56XCPeek.o $(CG56T)/CG56XCPoke.o \
		$(CG56T)/CGCXReceive.o $(CG56T)/CGCXSend.o \
		$(CG56T)/CGCXPeek.o $(CG56T)/CGCXPoke.o \
	 	$(CG56T)/CGCXSynchComm.o $(CG56T)/CG56XCSynchComm.o \
		$(CG56T)/CGCXAsynchComm.o $(CG56T)/CG56XCAsynchComm.o \
		$(CG56T)/CGCXBase.o  $(CG56T)/CGCS56XTarget.o
CG56TARGETS_LIBS =
CG56TARGETS_LIBSFILES =
endif

CG96TARGETS =	$(CG96T)/Sim96Target.o
CGCcm5TARGETS =	$(CGCT)/cm5/CGCcm5Send.o $(CGCT)/cm5/CGCcm5Recv.o \
		$(CGCT)/cm5/CGCcm5Target.o $(CGCT)/cm5/CGCcm5peTarget.o

# If we are not using shared libraries, then this will be empty
TARGETS_LIBS = $(CG56TARGETS_LIBS) $(CGCTARGETS_LIBS)

#
# Thread related definitions.
#

# PN domain.
ifeq ($(INCLUDE_PN_DOMAIN),yes) 
PN_LIBS =	-lpnstars -lpn
PN_LIBFILES =	$(LIBDIR)/libpnstars.$(LIBSUFFIX) $(LIBDIR)/libpn.$(LIBSUFFIX)
else
PN_LIBS =
PN_LIBFILES =
endif

# CP domain.
# -laudio is for the Infopad stars in the CP domain
CP_LIBS =	-lcpstars -lcpipstars -lcp -laudio
CP_LIBFILES =	$(LIBDIR)/libcpstars.$(LIBSUFFIX) \
		$(LIBDIR)/libcpipstars.$(LIBSUFFIX) \
		$(LIBDIR)/libcp.$(LIBSUFFIX)

# Sun's Lightweight Process library.
LWP_LIBS =		-llwpthread -llwp
LWP_LIBFILES =		$(LIBDIR)/liblwpthread.$(LIBSUFFIX)

# POSIX thread library from Florida State University.
PTHREAD_LIBS =		-lposixthread -L$(ROOT)/thread/lib.$(PTARCH) -lgthreads
PTHREAD_LIBFILES =	$(LIBDIR)/libposixthread.$(LIBSUFFIX)

#
# Architecture dependent definitions.
#

# Use POSIX threads for SunOS and Solaris.
ifneq (,$(filter sun% sol%,$(PTARCH)))
THREAD_STARS =		$(PNSTARS)
THREAD_STAR_LIBS =	$(PN_LIBS)
THREAD_STAR_LIBFILES =	$(PN_LIBFILES)
THREAD_LIBS =		$(PTHREAD_LIBS)
THREAD_LIBFILES =	$(PTHREAD_LIBFILES)
endif

# sol2.cfront can't build ipus because of template problems
ifeq ($(INCLUDE_IPUS_DOMAIN),no) 
IPUSSTARS =
IPUS_STAR_LIBFILES =
IPUS_TCLSTARS =
IPUS_TCL_STAR_LIBS =
IPUS_TCL_STAR_LIBFILES =
IPUS_STAR_LIBS =
endif

