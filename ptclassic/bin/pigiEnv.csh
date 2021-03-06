#! /bin/csh -f
# usage: pigi [-bw] [-cp] [-debug] [-console] [-help] [-ptiny] [-ptrim]
#	[-rpc rpcname] [-xres resname ] [cell_name]

# pigi - Ptolemy Interactive Graphics Interface
# This version uses Vem version 8-1
# 
# This is a modified version to support a PTOLEMY environment variable.
#
# Version: $Id$
# Copyright (c) 1990-%Q% The Regents of the University of California.
# 	All Rights Reserved.
#

# On interrupt, remove temporary files
onintr catch

if ( ! $?PTOLEMY ) setenv PTOLEMY ~ptolemy

# Set the commonly used environment variables
source $PTOLEMY/bin/ptsetup.csh

if ( ! $?OCTTOOLS ) setenv OCTTOOLS $PTOLEMY

if ( ! $?DISPLAY ) then
    echo "${0}: Your DISPLAY environment variable must be set or use the"
    echo "    -display option"
    exit 1
endif


# On DEC Alpha, do not print unaligned access message (vem)
switch ($PTARCH)
        case alpha:
        case alpha4:
                uac p noprint
                breaksw
        case *:
                breaksw
endsw

# If the user has set PIGIRPC, check to see if it exists
# We don't try and do anything smart if the user has set $PIGIRPC
# and it does not exist.
if ( $?PIGIRPC ) then
	if ( ! -x $PIGIRPC) then
		echo \$PIGIRPC = $PIGIRPC, which does not exist, exiting.
		exit 3
	endif
endif

set cell = init.pal
set resfile = pigiXRes9

while ($#argv)
	switch ($argv[1])
		case -help:
			echo "usage: pigi [-bw] [-cp] [-debug] [-console] [-help]"
			echo "  [-ptiny] [-ptrim] [-rpc rpcname] [-xres resname ] [-display display] [cell_name]"
			exit 0
		case -rpc:
			setenv PIGIRPC $argv[2]
			shift
			breaksw
		case -xres:
			setenv PIGIXRES $argv[2]
			shift
			breaksw
		case -debug:
			set pigidebug
			breaksw
		case -display:
			setenv DISPLAY $argv[2]
			shift
			breaksw
		case -console:
			echo "-console is obsolete and no longer supported."
			echo " To bring up a Tycho console that has similar"
			echo " functionality, start pigi and type a 'y' while"
			echo " the mouse is over a facet." 
			set pigiconsole
			breaksw
		case -bw:
			set resfile = pigiXRes9.bw
			breaksw
		case -cp:
			set resfile = pigiXRes9.cp
			breaksw
		case -ptiny:
			setenv PIGIBASE pigiRpc.ptiny
			breaksw
		case -ptrim:
			setenv PIGIBASE pigiRpc.ptrim
			breaksw
		case -*:
			echo Bad option: $argv[1]
			Echo "usage: $progname [-bw] [-cp] [-debug] [-console] [-help]"
			echo "  [-ptiny] [-ptrim] [-rpc rpcname] [-xres resname ] [-display display] [cell_name]"
			exit 1
			breaksw
		case *:
			set cell=$1
			if ( `basename $cell` == "" ) then
				echo "Warning: pigi can't handle names with" \
					"trailing slashes"
				set \
				 cell=`echo $cell | awk '{print substr($0,1,length($0)-1)}'`
				echo "Removed the trailing /, so now the" \
					"pathname is $cell"	
			endif
			breaksw
	endsw
	shift
end


# Try and do some smart error recovery if the pigiRpc binary can't be found.
# Rules if we can't find a binary:
# If we can't find $PIGIRPC, try looking in obj.$PTARCH/pigiRpc
# If we are running this script as ptiny or ptrim, or as pigi -ptiny or
# pigi -ptrim try looking for ptinyRpc or ptrimRpc.
# If we are running with -debug, try looking for .debug images

if ( ! $?PIGIBASE ) then
     switch ($progname)
	case ptiny:  
		setenv PIGIBASE pigiRpc.ptiny
		breaksw
	case ptrim:
		setenv PIGIBASE pigiRpc.ptrim
		breaksw
	case *:
		setenv PIGIBASE pigiRpc
		breaksw
     endsw
endif

# If the user has set $PIGIRPC and is calling -debug, don't try and be 
# smart here
if ($?pigidebug && ! $?PIGIRPC ) then
	if ( ! $?PIGIRPC ) then
	     setenv PIGIRPC $PTOLEMY/bin.$PTARCH/$PIGIBASE
	endif
	if ($?pigidebug && ! -x $PIGIRPC.debug ) then
		echo "$PIGIRPC.debug does not exist or is not executable"	
		if ( -x $PTOLEMY/obj.$PTARCH/pigiRpc/${PIGIBASE}.debug ) then
			setenv PIGIRPC $PTOLEMY/obj.$PTARCH/pigiRpc/${PIGIBASE}.debug
			echo "Using $PIGIRPC instead"
		else
			if ( -x $PIGIRPC ) then
				echo "Using $PIGIRPC instead."
			endif
		endif
	else
		if ($?pigidebug ) then
			setenv PIGIRPC $PIGIRPC.debug
		endif
	endif
endif


if ( ! $?PIGIRPC ) then
     setenv PIGIRPC $PTOLEMY/bin.$PTARCH/$PIGIBASE
endif

if ( ! -x $PIGIRPC ) then
	echo "$PIGIRPC does not exist or is not executable."
	if ( $?pigidebug && -x $PTOLEMY/obj.$PTARCH/pigiRpc/${PIGIBASE}.debug ) then
		setenv PIGIRPC $PTOLEMY/obj.$PTARCH/pigiRpc/${PIGIBASE}.debug
		echo "Using $PIGIRPC instead"
	else
		if ( -x $PTOLEMY/obj.$PTARCH/pigiRpc/$PIGIBASE ) then
			setenv PIGIRPC $PTOLEMY/obj.$PTARCH/pigiRpc/$PIGIBASE
			echo "Using $PIGIRPC instead"
		else
			echo "${progname}: exiting"
			exit 4
		endif
	endif
endif

if ( -z $PIGIRPC ) then
	echo "$PIGIRPC binary is zero length. Perhaps make install failed?"
	ls -l $PIGIRPC
	exit 4
endif

if ( "$1" =~ "-*" ) then
    echo "${0}: Bad option: $1"
    exit 1
endif

# Check starting palette/facet for validity.
if ( -f $cell || ( -d $cell && ! -d $cell/schematic )) then
    echo "${0}: error: $cell exists but is not an Oct facet"
    exit 1
endif

# Make starting facet if needed
if ( ! -d $cell ) then
    echo "${0}": creating initial facet '``'$cell"''"
    mkdir $cell $cell/schematic
    cp $PTOLEMY/lib/defaultContent "$cell/schematic/contents;"
    chmod +w "$cell/schematic/contents;"
endif

set dbfile = /tmp/pigiXR$$

# Check to see whether xrdb is in the path
(xrdb -help) >& /dev/null
if ($status != 0) then
	set path = ( $path /usr/X11/bin /usr/bin/X11 )
endif
xrdb -query > $dbfile
if ($status != 0) then 
	echo "${progname}: 'xrdb -query' failed. Exiting"
	exit 1
endif

xrdb -merge $PTOLEMY/lib/$resfile

# Allow user-specified X resources
if ( $?PIGIXRES ) then
    xrdb -merge $PIGIXRES
endif

if ( $?pigiconsole ) then
	if ( $?pigidebug ) then
		echo "-console and -debug together means that the "
		echo "   pigi console prompt will come up inside the debugger"
	endif
        setenv TAILARGS -console
endif
if ( $?pigidebug ) then
	switch ($PTARCH)
		case hppa.cfront:
			#setenv COMMAND $PTOLEMY/lib/pigiRpcDebug.xdb
			setenv COMMAND $PTOLEMY/lib/pigiRpcDebug.dde
			breaksw
		case sol2.cfront:
		case sol2.5.cfront:
	                setenv COMMAND $PTOLEMY/lib/pigiRpcDebug.dbx
			breaksw
		case *:
	                setenv COMMAND $PTOLEMY/lib/pigiRpcDebug
			breaksw
	endsw
else if ( $?pigiconsole ) then
        setenv COMMAND $PTOLEMY/lib/pigiRpcConsole
endif

if ( ! $?VEMBINARY) setenv VEMBINARY $PTOLEMY/bin.$PTARCH/vem 

$VEMBINARY -G 600x150+0+0 -F ${cell}:schematic -G +0+200 -R $PTOLEMY/lib/pigiRpcShell

catch:

if ( -r $dbfile ) then
    xrdb $dbfile
    /bin/rm -f $dbfile
endif
