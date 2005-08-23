# Makefile for ptvhdlsim, a interface between Synopsys and Ptolemy
# Version identification:
# @(#)make.template	1.10 01/29/97
# Copyright (c) 1990-1997 The Regents of the University of California.
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

# ptvhdlsim is a socket based interface between Synopsys and Ptolemy
# To use this tool you must have Synopsys installed locally.  You will
# need to do something like:
# setenv SYNOPSYS /usr/tools/synopsys
# setenv SIM_ARCH sparcOS5
# setenv ARCH $PTARCH
# set path = (	$SYNOPSYS/$SIM_ARCH/syn/bin $SYNOPSYS/$SIM_ARCH/sim/bin $path)
# Note that vhdlan seems to require that $ARCH is set.

ROOT = ../../..
SYNCOMPAT = $(ROOT)/src/compat/synopsys
VPATH = $(ROOT)/src/utils/ptvhdlsim
BINDIR = $(ROOT)/bin.$(PTARCH)

# get configuration info
CONFIG=$(ROOT)/mk/config-$(PTARCH).mk
include $(CONFIG)

EXTRA_SRCS = CLISocket.c CLISocket.h \
	V2Cinteger.c C2Vinteger.c V2Creal.c C2Vreal.c \
	V2Cinteger.vhdl C2Vinteger.vhdl V2Creal.vhdl C2Vreal.vhdl \
	synopsys_vss.setup

OBJS = 	V2Cinteger.o C2Vinteger.o V2Creal.o C2Vreal.o

# common.mk wants this, otherwise we get multiple targets for $(LIBDIR)/$(PTLIB)
PTLIB = foo

# .cli* files seem to be some sort of cache for the cli program
KRUFT = 	.cli* *.mra *.sim *.a ptvhdlsim vhdlsim \
		v2cinteger c2vinteger v2creal c2vreal


# If the SYNOPSYS shell environment variable is defined, then build
ifdef SYNOPSYS
all: makefile ptvhdlsim
install: ptvhdlsim $(BINDIR)/ptvhdlsim
else
all install: makefile
	@echo "SYNOPSYS is not present (\$$SYNOPSYS not defined)"
	@echo "	so there is no need to build ptvhdlsim"
endif


# Commonly used routines in a central place
CLISocket.o: CLISocket.c CLISocket.h
	$(CC) $< -c -I$(SYNOPSYS)/$(SIM_ARCH)/sim/lib \
	-I$(SYNCOMPAT)


# cli is too stupid to handle VPATH, so we make local links
v2cinteger: V2Cinteger.c ./CLISocket.h
	@if [ ! -f $(notdir $<) ]; then echo "ln -s $< ."; ln -s $< .; fi
	@if [ ! -f CLISocket.h ]; then ln -s $(VPATH)/CLISocket.h .; fi
	cli -nc -add -cflags "-I$(SYNCOMPAT)" \
		-cvl V2Cinteger V2Cinteger PTVHDLSIM
	touch v2cinteger
c2vinteger: C2Vinteger.c ./CLISocket.h
	@if [ ! -f $(notdir $<) ]; then echo "ln -s $< ."; ln -s $< .; fi
	@if [ ! -f CLISocket.h ]; then ln -s $(VPATH)/CLISocket.h .; fi
	cli -nc -add -cflags "-I$(SYNCOMPAT)" \
		-cvl C2Vinteger C2Vinteger PTVHDLSIM
	touch c2vinteger
v2creal: V2Creal.c ./CLISocket.h
	@if [ ! -f $(notdir $<) ]; then echo "ln -s $< ."; ln -s $< .; fi
	@if [ ! -f CLISocket.h ]; then ln -s $(VPATH)/CLISocket.h .; fi
	cli -nc -add -cflags "-I$(SYNCOMPAT)" \
		-cvl V2Creal V2Creal PTVHDLSIM
	touch v2creal
c2vreal: C2Vreal.c ./CLISocket.h
	@if [ ! -f $(notdir $<) ]; then echo "ln -s $< ."; ln -s $< .; fi
	@if [ ! -f CLISocket.h ]; then ln -s $(VPATH)/CLISocket.h .; fi
	cli -nc -add -cflags "-I$(SYNCOMPAT)" \
		-cvl C2Vreal C2Vreal PTVHDLSIM
	touch c2vreal

setup: synopsys_vss.setup
	@if [ ! -f $(notdir $<) ]; then echo "ln -s $< ."; ln -s $< .; fi
	rm -f .synopsys_vss.setup
	ln -s $< .synopsys_vss.setup

C2VINTEGER.sim: C2Vinteger.vhdl
	@if [ ! -f $(notdir $<) ]; then echo "ln -s $< ."; ln -s $< .; fi
	vhdlan -nc -w PTVHDLSIM $<
V2CINTEGER.sim: V2Cinteger.vhdl
	@if [ ! -f $(notdir $<) ]; then echo "ln -s $< ."; ln -s $< .; fi
	vhdlan -nc -w PTVHDLSIM $<
C2VREAL.sim: C2Vreal.vhdl
	@if [ ! -f $(notdir $<) ]; then echo "ln -s $< ."; ln -s $< .; fi
	vhdlan -nc -w PTVHDLSIM $<
V2CREAL.sim: V2Creal.vhdl
	@if [ ! -f $(notdir $<) ]; then echo "ln -s $< ."; ln -s $< .; fi
	vhdlan -nc -w PTVHDLSIM $<

libsynsock.a: CLISocket.o
	ar cr libsynsock.a CLISocket.o
	$(RANLIB) libsynsock.a
	touch $(OBJS)

# Build the new simulator with our extra CLI modules linked in
vhdlsim: v2cinteger c2vinteger v2creal c2vreal setup \
		V2CINTEGER.sim C2VINTEGER.sim V2CREAL.sim C2VREAL.sim \
		CLISocket.o libsynsock.a
	cli -nc -s -build -ldflags "-L/opt/SUNWspro/SC2.0.1 -L./" \
		-libs "-lsynsock"

ptvhdlsim: vhdlsim
	rm -f $@
	ln $< $@

$(BINDIR)/ptvhdlsim: ptvhdlsim
	rm -f $@
	ln $< $@	

# include common definitions, rules
include $(ROOT)/mk/common.mk

# Don't add anything after the next line; makedepend will zap it.
# DO NOT DELETE THIS LINE -- make depend depends on it.


