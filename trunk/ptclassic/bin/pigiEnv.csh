#! /bin/csh -f
# usage: pigi [-debug] [-console] [cell_name]

# pigi - Ptolemy Interactive Graphics Interface
# This version uses Vem version 8-1
# 
# This is a modified version to support a PTOLEMY environment variable.
#
# Version: $Id$
# Copyright (c) 1990,1991,1992 The Regents of the University of California.
# 	All Rights Reserved.
#

if ( ! $?DISPLAY ) then
    echo "${0}: Your DISPLAY environment variable must be set"
    exit 1
endif

if ( ! $?PTOLEMY ) setenv PTOLEMY ~ptolemy
if ( ! $?ARCH ) then
    setenv ARCH `$PTOLEMY/bin/arch`
endif

if ( ! $?PIGIRPC ) setenv PIGIRPC $PTOLEMY/bin.$ARCH/pigiRpc

if ( "$1" == "-debug" ) then
    shift
    set pigidebug
    if ( -x $PIGIRPC.debug ) setenv PIGIRPC $PIGIRPC.debug
endif

if ( "$1" == "-console" ) then
    shift
    set pigiconsole
endif

if ( "$1" =~ "-*" ) then
    echo "${0}: Bad option: $1"
    exit 1
endif

# Allow a user-specified starting palette,
set cell=$1; if ( $cell == "" ) set cell = "init.pal"

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
set resfile = pigiXRes9
if ( $?PIGIBW ) then
     xrdb -query > $dbfile
     set resfile = pigiXRes9.bw
else if ( $?PIGICP ) then
     xrdb -query > $dbfile
     set resfile = pigiXRes9.cp
endif
xrdb -merge $PTOLEMY/lib/$resfile

# Allow user-specified X resources
if ( $?PIGIRES ) then
    xrdb -merge $PIGIXRES
endif

 set path = ( $PTOLEMY/octtools/bin.$ARCH $path )

set cmdfile = /tmp/pigiCmds.$USER
/bin/rm -f $cmdfile
if ( ! $?nocmdfile ) then
    echo "PTOLEMY=$PTOLEMY"				>! $cmdfile
    echo "HOME=$HOME"					>> $cmdfile
    echo "ARCH=$ARCH"					>> $cmdfile
    echo "PIGIRPC=$PIGIRPC"				>> $cmdfile
    echo "DISPLAY=$DISPLAY"				>> $cmdfile
    echo "PATH=$PATH"					>> $cmdfile
    if ( $?pigiconsole ) then
        echo "TAILARGS=-console"			>> $cmdfile
    endif
    if ( $?pigidebug ) then
	echo "COMMAND=$PTOLEMY/lib/pigiRpcDebug"	>> $cmdfile
    else if ( $?pigiconsole ) then
	echo "COMMAND=$PTOLEMY/lib/pigiRpcConsole"	>> $cmdfile
    endif
endif

vem -G 600x150+0+0 -F ${cell}:schematic -G +0+160 -R $PTOLEMY/lib/pigiRpcShell

if ( -r $dbfile ) then
    xrdb $dbfile
    /bin/rm -f $dbfile
endif
/bin/rm -f $cmdfile
