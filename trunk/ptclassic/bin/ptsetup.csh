#! /bin/csh
# ptsetup.csh
# Setup common environment variables used by pigi, ptcl and tycho
# 
# Version: $Id$
# Copyright (c) %Q% The Regents of the University of California.
# 	All Rights Reserved.
#
# Author: Christopher Hylands

# This script sets commonly used environment variables.
# It expects $PTOLEMY to be set before being called.
# Current, we don't check the return value of this script, so this would
# be a poor place to do error checking.

# Find out the name that we are being called as
set progname = `basename $0`

if ( ! $?PTARCH ) then
    setenv PTARCH `$PTOLEMY/bin/ptarch`
endif
if ( ! $?USER ) then
    setenv USER $LOGNAME
endif

if ( ! $?PTX11DIR ) then
    switch ($PTARCH)
	case sol2:
	case sol2.cfront:
	    setenv PTX11DIR /usr/openwin
	    breaksw
	case *:
	    setenv PTX11DIR /usr/X11
	    breaksw
    endsw
endif

if ( ! $?QCKMON ) then
    switch ($PTARCH)
	case sol2:
	    setenv QCKMON qckMon5
	    breaksw
	case sun4:
            setenv QCKMON qckMon
	    breaksw
    endsw
endif

if ( ! $?S56DSP ) then
	setenv S56DSP /users/ptdesign/vendors/s56dsp
endif

if ($PTARCH =~ hppa?*) then
	if ( ! $?SHLIB_PATH ) then
		setenv SHLIB_PATH $PTOLEMY/lib.${PTARCH}:$PTOLEMY/octtools/lib.${PTARCH}:$PTOLEMY/gnu/$PTARCH/lib
	endif
else
	if ( ! $?LD_LIBRARY_PATH ) then
		# Save the LD_LIBRARY_PATH in case we are called with -bak
		setenv S_LD_LIBRARY_PATH yes
		setenv LD_LIBRARY_PATH $PTOLEMY/lib.${PTARCH}:$PTOLEMY/octtools/lib.${PTARCH}:$PTOLEMY/gnu/$PTARCH/lib:${PTX11DIR}/lib
	endif
endif


if ( -f $PTOLEMY/tcltk/itcl/lib/itcl/itcl/itcl.tcl ) then
	# Itcl2.1
	setenv TCL_LIBRARY $PTOLEMY/tcltk/itcl/lib/itcl/tcl
	setenv TK_LIBRARY $PTOLEMY/tcltk/itcl/lib/itcl/tk
	setenv ITCL_LIBRARY $PTOLEMY/tcltk/itcl/lib/itcl/itcl
	setenv ITK_LIBRARY $PTOLEMY/tcltk/itcl/lib/itcl/itk
	setenv IWIDGETS_LIBRARY $PTOLEMY/tcltk/itcl/lib/itcl/iwidgets
else
	# Itcl2.0, which was shipped with Ptolemy0.6
	setenv TCL_LIBRARY $PTOLEMY/tcltk/tcl/lib/tcl
	setenv TK_LIBRARY $PTOLEMY/tcltk/tk/lib/tk
	setenv ITCL_LIBRARY $PTOLEMY/tcltk/itcl/lib/itcl
	setenv ITK_LIBRARY $PTOLEMY/tcltk/itcl/lib/itk
	setenv IWIDGETS_LIBRARY $PTOLEMY/tcltk/itcl/lib/iwidgets
endif

setenv PTPWD `pwd`

set path = ( $PTOLEMY/bin.$PTARCH $PTOLEMY/bin $path )

