# Copyright (c) 1990-1999 The Regents of the University of California.
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
# matlab.mk :: Common definitions for the Ptolemy interface to Matlab
# Version: @(#)matlab.mk	1.18 09/16/99
#
# We have to handle four cases:
# (1/2) Matlab is/is not installed at compile time
# (3/4) Matlab is/is not installed at link time
#
# Matlab is installed if the matlabRootDir script returns an non-empty string
# -- If Matlab is not installed, set MATLABDIR to $(ROOT)/src/compat/matlab
# -- If Matlab is installed, then set MATLABDIR accordingly
#

# The Matlab include files are located in $(MATLABDIR)/extern/include
# At compile time, sdfmatlabstars.o and libsdfmatlabstars.so are
# created.
#
# For Matlab with no shared libs (4.2), Ptolemy is linked against
# libexttools.so, libptmatlab.so, and the matlab libs.
#
# For Matlab with shared libs (5.0), Ptolemy is linked against
# libexttools.so only.  libptmatlab is linked against the Matlab
# shared libraries, and Ptolemy will try to load it at runtime.
#
# Ptolemy is linked with MATLABEXT_LIB, and libptmatlab is linked
# with MATLABEXT2_LIB.
#
# If Matlab is installed, the old way to determine the Matlab architecture is
# MATARCH := $(shell $(ROOT)/bin/matlabArch $(PTARCH))
# Now, MATARCH is set by the config makefiles.

# matlabRootDir traverses the user's path, so we only run it when
# we really need it.
ifdef NEED_MATLABDIR
	# config-default.mk sets INCLUDE_MATLAB to no
	ifeq ($(INCLUDE_MATLAB),no)
		MATLABDIR= 		$(ROOT)/src/compat/matlab
	else
		ifndef MATLABDIR
			MATLABDIR := $(shell $(ROOT)/bin/matlabRootDir)
		endif
		ifeq ("$(MATLABDIR)","")
		MATLABDIR= 		$(ROOT)/src/compat/matlab
		else
		MATLABEXTERN = $(MATLABDIR)/extern/lib/$(MATARCH)
		ifeq ($(wildcard $(MATLABEXTERN)/libeng.*),)
			# Matlab 4.2
			MATLABEXT_LIB = -L$(MATLABEXTERN) -lptmatlab -lmat
		else
			# Matlab 5.0
			MATLABEXT2_LIB = $(MATLABEXTRAOPTS) -L$(MATLABEXTERN) -leng -lmat -lmx
		endif
		endif
	endif
	MATLAB_INCSPEC =	-I$(MATLABDIR)/extern/include
endif

# Ptolemy interface directories
EXTTOOLSLIB = $(ROOT)/src/utils/libexttools
PTMATLABLIB = $(ROOT)/src/utils/libptmatlab
MATLABIFC_INCSPEC = -I$(EXTTOOLSLIB) -I$(PTMATLABLIB)

