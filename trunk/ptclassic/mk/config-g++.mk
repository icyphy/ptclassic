# Config to use for g++
# This is not a complete config; it only overrides those options
# specific to using g++
#

# $Id$
# Copyright (c) 1991 The Regents of the University of California.
#                       All Rights Reserved.
#		       
# Programmer:  J. T. Buck

#
# Programs to use
CPLUSPLUS = g++
# If we have g++, then compile Octtools with gcc.  ARCHs that are cfront
# based probably don't have gcc.
# OCT_CC is used in src/octtools/vem-{lib,bin}.mk
OCT_CC =	gcc -fwritable-strings

# Compiler flags
# -Wsynth is new in g++-2.6.x
GPPFLAGS = -g -Wall -Wcast-qual -Wsynth $(MEMLOG)
CFLAGS = -g
DEPEND= $(CPLUSPLUS) -MM

# Command to build shared libraries (Not really supported yet)
SHARED_LIBRARY_COMMAND =	g++ -shared -o

# where the Gnu library is
GNULIB=$(PTOLEMY)/gnu/$(ARCH)/lib

# linker to use for pigi and interpreter.
LINKER=g++
# startup module
CRT0=
# system libraries (libraries from the environment)
SYSLIBS=-lg++ -lm
# link flags (tell linker to strip out debug symbols)
# -static prevents use of shared libraries when building ptolemy
# shared libraries interfere with incremental linking of stars.
# -Xlinker specifies that the next argument should be passed verbatim to
# the linker
# -x requests that only global symbols be left in the executable.
# -S requests that the executable be stripped.
# It would see that these two conflict, but using just -x with the gnu
# linker results in a huge executable.
LINKFLAGS=-L$(LIBDIR) -Xlinker -S -Xlinker -x -static
# link flags if debugging symbols are to be left
LINKFLAGS_D=-L$(LIBDIR) -static

# Since we are compiling octtools with gcc, we don't want the 
# -lmm library linked in
OCTTOOLS_MM_LIB=
