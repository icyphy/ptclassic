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
SDFSTARS = $(LIBDIR)/sdfstars.o $(LIBDIR)/sdfimagestars.o \
 	   $(LIBDIR)/sdfdspstars.o $(LIBDIR)/sdfmatrixstars.o
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
CPSTARS = $(LIBDIR)/cpstars.o $(LIBDIR)/cpipstars.o
ATMSTARS = $(LIBDIR)/mqstars.o $(LIBDIR)/deatmstars.o $(LIBDIR)/sdfatmstars.o
MDSDFSTARS = $(LIBDIR)/mdsdfstars.o

# parallel scheduler libraries.
PARLIBFILES = $(LIBDIR)/libDC.a $(LIBDIR)/libHu.a  $(LIBDIR)/libDL.a \
	$(LIBDIR)/libPar.a $(LIBDIR)/libcgstars.a $(LIBDIR)/libcg.a

# if S56DIR is defined compile in the S56X Wormhole target.  This
# is defined in config-sun4.mk.
ifdef S56DIR
S56WH_LIB= -lqckMon
S56WH_LIBDIR= -L$(S56DIR)/lib
S56WH_O= $(CG56T)/S56XTargetWH.o 
endif

# if MATLABDIR is defined, then compile with Matlab external interface library
ifdef MATLABDIR
MATLABEXT_LIB= -lmat
MATLABEXT_LIBDIR= -L$(MATLABDIR)/extern/lib/$(ARCH)
endif

# Library files reqd by stars.  Note that libptolemy.a is not included.
STAR_LIBFILES=\
$(LIBDIR)/libcgcstars.a $(LIBDIR)/libcgctcltk.a $(LIBDIR)/libcgc.a \
$(LIBDIR)/libcg96dspstars.a \
$(LIBDIR)/libcg96stars.a $(LIBDIR)/libcg96.a \
$(LIBDIR)/libcg56dspstars.a \
$(LIBDIR)/libcg56stars.a $(LIBDIR)/libcg56.a \
$(LIBDIR)/libsilagestars.a $(LIBDIR)/libsilage.a \
$(LIBDIR)/libcgstars.a $(PARLIBFILES) $(LIBDIR)/libcg.a \
$(LIBDIR)/libddfstars.a $(LIBDIR)/libddf.a \
$(LIBDIR)/libthorstars.a $(LIBDIR)/libthor.a \
$(LIBDIR)/libdestars.a $(LIBDIR)/libde.a \
$(LIBDIR)/libsdfimagestars.a $(LIBDIR)/libImage.a \
$(LIBDIR)/libsdfdspstars.a $(LIBDIR)/libsdfmatrixstars.a \
$(LIBDIR)/libbdfstars.a $(LIBDIR)/libbdf.a \
$(LIBDIR)/libsdfstars.a $(LIBDIR)/libLS.a $(LIBDIR)/libsdf.a \
$(LIBDIR)/libvhdlfstars.a $(LIBDIR)/libvhdlf.a \
$(LIBDIR)/libvhdlbstars.a $(LIBDIR)/libvhdlb.a \
$(LIBDIR)/libmdsdfstars.a $(LIBDIR)/libmdsdf.a

CP_LIBFILES= $(LIBDIR)/libcpstars.a $(LIBDIR)/libcpipstars.a \
	$(LIBDIR)/libcp.a
LWP_LIBFILES= $(LIBDIR)/liblwpthread.a

ATM_LIBFILES = $(LIBDIR)/libmq.a $(LIBDIR)/libmqstars.a \
	$(LIBDIR)/libdeatmstars.a $(LIBDIR)/libsdfatmstars.a \
	$(LIBDIR)/libatm.a


# CG-DDF no longer supported
#$(LIBDIR)/libcgddfstars.a $(LIBDIR)/libcgddf.a \

# Library switches reqd by stars.  Note that -lptolemy is not included.
STAR_LIBS=\
-lcgcstars -lcgc -lcgctcltk \
-lcg96dspstars -lcg96stars -lcg96 \
-lcg56dspstars -lcg56stars -lcg56 \
$(S56WH_LIBDIR) $(S56WH_LIB) \
-lsilagestars -lsilage \
-lcgstars -lDC -lHu -lDL -lPar -lcg \
$(MATLABEXT_LIBDIR) $(MATLABEXT_LIB) \
-lddfstars -lddf \
-lthorstars -lthor \
-ldestars -lde \
-lbdfstars -lbdf \
-lsdfimagestars -lImage -lsdfdspstars -lsdfstars -lLS -lsdf -lsdfmatrixstars \
-lvhdlfstars -lvhdlf \
-lvhdlbstars -lvhdlb \
-lmdsdfstars -lmdsdf

# -laudio is for the Infopad stars in the CP domain
CP_LIBS= -lcpstars -lcpipstars -lcp -laudio
LWP_LIBS= -llwpthread -llwp

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
		$(CG56T)/CG56XCSReceive.o $(CG56T)/CG56XCSSend.o \
		$(CG56T)/CG56XCAReceive.o $(CG56T)/CG56XCASend.o \
		$(CG56T)/CGCXSReceive.o $(CG56T)/CGCXSSend.o \
		$(CG56T)/CGCXAReceive.o $(CG56T)/CGCXASend.o \
	 	$(CG56T)/CGCXSBase.o $(CG56T)/CG56XCSBase.o \
		$(CG56T)/CGCXABase.o $(CG56T)/CG56XCABase.o \
		$(CG56T)/CGCXBase.o  $(CG56T)/CGCS56XTarget.o
CG96TARGETS =	$(CG96T)/Sim96Target.o
CGCcm5TARGETS =	$(CGCT)/cm5/CGCcm5Send.o $(CGCT)/cm5/CGCcm5Recv.o \
		$(CGCT)/cm5/CGCcm5Target.o $(CGCT)/cm5/CGCcm5peTarget.o
