#! /bin/sh
# ptsetup.sh
# Setup common environment variables used by pigi, ptcl and tycho
# 
# Version: @(#)ptsetup.sh	1.6 01/05/99
# Copyright (c) 1996-1998 The Regents of the University of California.
# 	All Rights Reserved.
#
# Author: Christopher Hylands, based on code by Wolfgang Reimer

# This script sets commonly used environment variables.
# It expects $PTOLEMY to be set before being called.
# Current, we don't check the return value of this script, so this would
# be a poor place to do error checking.

# Find out the name that we are being called as

prg=${prg:-"`basename \"$0\"`"}

# set OCTTOOLS
OCTTOOLS="${OCTTOOLS:-$PTOLEMY}"
export OCTTOOLS

# check/guess PTARCH
ptarch="`$PTOLEMY/bin/ptarch`"
if [ -z "${PTARCH-}" ]; then
    PTARCH="$ptarch"
    export PTARCH
else 
    # check that the basic architecture is set correctly
    case "$PTARCH" in
	$ptarch*);;
	*)
	    echo "$prg: Warning: \$PTARCH == '$PTARCH',"
	    echo "      but \$PTOLEMY/bin/ptarch returns '$ptarch'."
	    ;;
    esac
    # check that a makefile for this $PTARCH exists
    if [ ! -r "$PTOLEMY/mk/config-$PTARCH.mk" ]; then
	echo "$prg: Warning: \$PTARCH == '$PTARCH',"
	echo "      but there is no makefile for this architecture."
    fi
fi
unset ptarch

USER="${USER:-${LOGNAME:-ptolemy}}"
export USER

# check/guess TYCHO
if [ -z "${TYCHO-}" ]; then
    if [ -f "$PTOLEMY/tycho/bin/ptarch" ]; then
	TYCHO="$PTOLEMY/tycho"
	export TYCHO
    elif [ -d "${TYDEFAULT-}" ]; then
	TYCHO="$TYDEFAULT"
	export TYCHO
    fi
fi

# Check for success.
if [ ! -d "${TYCHO-}" ]; then
    echo "$prg: \$TYCHO == ${TYCHO-}, which was not found" 
    echo '      You must set $TYCHO or $PTOLEMY for tycho to work'
    echo "      See tycho/doc/running.html for further information"
fi

# Used by cg56/targets/CGCXBase.pl
case "$PTARCH" in
    sol2) QCKMON=${QCKMON:-qckMon5}
	export QCKMON;;
    sun4) QCKMON=${QCKMON:-qckMon}
	export QCKMON;;
esac

S56DSP=${S56DSP:-/users/ptdesign/vendors/s56dsp}
export S56DSP

# SOME USEFUL FUNCTIONS:
# Adds to the path variable named by $1 the components $2 if components $2 are
# not in $1 already. $3 must be "append" or "prepend" to indicate what to do.
addpath () {
    eval value=\"\${$1:-}\"
    oldifs="$IFS"; IFS=":"
    case "$3" in
	p*) tmp=""; for i in $2; do tmp="$i${tmp:+:}$tmp"; done;;
	*)  tmp="$2";;
    esac
    for i in $tmp; do
	case "$value" in
	    *:$i:*|*:$i|$i:*|$i);;
	    "") value="$i";;
	    *)  case "$3" in 
		    p*)  value="$i:$value";;
		    *)   value="$value:$i";;
		esac;;
	esac
    done
    IFS="$oldifs"
    eval $1=\"$value\"
    unset value oldifs i tmp
}

# appends to a path variable $1 the components $2 if not in path already.
append () { addpath "$1" "$2" append; }

# prepends to a path variable $1 the components $2 if not in path already.
prepend () { addpath "$1" "$2" prepend; }


# set search path for DLLs
TMPLD_LIB_PATH="$PTOLEMY/lib.$PTARCH:$PTOLEMY/octtools/lib.$PTARCH"

if [ -d "$PTOLEMY/gnu/$PTARCH/lib" ]; then
    TMPLD_LIB_PATH="$TMPLD_LIB_PATH:$PTOLEMY/gnu/$PTARCH/lib"
fi 

if [ -d "$PTOLEMY/tcltk/tcl.${PTARCH}/lib" ]; then
    TMPLD_LIB_PATH="$TMPLD_LIB_PATH:$PTOLEMY/tcltk/tcl.${PTARCH}/lib"
fi 

# Matlab settings
matlabdir=`$PTOLEMY/bin/matlabRootDir`
if [ -x "$matlabdir/bin/util/arch.sh" ]; then
    matlabarch=`. "$matlabdir/bin/util/arch.sh"; echo "$Arch"`
    TMPLD_LIB_PATH="$TMPLD_LIB_PATH:$matlabdir/extern/lib/$matlabarch"
    unset matlabarch
fi
unset matlabdir

case "$PTARCH" in
    hppa*)
	prepend SHLIB_PATH "$TMPLD_LIB_PATH"
	export SHLIB_PATH;;
    *)	
	prepend LD_LIBRARY_PATH "$TMPLD_LIB_PATH"
	export LD_LIBRARY_PATH;;
esac

unset TMPLD_LIB_PATH

TCL_LIBRARY="$PTOLEMY/tcltk/tcl/lib/tcl"
TK_LIBRARY="$PTOLEMY/tcltk/tcl/lib/tk"
ITCL_LIBRARY="$PTOLEMY/tcltk/tcl/lib/itcl"
ITK_LIBRARY="$PTOLEMY/tcltk/tcl/lib/itk"
IWIDGETS_LIBRARY="$PTOLEMY/tcltk/tcl/lib/iwidgets"
export TCL_LIBRARY TK_LIBRARY ITCL_LIBRARY ITK_LIBRARY IWIDGETS_LIBRARY

# Wrap this in quotes in case we are in a directory that has
# spaces in its name
PTPWD="`pwd`"
export PTPWD

# prepend Ptolemy script path and binary path if not in $PATH
prepend PATH "$PTOLEMY/bin:$PTOLEMY/bin.$PTARCH"
export PATH

