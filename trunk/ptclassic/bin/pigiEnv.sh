#! /bin/sh
# pigi - Ptolemy Interactive Graphics Interface
#
# Version: $Id$
# Copyright (c) 1990-%Q% The Regents of the University of California.
# 	All Rights Reserved.
#
# This script is what the user calls to start up the vem process and the
# pigiRpc process
# 

# Call this sh function to print the usage
usage () {
    echo "usage: pigi [-bw] [-cp] [-debug] [-console] [-help]"
    echo "  [-ptiny] [-ptrim] [-rpc rpcname] [-xres resname ] [-display display] [cell_name]"
}

xrdb=xrdb
if [ $PTARCH = "nt4" ]; then
    # If we are running under nt4, then add the proper cpp
    xrdb="xrdb -cpp `gcc --print-prog-name cpp| sed 's@\\\@/@g`" 
fi

# Call this sh function to cleanup
cleanup () {
    if [ -n "$dbfile" -a -r "$dbfile" ]; then
	$xrdb $dbfile
	rm -f $dbfile
    fi
}

# On interrupt, clean up after ourselves
trap cleanup 0 1 2 13 15

if [ -z "$PTOLEMY" ]; then
    if [ -d /users/ptolemy ]; then
	PTOLEMY=/users/ptolemy
    else	
	echo "$0: \$PTOLEMY environment variable not set, exiting script!"
    fi	
fi

# Set the commonly used environment variables
. $PTOLEMY/bin/ptsetup.sh
OCTTOOLS=${OCTTOOLS:=$PTOLEMY}

if [ -z "$DISPLAY" ]; then
    echo "${0}: Your DISPLAY environment variable must be set or use the"
    echo "    -display option"
    exit 1
fi


# On DEC Alpha, do not print unaligned access message (vem)
case $PTARCH in
    alpha|alpha4)
	uac p noprint
        breaksw;;
esac

# If the user has set PIGIRPC, check to see if it exists
# We don't try and do anything smart if the user has set $PIGIRPC
# and it does not exist.
if [ -n "$PIGIRPC" ]; then
    if [ ! -x "$PIGIRPC" ]; then
	echo "$0: \$PIGIRPC = '$PIGIRPC', which does not exist, exiting."
	exit 3
    fi
fi

cell=init.pal
resfile=pigiXRes9

while [ $# -gt 0 ]; do
    case $1 in
	-help)
	    usage
	    exit 0;;
	-rpc)
	    PIGIRPC=$2
	    shift;;
	-xres)
	    PIGIXRES=$2
	    shift;;
	-debug)
	    pigidebug=1;;
	-display)
	    DISPLAY=$2
	    shift;;
	-console)
	    echo "-console is obsolete and no longer supported."
	    echo " To bring up a Tycho console that has similar"
	    echo " functionality, start pigi and type a 'y' while"
	    echo " the mouse is over a facet." 
	    pigiconsole=1;;
	-bw)
	    resfile=pigiXRes9.bw;;
	-cp)
	    resfile=pigiXRes9.cp;;
	-ptiny)
	    PIGIBASE=pigiRpc.ptiny;;
	-ptrim)
	    PIGIBASE=pigiRpc.ptrim;;
	-*)
	    echo "Bad option: $1"
	    usage
	    exit 1;;
	*)
	    cell=$1
	    if [ `basename $cell` = "" ]; then
		echo "Warning: pigi can't handle names with trailing slashes"
		cell=`echo $cell | awk '{print substr($0,1,length($0)-1)}'`
		echo "Removed the trailing /, so now the pathname is $cell"
    fi;;
	esac    
	shift
done


# Try and do some smart error recovery if the pigiRpc binary can't be found.
# Rules if we can't find a binary:
# If we can't find $PIGIRPC, try looking in obj.$PTARCH/pigiRpc
# If we are running this script as ptiny or ptrim, or as pigi -ptiny or
# pigi -ptrim try looking for ptinyRpc or ptrimRpc.
# If we are running with -debug, try looking for .debug images

if [ -z "$PIGIBASE" ]; then
     case $progname in
	ptiny)  
	    PIGIBASE=pigiRpc.ptiny;;
	ptrim)
	    PIGIBASE=pigiRpc.ptrim;;
	*)
	    PIGIBASE=pigiRpc;;
     esac
fi

# If the user has set $PIGIRPC and is calling -debug, don't try and be 
# smart here
if [ -z "$pigidebug" -a -z "$PIGIRPC" ]; then
    if [ -z "$PIGIRPC" ]; then
	     PIGIRPC=$PTOLEMY/bin.$PTARCH/$PIGIBASE
	fi
	if [ -z "$pigidebug" -a ! -x $PIGIRPC.debug ]; then
		echo "$PIGIRPC.debug does not exist or is not executable"	
		if [ -x $PTOLEMY/obj.$PTARCH/pigiRpc/${PIGIBASE}.debug ]; then
			PIGIRPC=$PTOLEMY/obj.$PTARCH/pigiRpc/${PIGIBASE}.debug
			echo "Using $PIGIRPC instead"
		else
			if [ -x $PIGIRPC ]; then
				echo "Using $PIGIRPC instead."
			fi
		fi
	else
		if [ -n $"pigidebug" ]; then
			PIGIRPC=$PIGIRPC.debug
		fi
	fi
fi


if [ -z "$PIGIRPC" ]; then
     PIGIRPC=$PTOLEMY/bin.$PTARCH/$PIGIBASE
fi

if [ ! -x $PIGIRPC ]; then
    echo "$PIGIRPC does not exist or is not executable."
	if [ -n "$pigidebug" -a -x $PTOLEMY/obj.$PTARCH/pigiRpc/${PIGIBASE}.debug ]; then
	    PIGIRPC=$PTOLEMY/obj.$PTARCH/pigiRpc/${PIGIBASE}.debug
	    echo "Using $PIGIRPC instead"
	else
		if [ -x $PTOLEMY/obj.$PTARCH/pigiRpc/$PIGIBASE ]; then
		    PIGIRPC=$PTOLEMY/obj.$PTARCH/pigiRpc/$PIGIBASE
		    echo "Using $PIGIRPC instead"
		else
		    echo "${progname}: exiting"
		    exit 4
		fi
	fi
fi

if [ -z $PIGIRPC ]; then
    echo "$PIGIRPC binary is zero length. Perhaps make install failed?"
    ls -l $PIGIRPC
    exit 4
fi

case "$1" in
    -*)
	echo "${0}: Bad option: $1"
	exit 1;;
esac

# Check starting palette/facet for validity.
if [ -f $cell -o \( -d $cell -a ! -d $cell/schematic \) ]; then
    echo "${0}: error: $cell exists but is not an Oct facet"
    exit 1
fi

# Make starting facet if needed
if [ ! -d $cell ]; then
    echo "${0}": creating initial facet '``'$cell"''"
    mkdir $cell $cell/schematic
    cp $PTOLEMY/lib/defaultContent "$cell/schematic/contents;"
    chmod +w "$cell/schematic/contents;"
fi

dbfile=/tmp/pigiXR$$


# Check to see whether xrdb is in the path
($xrdb -help) >/dev/null 2>&1
if [ $? != 0 ]; then
    PATH=$PATH:/usr/X11/bin:/usr/bin/X11
fi
$xrdb -query > $dbfile
if [ $? != 0 ]; then 
    echo "${progname}: '$xrdb -query' failed. Exiting"
    exit 1
fi

$xrdb -merge $PTOLEMY/lib/$resfile

# Allow user-specified X resources
if [ -n "$PIGIXRES" ]; then
    $xrdb -merge $PIGIXRES
fi

if [ -n "$pigiconsole" ]; then
    if [ -n "$pigidebug" ]; then
	echo "-console and -debug together means that the "
	echo "   pigi console prompt will come up inside the debugger"
    fi
    TAILARGS=-console
fi
if [ -n "$pigidebug" ]; then
    case $PTARCH in
	hppa.cfront)
	    #COMMAND=$PTOLEMY/lib/pigiRpcDebug.xdb
	    COMMAND=$PTOLEMY/lib/pigiRpcDebug.dde;;
	sol2.cfront|sol2.5.cfront)
            COMMAND=$PTOLEMY/lib/pigiRpcDebug.dbx;;
	*)
            COMMAND=$PTOLEMY/lib/pigiRpcDebug;;
	esac
else 
    if [ -n "$pigiconsole" ]; then
	COMMAND=$PTOLEMY/lib/pigiRpcConsole
    fi
fi

export PTOLEMY HOME PTARCH PIGIRPC DISPLAY PATH TCL_LIBRARY TK_LIBRARY PTPWD LD_LIBRARY_PATH TYCHO OCTTOOLS COMMAND

VEMBINARY=${VEMBINARY:=$PTOLEMY/bin.$PTARCH/vem} 

$VEMBINARY -G 600x150+0+0 -F ${cell}:schematic -G +0+200 -R $PTOLEMY/lib/pigiRpcShell



