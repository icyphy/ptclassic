#
# stars.mk :: Common definitions of star and target subsets.
#
# Eventually this file will be automatically generated.
# But for now, we do it by hand.
# Also, for now we ignore the libraries that must be linked against.
# That should also be a function of the domains.
#
# Extracted from Joe's pigiRpc makefile by Kennard.
# Version: $Id$

# The following makefile symbols should be defined:
# ROOT		the root of the ptolemy tree (e.g., $PTOLEMY).  Typically
#		a relative path.
# OBJDIR	the root of the object file tree for this arch.
#		it can be either relative to current directory or absolute.
#

# The following symbols point to the directories containing the object
# files for the stars.  Defined in terms of OBJDIR, which is typically
# a relative (../..) path.  If you copy this makefile to make an
# alternate pigiRpc, replace the definitions by absolute paths.

SDF=$(OBJDIR)/domains/sdf/stars
SDF_DSP=$(OBJDIR)/domains/sdf/dsp/stars
SDF_IMAGE=$(OBJDIR)/domains/sdf/image/stars
DDF=$(OBJDIR)/domains/ddf/stars
DE =$(OBJDIR)/domains/de/stars
Thor=$(OBJDIR)/domains/thor/stars
CG=$(OBJDIR)/domains/cg/stars
CGDDF=$(OBJDIR)/domains/cg-ddf/stars
CG56=$(OBJDIR)/domains/cg56/stars
CG56DSP=$(OBJDIR)/domains/cg56/dsp/stars
CG96=$(OBJDIR)/domains/cg96/stars

# The following symbols point to the directories containing the source
# code for the stars of each domain.

SDF_SRC=$(ROOT)/src/domains/sdf/stars
SDF_DSP_SRC=$(ROOT)/src/domains/sdf/dsp/stars
SDF_IMAGE_SRC=$(ROOT)/src/domains/sdf/image/stars
DDF_SRC=$(ROOT)/src/domains/ddf/stars
DE_SRC =$(ROOT)/src/domains/de/stars
Thor_SRC=$(ROOT)/src/domains/thor/stars
CG_SRC=$(ROOT)/src/domains/cg/stars
CGDDF_SRC=$(ROOT)/src/domains/cg-ddf/stars
CG56_SRC=$(ROOT)/src/domains/cg56/stars
CG56DSP_SRC=$(ROOT)/src/domains/cg56/dsp/stars
CG96_SRC=$(ROOT)/src/domains/cg96/stars


# These include files define makefile symbols which list the
# the paths of all the stars in a particular star subset.
# Text in these included makefiles requires that certain symbols
# be defined already.  The comments on the right indicate the
# required symbol and the resulting star list symbol.

include $(SDF_SRC)/sdfstars.mk			# SDF -> SDFSTARS
include $(SDF_DSP_SRC)/sdfdspstars.mk		# SDF_DSP -> SDF_DSPSTARS
include $(SDF_IMAGE_SRC)/sdfimagestars.mk	# SDF_IMAGE -> SDF_IMAGESTARS
include $(DE_SRC)/destars.mk			# DE -> DESTARS
include $(DDF_SRC)/ddfstars.mk			# DDF -> DDFSTARS
include $(Thor_SRC)/thorstars.mk		# Thor -> ThorSTARS
include $(CG_SRC)/cgstars.mk			# CG -> CGSTARS
include $(CGDDF_SRC)/cgddfstars.mk		# CGDDF -> CGDDFSTARS
include $(CG56_SRC)/cg56stars.mk		# CG56 -> CG56STARS
include $(CG56DSP_SRC)/cg56dspstars.mk		# CG56DSP -> CG56STARS
include $(CG96_SRC)/cg96stars.mk		# CG96 -> CG96STARS

# Extra targets
SDFT = $(OBJDIR)/domains/sdf/targets
CGT = $(OBJDIR)/domains/cg/targets
CG56T = $(OBJDIR)/domains/cg56/targets
CG96T = $(OBJDIR)/domains/cg96/targets

CGTARGETS =	$(OBJDIR)/domains/sdf/loopScheduler/LoopTarget.o \
		$(CGT)/CGMultiTarget.o $(CGT)/CGSharedBus.o \
		$(SDFT)/CompileTarget.o 
CG56TARGETS =	$(CG56T)/Sim56Target.o $(CG56T)/S56XTarget.o \
		$(CG56T)/Sub56Target.o
CG96TARGETS =	$(CG96T)/Sim96Target.o

