# Config to use for g++
# This is not a complete config; it only overrides those options
# specific to using g++
#

# $Id$
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
# Under gcc-2.7.0, you will need to add -fno-for-scope to GPPFLAGS
# Under gxx-2.7.0 -Wcast-qual will drown you with warnings from libg++ includes
GPPFLAGS = -g -Wall -Wcast-qual -Wsynth $(MEMLOG)
CFLAGS = -g
# Itcl-2.0 need -fwritable-strings, or tclsh will segv in ItclFollowNamespPath
WRITABLE_STRINGS_CFLAGS = -fwritable-strings

# $PTOLEMY/src/domains/ipus/kernel uses this.
NO_IMPLICIT_TEMPLATES = -fno-implicit-templates

DEPEND= $(CPLUSPLUS) -MM

# where the Gnu library is
GNULIB=$(PTOLEMY)/gnu/$(PTARCH)/lib

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

# Used by cgwork.mk
INC_LINK_FLAGS =	-fPIC
