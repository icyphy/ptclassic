#! /bin/sh
# ptsetup.sh
# Setup common environment variables used by pigi, ptcl and tycho
# 
# Version: $Id$
# Copyright (c) 1996-%Q% The Regents of the University of California.
# 	All Rights Reserved.
#
# Author: Christopher Hylands, based on code by Wolfgang Reimer

# This script sets commonly used environment variables.
# It expects $PTOLEMY to be set before being called.
# Current, we don't check the return value of this script, so this would
# be a poor place to do error checking.

# Find out the name that we are being called as
# Check if $0 is set so we can source this from the csh prompt for testing.
if [ $# = 0 ]; then
    progname=`basename $0`
else
    progname=ptsetup.sh
fi

ptarch=`$PTOLEMY/bin/ptarch`
if [ -z "$PTARCH" ]; then
    PTARCH=$ptarch
    export PTARCH
else 
    # check that the basic architecture is set correctly
    case $PTARCH in
	$ptarch*);;
	*)
	    echo ${progname}: Warning: \$PTARCH == $PTARCH,
	    echo "        "but \$PTOLEMY/bin/ptarch returns ${ptarch}.
	    ;;
    esac
    # check that a makefile for this $PTARCH exists
    if [ ! -r "$PTOLEMY/mk/config-$PTARCH.mk" ]; then
	echo "${progname}: Warning: \$PTARCH == $PTARCH,"
	echo "        but there is no makefile for this architecture."
    fi
fi
unset ptarch

if [ -z "$USER" ]; then
    USER=$LOGNAME
    export USER
fi

# We attempt to set the PTOLEMY variable so we can use  
# it if TYCHO is not set.
if [ -z "$PTOLEMY" ]; then 
    if [ "$TYCHO" -a -f "$TYCHO/../bin/ptarch" ]; then
	PTOLEMY="$TYCHO/.."
	export PTOLEMY
    elif [ -d "$PTDEFAULT" ]; then
	PTOLEMY="$PTDEFAULT"
	export PTOLEMY
    fi
fi

if [ -z "$TYCHO" ]; then
    if [ "$PTOLEMY" -a -f "$PTOLEMY/tycho/bin/ptarch" ]; then
	TYCHO="$PTOLEMY/tycho"
	export TYCHO
    elif [ -d "$TYDEFAULT" ]; then
	TYCHO="$TYDEFAULT"
	export TYCHO
    fi
    export TYCHO
fi

# Check for success.
if [ ! -d "$TYCHO" ]; then
    echo "\$TYCHO == $TYCHO, which was not found" 
    echo 'You must set $TYCHO or $PTOLEMY for tycho to work'
    echo " See tycho/doc/running.html for further information"
fi


# Used by cg56/targets/CGCXBase.pl
if [ -z "$QCKMON" ]; then
    case $PTARCH in
	sol2) QCKMON=qckMon5
	    export QCKMON;;
	sun4) QCKMON=qckMon
	    export QCKMON;;
    esac
fi

if [ -z "$S56DSP" ]; then
    S56DSP=/users/ptdesign/vendors/s56dsp
    export S56DSP
fi

TMPLD_LIBRARY_PATH=$PTOLEMY/lib.${PTARCH}:$PTOLEMY/octtools/lib.${PTARCH}:$PTOLEMY/gnu/${PTARCH}/lib:$PTOLEMY/tcltk/itcl.${PTARCH}/lib/itcl

# Matlab settings
matlabdir=`$PTOLEMY/bin/matlabRootDir`
if [ -x "$matlabdir/bin/util/arch.sh" ]; then
    matlabarch=`echo 'echo $Arch' | cat $matlabdir/bin/util/arch.sh - | /bin/sh`
    matlablibdir="$matlabdir/extern/lib/$matlabarch"
    TMPLD_LIBRARY_PATH=${TMPLD_LIBRARY_PATH}:${matlablibdir}
fi

case $PTARCH in
    hppa*)
	if [ -z "$SHLIB_PATH" ]; then
	    SHLIB_PATH=$TMPLD_LIBRARY_PATH
	    export SHLIB_PATH
	else
	    SHLIB_PATH=${TMPLD_LIBRARY_PATH}:${SHLIB_PATH}
	    export SHLIB_PATH
	fi;;
    *)	
	if [ -z "$LD_LIBRARY_PATH" ]; then
	    LD_LIBRARY_PATH=$TMPLD_LIBRARY_PATH
	    export LD_LIBRARY_PATH
	else
	    LD_LIBRARY_PATH=${TMPLD_LIBRARY_PATH}:${LD_LIBRARY_PATH}
	    export LD_LIBRARY_PATH
	fi;;
esac

unset TMPLD_LIBRARY_PATH

TCL_LIBRARY="$PTOLEMY/tcltk/itcl/lib/itcl/tcl"
TK_LIBRARY="$PTOLEMY/tcltk/itcl/lib/itcl/tk"
ITCL_LIBRARY="$PTOLEMY/tcltk/itcl/lib/itcl/itcl"
ITK_LIBRARY="$PTOLEMY/tcltk/itcl/lib/itcl/itk"
IWIDGETS_LIBRARY="$PTOLEMY/tcltk/itcl/lib/itcl/iwidgets"
export TCL_LIBRARY TK_LIBRARY ITCL_LIBRARY ITK_LIBRARY IWIDGETS_LIBRARY

# Wrap this in quotes in case we are in a directory that has
# spaces in its name
PTPWD="`pwd`"
export PTPWD

# check if binary and script path is in $path
bp=1
sp=1
oldifs="$IFS"; IFS=":"
for i in $PATH
do
    if [ "$PTOLEMY/bin.$PTARCH" = "$i" ]; then
	bp=1
    fi 	
    if [ "$PTOLEMY/bin" = "$i" ]; then
	sp=1
    fi	
done	
IFS="$oldifs"

# prepend Ptolemy binary path if not in $path
if [ $bp = 0 ]; then
    PATH=$PTOLEMY/bin.$PTARCH:$PATH
    export PATH
fi
# prepend Ptolemy script path if not in $path
if [ $sp = 0 ]; then
    PATH=$PTOLEMY/bin:$PATH
    export PATH
fi
unset sp bp i
