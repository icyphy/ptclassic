#! /bin/csh -f
# usage: pigi [-bw] [-cp] [-debug] [-console] [-help] [-ptiny] [-ptrim]
#	[-rpc rpcname] [-xres resname ] [cell_name]

# pigi - Ptolemy Interactive Graphics Interface
# This version uses Vem version 8-1
# 
# This is a modified version to support a PTOLEMY environment variable.
#
# Version: $Id$
# Copyright (c) 1990-1995 The Regents of the University of California.
# 	All Rights Reserved.
#

# On interrupt, remove temporary files
onintr catch

# If 'ptiny' is a link to this script, then we just use ptinyRpc
set progname = `basename $0`

if ( ! $?PTOLEMY ) setenv PTOLEMY ~ptolemy
if ( ! $?OCTTOOLS ) setenv OCTTOOLS $PTOLEMY
if ( ! $?ARCH ) then
    setenv ARCH `$PTOLEMY/bin/ptarch`
endif
if ( ! $?USER ) then
    setenv USER $LOGNAME
endif
if ( ! $?PTX11DIR ) then
    switch ($ARCH)
	case sol2:
	case sol2.cfront:
	    setenv PTX11DIR /usr/openwin
	    breaksw
	case *:
	    setenv PTX11DIR /usr/X11
	    breaksw
    endsw
endif
if ( ! $?LD_LIBRARY_PATH ) then
    setenv LD_LIBRARY_PATH /usr/lib:${PTX11DIR}/lib
endif

setenv PIGIBASE pigiRpc
if ( ! $?PIGIRPC ) then
     switch ($progname)
	case ptiny:  
		setenv PIGIBASE ptinyRpc
		breaksw
	case ptrim:
		setenv PIGIBASE ptrimRpc
		breaksw
	case *:
		setenv PIGIBASE pigiRpc
		breaksw
     endsw
     setenv PIGIRPC $PTOLEMY/bin.$ARCH/$PIGIBASE
endif

set cell = init.pal
set resfile = pigiXRes9

setenv TCL_LIBRARY $PTOLEMY/tcltk/tcl/lib/tcl
setenv TK_LIBRARY $PTOLEMY/tcltk/tk/lib/tk
setenv PTPWD `pwd`

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
			set pigiconsole
			breaksw
		case -bw:
			set resfile = pigiXRes9.bw
			breaksw
		case -cp:
			set resfile = pigiXRes9.cp
			breaksw
		case -ptiny:
			setenv PIGIBASE ptinyRpc
			setenv PIGIRPC $PTOLEMY/bin.$ARCH/ptinyRpc
			breaksw
		case -ptrim:
			setenv PIGIBASE ptrimRpc
			setenv PIGIRPC $PTOLEMY/bin.$ARCH/ptrimRpc
			breaksw
		case -*:
			echo Bad option: $argv[1]
			Echo "usage: $progname [-bw] [-cp] [-debug] [-console] [-help]"
			echo "  [-ptiny] [-ptrim] [-rpc rpcname] [-xres resname ] [-display display] [cell_name]"
			exit 1
			breaksw
		case *:
			set cell=$1
			breaksw
	endsw
	shift
end

if ( ! $?DISPLAY ) then
    echo "${0}: Your DISPLAY environment variable must be set or use the"
    echo "    -display option"
    exit 1
endif

if ($?pigidebug && -x $PIGIRPC.debug ) then
	setenv PIGIRPC $PIGIRPC.debug
endif

if ( ! -e $PIGIRPC ) then
	echo "$PIGIRPC does not exist."
	if ( -e $PTOLEMY/obj.$ARCH/pigiRpc/$PIGIBASE ) then
		setenv PIGIRPC $PTOLEMY/obj.$ARCH/pigiRpc/$PIGIBASE
		echo "Using $PIGIRPC instead"
	else
		echo "${program}: exiting"
		exit 4
	endif
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
xrdb -merge $PTOLEMY/lib/$resfile

# Allow user-specified X resources
if ( $?PIGIXRES ) then
    xrdb -merge $PIGIXRES
endif

set path = ( $PTOLEMY/bin.$ARCH $PTOLEMY/bin $path )

if ( $?pigiconsole ) then
        setenv TAILARGS -console
endif
if ( $?pigidebug ) then
        if ( "$ARCH" == "hppa.cfront" ) then
                setenv COMMAND $PTOLEMY/lib/pigiRpcDebug.xdb
        else
                setenv COMMAND $PTOLEMY/lib/pigiRpcDebug
        endif
else if ( $?pigiconsole ) then
        setenv COMMAND $PTOLEMY/lib/pigiRpcConsole
endif

if ( ! $?VEMBINARY) setenv VEMBINARY $PTOLEMY/bin.$ARCH/vem 

$VEMBINARY -G 600x150+0+0 -F ${cell}:schematic -G +0+200 -R $PTOLEMY/lib/pigiRpcShell

catch:

if ( -r $dbfile ) then
    xrdb $dbfile
    /bin/rm -f $dbfile
endif
