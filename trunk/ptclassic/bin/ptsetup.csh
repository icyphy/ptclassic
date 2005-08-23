#! /bin/csh
# ptsetup.csh
# Setup common environment variables used by pigi, ptcl and tycho
# 
# Version: @(#)ptsetup.csh	1.22 01/05/99
# Copyright (c) 1996-1997 The Regents of the University of California.
# 	All Rights Reserved.
#
# Author: Christopher Hylands

# This script sets commonly used environment variables.
# It expects $PTOLEMY to be set before being called.
# Current, we don't check the return value of this script, so this would
# be a poor place to do error checking.

# Find out the name that we are being called as
# Check if $0 is set so we can source this from the csh prompt for testing.
if ( $?0 ) then
	set progname = `basename $0`
else
	set progname = ptsetup.csh
endif

set ptarch=`$PTOLEMY/bin/ptarch`
if ( ! $?PTARCH ) then
    setenv PTARCH $ptarch
else 
    # check that the basic architecture is set correctly
    if ( "$PTARCH" !~ "$ptarch"* ) then
	echo ${progname}: Warning: \$PTARCH == $PTARCH,
	echo "        "but \$PTOLEMY/bin/ptarch returns ${ptarch}.
    endif
    # check that a makefile for this $PTARCH exists
    if ( ! -e "$PTOLEMY/mk/config-${PTARCH}.mk" ) then
	echo ${progname}: Warning: \$PTARCH == $PTARCH,
	echo "        "but there is no makefile for this architecture.
    endif
endif
unset ptarch
if ( ! $?USER ) then
    setenv USER $LOGNAME
endif

if ( ! $?TYCHO ) then
	if ( $?PTOLEMY ) then
		setenv TYCHO "$PTOLEMY/tycho"
	else
		setenv TYCHO ~ptolemy/tycho
		if ( -d "$TYCHO" ) then
			echo 'Neither $TYCHO or $PTOLEMY set, so we are setting TYCHO to ~ptolemy/tycho'
		else
		    echo 'You must set $TYCHO or $PTOLEMY for tycho to work'
		    echo " See tycho/doc/running.html for further information"
		endif
	endif
endif

# Used by cg56/targets/CGCXBase.pl
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

setenv TMPLD_LIBRARY_PATH $PTOLEMY/lib.${PTARCH}:$PTOLEMY/octtools/lib.${PTARCH}:$PTOLEMY/gnu/${PTARCH}/lib:$PTOLEMY/tcltk/tcl.${PTARCH}/lib

# Matlab settings
set matlabdir = `$PTOLEMY/bin/matlabRootDir`
if (-x "$matlabdir/bin/util/arch.sh") then
	set matlabarch = `echo 'echo $Arch' | cat $matlabdir/bin/util/arch.sh - | /bin/sh`
	set matlablibdir = "$matlabdir/extern/lib/$matlabarch"
	setenv TMPLD_LIBRARY_PATH ${TMPLD_LIBRARY_PATH}:${matlablibdir}
endif

if ($PTARCH =~ hppa*) then
	if ( ! $?SHLIB_PATH ) then
		setenv SHLIB_PATH $TMPLD_LIBRARY_PATH
	else
		setenv SHLIB_PATH ${TMPLD_LIBRARY_PATH}:${SHLIB_PATH}
	endif
else
	if ( ! $?LD_LIBRARY_PATH ) then
		setenv LD_LIBRARY_PATH $TMPLD_LIBRARY_PATH
	else
		setenv LD_LIBRARY_PATH ${TMPLD_LIBRARY_PATH}:${LD_LIBRARY_PATH}

	endif
endif
unsetenv TMPLD_LIBRARY_PATH

if ( -f "$PTOLEMY/tcltk/tcl/lib/tcl/init.tcl" ) then
	# Itcl3.0
	setenv TCL_LIBRARY "$PTOLEMY/tcltk/tcl/lib/tcl"
	setenv TK_LIBRARY "$PTOLEMY/tcltk/tcl/lib/tk"
	setenv ITCL_LIBRARY "$PTOLEMY/tcltk/tcl/lib/itcl"
	setenv ITK_LIBRARY "$PTOLEMY/tcltk/tcl/lib/itk"
	setenv IWIDGETS_LIBRARY "$PTOLEMY/tcltk/tcl/lib/iwidgets"

else
	if ( -f "$PTOLEMY/tcltk/itcl/lib/itcl/itcl/itcl.tcl" ) then
		# Itcl2.1
		setenv TCL_LIBRARY "$PTOLEMY/tcltk/itcl/lib/itcl/tcl"
		setenv TK_LIBRARY "$PTOLEMY/tcltk/itcl/lib/itcl/tk"
		setenv ITCL_LIBRARY "$PTOLEMY/tcltk/itcl/lib/itcl/itcl"
		setenv ITK_LIBRARY "$PTOLEMY/tcltk/itcl/lib/itcl/itk"
		setenv IWIDGETS_LIBRARY "$PTOLEMY/tcltk/itcl/lib/itcl/iwidgets"
	else
		echo "${progname}: Warning: Can't find init.tcl"

	endif
endif

# Wrap this in quotes in case we are in a directory that has
# spaces in its name
setenv PTPWD "`pwd`"

# check if binary and script path is in $path
unset sp bp
foreach i ( $path )
	if ( "$PTOLEMY/bin.$PTARCH" == "$i" ) set bp
	if ( "$PTOLEMY/bin" == "$i" ) set sp
end 
# prepend Ptolemy binary path if not in $path
if ( ! $?bp ) set path = ( $PTOLEMY/bin.$PTARCH $path )
# prepend Ptolemy script path if not in $path
if ( ! $?sp ) set path = ( $PTOLEMY/bin $path )
unset sp bp i
