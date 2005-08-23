#! /bin/sh
# pigi - Ptolemy Interactive Graphics Interface
#
# Version: @(#)pigiEnv.sh	1.11 02/10/99
# Copyright (c) 1990-1999 The Regents of the University of California.
# 	All Rights Reserved.
#
# This script is what the user calls to start up the vem process and the
# pigiRpc process
# 

prg="`basename \"$0\"`"

# First check/guess PTOLEMY. PTOLEMY must be set in order to 
# run the following part of the script successfully.
# If not set try looking in the following order:
# TYCHO/.., PTDEFAULT, ~ptolemy, /users/ptolemy, the directory which holds pigi
if [ -z "${PTOLEMY-}" ]; then
    if [ "${TYCHO:+x}" ] && [ -f "$TYCHO/../bin/ptarch" ]; then
	PTOLEMY="$TYCHO/.."
    elif [ -d "${PTDEFAULT-}" ]; then
	PTOLEMY="$PTDEFAULT"
    elif [ -x /bin/csh ] && pto="`/bin/csh -fc 'echo ~ptolemy' 2>/dev/null`" \
    && [ -f "$pto/bin/ptarch" ]; then
	PTOLEMY="$pto"
    elif [ -f /users/ptolemy/bin/ptarch ]; then
	PTOLEMY=/users/ptolemy
    else
	# Last resort: Try directory which this script resides in.
	pto="`type \"$0\"|sed 's|.* (*\([^)]*/\).*|\1|'`"
	case "$pto" in
	  /*) ;;
	  *)  pto="`pwd`/$pto";;
	esac
	if [ -f "${pto}ptarch" ]; then
	    PTOLEMY="${pto}.."
	else
	    echo "$prg: \$PTOLEMY environment variable not set, exiting."
	    exit 2
	fi
    fi
    unset pto
elif [ ! -d "$PTOLEMY" ]; then
    echo "$prg: \$PTOLEMY == '$PTOLEMY',"
    echo "      which is not an existing directory, exiting."
    exit 2
fi

# Set the commonly used environment variables
# PTARCH is needed for the subsequent commands
. "$PTOLEMY/bin/ptsetup.sh"

xrdb=xrdb
case "$PTARCH" in
  nt4)
      # If we are running under nt4, then add the proper cpp
      xrdb="xrdb -cpp `gcc --print-prog-name cpp| sed 's@\\\@/@g'`";;
  alpha|alpha4)
      # On DEC Alpha, do not print unaligned access message (vem)
      uac p noprint;;
esac


# Call this sh function to print the usage
usage () {
    echo "Usage: $prg [-bw] [-cp] [-debug] [-console] [-help] [-ptiny] [-ptrim]"
    echo "       [-rpc rpcname] [-xres resname] [-display display] [cell_name]"
}

# Call this sh function to cleanup
cleanup () {
    if [ "${dbfile:+x}" ] && [ -r "$dbfile" ]; then
	$xrdb "$dbfile"
	rm -f "$dbfile"
    fi
    if [ -n "${xclockpid:-}" ]; then
	kill $xclockpid
    fi	
}

# On interrupt, clean up after ourselves
trap cleanup 0 1 2 13 15

cell=init.pal
resfile=pigiXRes9

while [ $# -gt 0 ]; do
    case "$1" in
      -help)
	  usage
	  exit 0;;
      -rpc)
	  PIGIRPC="${2:?Option -rpc needs parameter. Exiting.}"
	  shift;;
      -xres)
	  PIGIXRES="${2:?Option -xres needs parameter. Exiting.}"
	  shift;;
      -debug)
	  pigidebug=1;;
      -display)
	  DISPLAY="${2:?Option -display needs parameter. Exiting.}"
	  shift;;
      -console)
	  echo "$prg: -console is obsolete and no longer supported."
	  echo "      To bring up a Tycho console that has similar"
	  echo "      functionality, start pigi and type a 'y' while"
	  echo "      the mouse is over a facet." 
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
	  echo "$prg: Bad option: $1"
	  usage
	  exit 1;;
      *)
	  cell="$1"
	  case "$cell" in
	    */)
		echo "$prg: Warning: Can't handle names with trailing slashes."
		cell="`echo \"$cell\"|awk '{print substr($0,1,length($0)-1)}'`"
		echo "      Removed the trailing /, so now the pathname is"
		echo "      $cell";;
	  esac;;
    esac    
    shift
done

if [ -z "${DISPLAY-}" ]; then
    echo "$prg: Setting DISPLAY to localhost:0" 
    DISPLAY=localhost:0
    export DISPLAY
fi

if [ "${TYCHO+x}" ] && [ "$TYCHO" != "$PTOLEMY/tycho" ]; then
    echo "Warning: \$TYCHO != \$PTOLEMY/tycho"
    echo "          '$TYCHO' != $PTOLEMY/tycho"
    echo "          You may want to do 'unset TYCHO' or 'unsetenv TYCHO'"
fi

# Try and do some smart error recovery if the pigiRpc binary can't be found.
# Rules if we can't find a binary:
# If we can't find $PIGIRPC, try looking in obj.$PTARCH/pigiRpc
# If we are running this script as ptiny or ptrim, or as pigi -ptiny or
# pigi -ptrim try looking for ptinyRpc or ptrimRpc.
# If we are running with -debug, try looking for .debug images

if [ -z "${PIGIBASE-}" ]; then
    case "$0" in
      *ptiny)  
	  PIGIBASE=pigiRpc.ptiny;;
      *ptrim)
	  PIGIBASE=pigiRpc.ptrim;;
      *)
	  PIGIBASE=pigiRpc;;
    esac
fi

# If the user has set $PIGIRPC and is calling -debug, don't try and be 
# smart here
if [ -z "${PIGIRPC-}" ]; then
    PIGIRPC="$PTOLEMY/bin.$PTARCH/$PIGIBASE"
    if [ "${pigidebug:+x}" ]; then
	if [ -x "$PIGIRPC.debug" ]; then
	    PIGIRPC="$PIGIRPC.debug"
        else
	    if [ -x "$PTOLEMY/obj.$PTARCH/pigiRpc/$PIGIBASE.debug" ]; then
		echo "$prg: $PIGIRPC.debug does not exist or is not"
		PIGIRPC="$PTOLEMY/obj.$PTARCH/pigiRpc/$PIGIBASE.debug"
		echo "       executable. Using $PIGIRPC instead."
	    elif [ -x "$PIGIRPC" ]; then
		echo "$prg: $PIGIRPC.debug does not exist or is not"
		echo "       executable. Using $PIGIRPC instead."
	    fi	
	fi
    fi
fi

if [ ! -x "$PIGIRPC" ]; then
    echo "$prg: $PIGIRPC does not exist or is not executable."
    if [ -x "$PTOLEMY/obj.$PTARCH/pigiRpc/$PIGIBASE" ]; then
	PIGIRPC="$PTOLEMY/obj.$PTARCH/pigiRpc/$PIGIBASE"
	echo "      Using $PIGIRPC instead."
    else
	echo "      Exiting."
	exit 4
    fi
fi
if [ ! -s "$PIGIRPC" ]; then
    echo "$prg: $PIGIRPC binary is zero length. Perhaps make install failed?"
    ls -l "$PIGIRPC"
    exit 4
fi

# Check starting palette/facet for validity.
if [ -f "$cell" -o \( -d "$cell" -a ! -d "$cell/schematic" \) ]; then
    echo "$prg: Error: $cell exists but is not an Oct facet."
    exit 1
fi

# Make starting facet if needed
if [ ! -d "$cell" ]; then
    echo "$prg: creating initial facet \`\`$cell''"
    mkdir "$cell" "$cell/schematic"
    cp "$PTOLEMY/lib/defaultContent" "$cell/schematic/contents;"
    chmod +w "$cell/schematic/contents;"
fi


# Check to see whether xrdb is in the path
set $xrdb
# Note that under Cygwinb19.1 /bin/sh.exe does not have the 'type' command 
if type "$1" >/dev/null 2>&1; then
    :
elif [ -x "/usr/X11/bin/$1" ]; then
    append PATH /usr/X11/bin
elif [ -x "/usr/bin/X11/$1" ]; then
    append PATH /usr/bin/X11
elif [ -x "/usr/openwin/bin/$1" ]; then
    append PATH /usr/openwin/bin
elif [ -x "/usr/X11R6.4/bin/$1" ]; then
    append PATH /usr/X11R6.4/bin
else
    echo "$prg: warning: '$1' not found."
fi
    
dbfile=/tmp/pigiXR$$
$xrdb -query > "$dbfile"
if [ $? != 0 ]; then 
    echo "$prg: '$xrdb -query' failed, exiting."
    exit 1
fi

$xrdb -merge "$PTOLEMY/lib/$resfile"

if [ -z "`$xrdb -query`" ]; then
    echo "$prg: 'xrdb -query' returned no X resources."
    echo "      If you are running under the Hummingbird Exceed X Server"
    echo "      then you may need to start up another client, such as"
    echo "      'xclock &', before starting up Ptolemy."
    echo "      Ptolemy will now attempt to startup xclock and try again."
    xclock &
    xclockpid=$!
    echo "Sleeping 6 seconds"
    sleep 6
    $xrdb -merge "$PTOLEMY/lib/$resfile"
    if [ -z "`$xrdb -query`" ]; then
    	echo "$prg: 'xrdb -query' is still returning no resources. Exiting"
	exit 1
    else 
	echo "$prg: starting up xclock fixed the xrdb problem."
    fi	
fi

# Allow user-specified X resources
if [ "${PIGIXRES:+x}" ]; then
    $xrdb -merge "$PIGIXRES"
fi

if [ "${pigiconsole:+x}" ]; then
    if [ "${pigidebug:+x}" ]; then
	echo "$prg: -console and -debug together means that the pigi"
	echo "      console prompt will come up inside the debugger"
    fi
    TAILARGS=-console
fi
if [ "${pigidebug:+x}" ]; then
    case "$PTARCH" in
      hppa.cfront)
	  #COMMAND="$PTOLEMY/lib/pigiRpcDebug.xdb";;
	  COMMAND="$PTOLEMY/lib/pigiRpcDebug.dde";;
      sol2*.cfront)
          COMMAND="$PTOLEMY/lib/pigiRpcDebug.dbx";;
      *)
          COMMAND="$PTOLEMY/lib/pigiRpcDebug";;
    esac
elif [ "${pigiconsole:+x}" ]; then
    COMMAND="$PTOLEMY/lib/pigiRpcConsole"
fi

export PTOLEMY PIGIRPC DISPLAY PATH TAILARGS COMMAND

VEMBINARY="${VEMBINARY:-$PTOLEMY/bin.$PTARCH/vem}" 

$VEMBINARY -G 600x150+0+0 -F $cell:schematic -G +0+200 -R $PTOLEMY/lib/pigiRpcShell

