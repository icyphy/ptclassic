# Makefile to test out incremental linking of new stars
# Version identification:
# $Id$
# Copyright (c) %Q% The Regents of the University of California.
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
# Programmer: Christopher Hylands
# Date of creation: 5/24/96

ROOT = $(PTOLEMY)
VPATH = ../../../src/tst/incLink
include $(ROOT)/mk/config-$(PTARCH).mk
INCL =  -I$(VPATH) \
	-I$(ROOT)/src/domains/sdf/stars \
	-I$(ROOT)/src/domains/sdf/kernel \
	-I$(KERNDIR)
PL_SRCS = SDFMyRamp.pl SDFMyRampDerived.pl SDFMyRampSysDerived.pl
DOMAIN = SDF

EXTRA_SRCS = incLink-tst.tcl
# Files to be removed
KRUFT =	myrun10


include ../makefile.tst
test:  
	# We copy the files 
	rm -f ./SDFMyRamp.pl ./SDFMyRampDerived.pl ./SDFMyRampSysDerived.pl
	rm -f ./SDFMyRamp-dist.pl
	cp $(VPATH)/SDFMyRamp.pl .
	cp $(VPATH)/SDFMyRampDerived.pl . 
	cp $(VPATH)/SDFMyRampSysDerived.pl .
	cp $(VPATH)/SDFMyRamp-dist.pl .
	chmod u+w ./SDFMyRamp.pl ./SDFMyRampDerived.pl ./SDFMyRampSysDerived.pl
	chmod u+w ./SDFMyRamp-dist.pl
	echo "source $(VPATH)/incLink-tst.tcl" | $(PTCL)

all.pt updateresults:
	echo "Nothing needs to be done for $@ in this directory"
include $(ROOT)/mk/userstars.mk

SDFMyRamp.o: SDFMyRamp.cc SDFMyRamp.h \
 /users/ptdesign/src/domains/sdf/kernel/SDFStar.h \
 /users/ptdesign/src/kernel/DataFlowStar.h \
 /users/ptdesign/src/kernel/Star.h /users/ptdesign/src/kernel/Block.h \
 /users/ptdesign/src/kernel/NamedObj.h \
 /users/ptdesign/src/kernel/StringList.h \
 /users/ptdesign/src/kernel/miscFuncs.h \
 /users/ptdesign/src/kernel/logNew.h \
 /users/ptdesign/src/kernel/DataStruct.h \
 /users/ptdesign/src/kernel/type.h /users/ptdesign/src/kernel/isa.h \
 /users/ptdesign/src/kernel/PortHole.h \
 /users/ptdesign/src/kernel/dataType.h \
 /users/ptdesign/src/kernel/Particle.h \
 /users/ptdesign/src/kernel/Fix.h \
 /users/ptdesign/src/kernel/ComplexSubset.h \
 /users/ptdesign/src/kernel/Attribute.h \
 /users/ptdesign/src/kernel/State.h \
 /users/ptdesign/src/kernel/InvokeInterp.h \
 /users/ptdesign/src/kernel/Error.h \
 /users/ptdesign/src/kernel/FlagArray.h \
 /users/ptdesign/src/kernel/Fraction.h \
 /users/ptdesign/src/domains/sdf/kernel/SDFPortHole.h \
 /users/ptdesign/src/kernel/DFPortHole.h \
 /users/ptdesign/src/kernel/FloatState.h
SDFMyRampDerived.o: SDFMyRampDerived.cc SDFMyRampDerived.h SDFMyRamp.h \
 /users/ptdesign/src/domains/sdf/kernel/SDFStar.h \
 /users/ptdesign/src/kernel/DataFlowStar.h \
 /users/ptdesign/src/kernel/Star.h /users/ptdesign/src/kernel/Block.h \
 /users/ptdesign/src/kernel/NamedObj.h \
 /users/ptdesign/src/kernel/StringList.h \
 /users/ptdesign/src/kernel/miscFuncs.h \
 /users/ptdesign/src/kernel/logNew.h \
 /users/ptdesign/src/kernel/DataStruct.h \
 /users/ptdesign/src/kernel/type.h /users/ptdesign/src/kernel/isa.h \
 /users/ptdesign/src/kernel/PortHole.h \
 /users/ptdesign/src/kernel/dataType.h \
 /users/ptdesign/src/kernel/Particle.h \
 /users/ptdesign/src/kernel/Fix.h \
 /users/ptdesign/src/kernel/ComplexSubset.h \
 /users/ptdesign/src/kernel/Attribute.h \
 /users/ptdesign/src/kernel/State.h \
 /users/ptdesign/src/kernel/InvokeInterp.h \
 /users/ptdesign/src/kernel/Error.h \
 /users/ptdesign/src/kernel/FlagArray.h \
 /users/ptdesign/src/kernel/Fraction.h \
 /users/ptdesign/src/domains/sdf/kernel/SDFPortHole.h \
 /users/ptdesign/src/kernel/DFPortHole.h \
 /users/ptdesign/src/kernel/FloatState.h
SDFMyRampSysDerived.o: SDFMyRampSysDerived.cc SDFMyRampSysDerived.h \
 /users/ptdesign/src/domains/sdf/stars/SDFRamp.h \
 /users/ptdesign/src/domains/sdf/kernel/SDFStar.h \
 /users/ptdesign/src/kernel/DataFlowStar.h \
 /users/ptdesign/src/kernel/Star.h /users/ptdesign/src/kernel/Block.h \
 /users/ptdesign/src/kernel/NamedObj.h \
 /users/ptdesign/src/kernel/StringList.h \
 /users/ptdesign/src/kernel/miscFuncs.h \
 /users/ptdesign/src/kernel/logNew.h \
 /users/ptdesign/src/kernel/DataStruct.h \
 /users/ptdesign/src/kernel/type.h /users/ptdesign/src/kernel/isa.h \
 /users/ptdesign/src/kernel/PortHole.h \
 /users/ptdesign/src/kernel/dataType.h \
 /users/ptdesign/src/kernel/Particle.h \
 /users/ptdesign/src/kernel/Fix.h \
 /users/ptdesign/src/kernel/ComplexSubset.h \
 /users/ptdesign/src/kernel/Attribute.h \
 /users/ptdesign/src/kernel/State.h \
 /users/ptdesign/src/kernel/InvokeInterp.h \
 /users/ptdesign/src/kernel/Error.h \
 /users/ptdesign/src/kernel/FlagArray.h \
 /users/ptdesign/src/kernel/Fraction.h \
 /users/ptdesign/src/domains/sdf/kernel/SDFPortHole.h \
 /users/ptdesign/src/kernel/DFPortHole.h \
 /users/ptdesign/src/kernel/FloatState.h
