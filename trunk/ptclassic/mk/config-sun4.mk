# $Id$

include $(ROOT)/config-g++.mk

# If the X11 include directories are in /usr/include/X11, leave
# the following symbol blank.  Otherwise define it as "-Idir" where
# dir is the PARENT of the include directory, which must end in X11.
X11INCL = -I/usr/X11/include

# If the X11 libraries are not on the standard library search path,
# define the following symbol as "-Ldir" where dir is the directory
# containing them.  Otherwise leave it blank.
X11LIBDIR = -L/usr/X11/lib

TCL_ROOT=$(ROOT)/tcl
# Directory containing Tcl/Tk include files
TCL_H_DIR=$(TCL_ROOT)/include

# Directory containing Tcl/Tk libraries
TCL_LIBDIR=$(TCL_ROOT)/lib.$(ARCH)

S56DIR= $(ROOT)/vendors/s56dsp
