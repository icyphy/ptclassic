# @Copyright (c) 1998-%Q% The Regents of the University of California.
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
# Makefile for Ptolemy ACS stars
# @(#)make.template	1.13 09/08/99
#
# If this file's name is "make.template" and there is no makefile in
# this directory, do
#	cp make.template makefile
#	chmod +w makefile
#	make depend
# This will add the dependencies to the makefile.

ROOT = ../../../..
VPATH = $(ROOT)/src/domains/acs/stars

# get configuration info
CONFIG=$(ROOT)/mk/config-$(PTARCH).mk
include $(CONFIG)

# ACSDIR is the directory containing ACS domain support
ACSDIR = 	$(ROOT)/src/domains/acs/kernel
ACSTARGET =     $(ROOT)/src/domains/acs/targets
CGKERNEL = 	$(ROOT)/src/domains/cg/kernel
SDFKERNEL =     $(ROOT)/src/domains/sdf/kernel
INCL = 		-I$(ACSDIR) -I$(CGKERNEL) -I$(SDFKERNEL) -I$(KERNDIR) -I$(ACSTARGET)

# Library name
PTLIB=libacsstars.$(LIBSUFFIX)

# source files for all stars, using preprocessor
# base classes/stars go in the front of the list
PL_SRCS= \
  ACS16FFT.pl ACS16FFTCGFPGA.pl ACS16FFTFixSim.pl \
  ACSAbs.pl ACSAbsCGFPGA.pl ACSAbsFPCGC.pl ACSAbsFPSim.pl ACSAbsFixSim.pl \
  ACSAdd.pl ACSAddCGFPGA.pl ACSAddFPCGC.pl ACSAddFPSim.pl ACSAddFixSim.pl \
  ACSBuffer.pl ACSBufferCGFPGA.pl \
  ACSCompare.pl ACSCompareCGFPGA.pl ACSCompareFPCGC.pl ACSCompareFPSim.pl ACSCompareFixSim.pl \
  ACSConst.pl ACSConstCGFPGA.pl ACSConstFPCGC.pl ACSConstFPSim.pl ACSConstFixSim.pl \
  ACSCos.pl ACSCosCGFPGA.pl ACSCosFPCGC.pl ACSCosFPSim.pl ACSCosFixSim.pl \
  ACSDelay.pl ACSDelayCGFPGA.pl ACSDelayFPCGC.pl ACSDelayFPSim.pl ACSDelayFixSim.pl \
  ACSExp.pl ACSExpCGFPGA.pl ACSExpFPCGC.pl ACSExpFPSim.pl ACSExpFixSim.pl \
  ACSFIR.pl ACSFIRCGFPGA.pl ACSFIRFPCGC.pl ACSFIRFPSim.pl ACSFIRFixSim.pl \
  ACSFork.pl ACSForkCGFPGA.pl ACSForkFPCGC.pl ACSForkFPSim.pl ACSForkFixSim.pl \
  ACSGain.pl ACSGainFPCGC.pl ACSGainFPSim.pl ACSGainFixSim.pl \
  ACSIDCT.pl ACSIDCTFPCGC.pl ACSIDCTFPSim.pl ACSIDCTFixSim.pl \
  ACSImpulse.pl ACSImpulseCGFPGA.pl ACSImpulseFPCGC.pl ACSImpulseFPSim.pl ACSImpulseFixSim.pl \
  ACSIntegrator.pl ACSIntegratorCGFPGA.pl ACSIntegratorFPCGC.pl ACSIntegratorFPSim.pl ACSIntegratorFixSim.pl \
  ACSIntGain.pl ACSIntGainCGFPGA.pl ACSIntGainFPCGC.pl ACSIntGainFPSim.pl ACSIntGainFixSim.pl \
  ACSIoPort.pl ACSIoPortCGFPGA.pl \
  ACSMpy.pl ACSMpyCGFPGA.pl ACSMpyFPCGC.pl ACSMpyFPSim.pl ACSMpyFixSim.pl \
  ACSMux.pl ACSMuxCGFPGA.pl ACSMuxFPCGC.pl ACSMuxFPSim.pl ACSMuxFixSim.pl \
  ACSNL.pl ACSNLFPCGC.pl ACSNLFPSim.pl ACSNLFixSim.pl \
  ACSNonBiMem.pl ACSNonBiMemCGFPGA.pl \
  ACSNop.pl ACSNopFPCGC.pl ACSNopFPSim.pl ACSNopFixSim.pl \
  ACSPackBits.pl ACSPackBitsCGFPGA.pl ACSPackBitsFixSim.pl \
  ACSPolarToRect.pl ACSPolarToRectFPCGC.pl ACSPolarToRectFPSim.pl ACSPolarToRectFixSim.pl \
  ACSPrinter.pl ACSPrinterCGFPGA.pl ACSPrinterFPCGC.pl ACSPrinterFPSim.pl ACSPrinterFixSim.pl \
  ACSRamDelay.pl ACSRamDelayCGFPGA.pl ACSRamDelayFPCGC.pl ACSRamDelayFPSim.pl ACSRamDelayFixSim.pl \
  ACSRamp.pl ACSRampCGFPGA.pl ACSRampFPCGC.pl ACSRampFPSim.pl ACSRampFixSim.pl \
  ACSShift.pl ACSShiftCGFPGA.pl ACSShiftFPSim.pl ACSShiftFixSim.pl \
  ACSSin.pl ACSSinCGFPGA.pl ACSSinFPCGC.pl ACSSinFPSim.pl ACSSinFixSim.pl \
  ACSSink.pl ACSSinkCGFPGA.pl ACSSinkFPCGC.pl ACSSinkFPSim.pl ACSSinkFixSim.pl \
  ACSSource.pl ACSSourceCGFPGA.pl ACSSourceFPCGC.pl ACSSourceFPSim.pl ACSSourceFixSim.pl \
  ACSSqrt.pl ACSSqrtCGFPGA.pl ACSSqrtFPCGC.pl ACSSqrtFPSim.pl ACSSqrtFixSim.pl \
  ACSSub.pl ACSSubCGFPGA.pl ACSSubFPCGC.pl ACSSubFPSim.pl ACSSubFixSim.pl \
  ACSUniSeq.pl ACSUniSeqCGFPGA.pl \
  ACSUnPackBits.pl ACSUnPackBitsCGFPGA.pl ACSUnPackBitsFixSim.pl \
  ACSXMGraph.pl ACSXMGraphCGFPGA.pl ACSXMGraphFPCGC.pl ACSXMGraphFPSim.pl ACSXMGraphFixSim.pl \
  ACSXYgraph.pl ACSXYgraphCGFPGA.pl ACSXYgraphFPCGC.pl ACSXYgraphFPSim.pl ACSXYgraphFixSim.pl \
  ACSRamDelayFixSim.pl \
  ACSRamp.pl ACSRampCGFPGA.pl ACSRampFPCGC.pl ACSRampFPSim.pl ACSRampFixSim.pl \
  ACSShift.pl ACSShiftCGFPGA.pl ACSShiftFPSim.pl ACSShiftFixSim.pl \
  ACSSin.pl ACSSinCGFPGA.pl ACSSinFPCGC.pl ACSSinFPSim.pl ACSSinFixSim.pl \
  ACSSink.pl ACSSinkCGFPGA.pl ACSSinkFPCGC.pl ACSSinkFPSim.pl ACSSinkFixSim.pl \
  ACSSource.pl ACSSourceCGFPGA.pl ACSSourceFPCGC.pl ACSSourceFPSim.pl ACSSourceFixSim.pl \
  ACSSqrt.pl ACSSqrtCGFPGA.pl ACSSqrtFPCGC.pl ACSSqrtFPSim.pl ACSSqrtFixSim.pl \
  ACSSub.pl ACSSubCGFPGA.pl ACSSubFPCGC.pl ACSSubFPSim.pl ACSSubFixSim.pl \
  ACSUniSeq.pl ACSUniSeqCGFPGA.pl \
  ACSXMGraph.pl ACSXMGraphCGFPGA.pl ACSXMGraphFPCGC.pl ACSXMGraphFPSim.pl ACSXMGraphFixSim.pl \
  ACSXYgraph.pl ACSXYgraphCGFPGA.pl ACSXYgraphFPCGC.pl ACSXYgraphFPSim.pl ACSXYgraphFixSim.pl

# The .cc files
SRCS=$(PL_SRCS:.pl=.cc)

# object files for all stars
OBJS= $(SRCS:.cc=.o)

# header files for all stars
HDRS= $(SRCS:.cc=.h)

STAR_MK=acsstars
DOMAIN=ACS
EXTRA_SRCS=$(STAR_MK).c $(PL_SRCS)

all:		makefile stars_all

install:	stars_install

# include common definitions, rules
include $(ROOT)/mk/common.mk


# Don't add anything after the next line; makedepend will zap it.
# DO NOT DELETE THIS LINE -- make depend depends on it.

