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
# Programmer:  Brian L. Evans
#
# mathematica.mk: Common definitions for the Ptolemy interface to Mathematica
# $Id$
#
# We have to handle four cases:
# (1/2) Mathematica is/is not installed at compile time
# (3/4) Mathematica is/is not installed at link time
#
# Mathematica is installed if the mathRootDir script returns a non-empty string
# -- If Mathematica is not installed, set MATHEMATICADIR to
#    $(ROOT)/src/compat/mathematica
# -- If Mathematica is installed, then set MATHEMATICADIR accordingly
#
# The Mathematica include files are in $(MATHEMATICADIR)/Source/Includes.
# At link time, Ptolemy is linked against libexttools.so (MathematicaIfc class)
# and either
# (a) libptmathematica.so if Mathematica is not installed, or
# (b) libMLelf.a if Mathematica is installed.
#

# Set the name of the Mathematica library to libML.a if it's not set
ifndef MATHLINKLIBNAME
	MATHLINKLIBNAME = ML
endif

# MATHEMATICAARCH is set in the config makefiles.

# matlabRootDir traverses the user's path, so we only run it when
# we really need it.
ifdef NEED_MATHEMATICADIR
	MATHEMATICADIR := $(shell $(ROOT)/bin/mathRootDir)
	MATHEMATICA_INCSPEC =	-I$(MATHEMATICADIR)/Source/Includes
	ifeq ("$(MATHEMATICADIR)","")
	MATHEMATICADIR= 	$(ROOT)/src/compat/mathematica
	MATHEMATICAEXT_LIB = 	-lptmathematica
	else
	ifeq ($(wildcard $(MATHEMATICADIR)/Bin/MathLink),)
		# Mathematica3.x
		MATHEMATICAEXT_LIB = 	-L$(MATHEMATICADIR)/AddOns/MathLink/DevelopersKits/$(MATHEMATICAARCH)/CompilerAdditions \
					-l$(MATHLINKLIBNAME)

		MATHEMATICA_INCSPEC =	-I$(MATHEMATICADIR)/AddOns/MathLink/DevelopersKits/$(MATHEMATICAARCH)/CompilerAdditions
	else
		# Mathematica2.x
		MATHEMATICAEXT_LIB = 	-L$(MATHEMATICADIR)/Bin/MathLink \
					-l$(MATHLINKLIBNAME)
	endif
	endif
endif

# Ptolemy interface directories
EXTTOOLSLIB = $(ROOT)/src/utils/libexttools
PTMATHEMATICALIB = $(ROOT)/src/utils/libptmathematica
MATHEMATICAIFC_INCSPEC = -I$(EXTTOOLSLIB) -I$(PTMATHEMATICALIB)

