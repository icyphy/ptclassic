#! /bin/csh -f
# usage: pigi [-bw] [-cp] [-debug] [-console]
#	[-rpc rpcname] [-xres resname ] [cell_name]

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
if ( ! $?OCTTOOLS ) setenv OCTTOOLS $PTOLEMY
if ( ! $?ARCH ) then
    setenv ARCH `$PTOLEMY/bin/arch`
endif
if ( ! $?USER ) then
    setenv USER $LOGNAME
endif

if ( ! $?PIGIRPC ) setenv PIGIRPC $PTOLEMY/bin.$ARCH/pigiRpc

set cell = init.pal
set resfile = pigiXRes9

while ($#argv)
	switch ($argv[1])
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
		case -console:
			set pigiconsole
			breaksw
		case -bw:
			set resfile = pigiXRes9.bw
			breaksw
		case -cp:
			set resfile = pigiXRes9.cp
			breaksw
		case -*:
			echo Bad option: $argv[1]
			exit 1
			breaksw
		case *:
			set cell=$1
			breaksw
	endsw
	shift
end


if ($?pigidebug && -x $PIGIRPC.debug ) setenv PIGIRPC $PIGIRPC.debug

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
