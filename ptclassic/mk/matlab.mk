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
# matlab.mk :: Common definitions for the Ptolemy interface to Matlab
# Version: $Id$
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
# At compile time, either
# (a) sdfnomatlabstars.o and libsdfnomatlabstars.a are created if
#     Matlab is not installed, or
# (b) sdfmatlabstars.o and libsdfmatlabstars.a are created if
#     Matlab is installed
#
# At link time, either
# (a) libsdfnomatlabstars.a is linked if Matlab is not installed, or
# (b) libmat.a is linked if Matlab is installed.
#
# If Matlab is installed, the old way to determine the Matlab architecture is
# MATARCH := $(shell $(ROOT)/bin/matlabArch $(ARCH))
# Now, MATARCH is set by the config makefiles.
#
MATLABDIR := $(shell $(ROOT)/bin/matlabRootDir)
ifeq ("$(MATLABDIR)","")
MATLABDIR= 		$(ROOT)/src/compat/matlab
MATLABSTAR_LIBFILE=	$(LIBDIR)/libsdfnomatlabstars.$(LIBSUFFIX) 
MATLABSTAR_LIB=		-lsdfnomatlabstars
MATLABSTARS_DOT_O=	$(LIBDIR)/sdfnomatlabstars.o
else
MATLABEXT_LIB = 	-L$(MATLABDIR)/extern/lib/$(MATARCH) -lmat
MATLABSTAR_LIBFILE=	$(LIBDIR)/libsdfmatlabstars.$(LIBSUFFIX) 
MATLABSTAR_LIB=		-lsdfmatlabstars
MATLABSTARS_DOT_O=	$(LIBDIR)/sdfmatlabstars.o
endif

