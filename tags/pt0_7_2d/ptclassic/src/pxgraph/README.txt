Version: @(#)README.txt	1.3 01/28/99
$PTOLEMY/src/pxgraph/README.txt

In Ptolemy 0.7.1 and later, the X11 version of pxgraph has been replaced
with a Java version.

The Java pxgraph sources are in $PTOLEMY/tycho/java/ptplot.
The Java pxgraph binary is installed as $PTOLEMY/bin.$PTARCH/pxgraph

The X11 pxgraph sources in in $PTOLEMY/src/pxgraph/pxgraph.
The X11 pxgraph binary is installed as $PTOLEMY/bin.$PTARCH/pxgraph.x11

$PTOLEMY/bin/pxgraph is a script that reads the value of the
PT_USE_X11_PXGRAPH environment variable.  If that variable is
set, then X11 version is used.  If it is not set, then
the Java version is used


To set the PT_USE_X11_PXGRAPH environment variable, add the following
line to your .cshrc
	   setenv PT_USE_X11_PXGRAPH yes

or edit $PTOLEMY/bin/pxgraph and uncomment the line that sets
that variable.

Note that for this script to work, $PTOLEMY/bin needs to be in your
path earlier than $PTOLEMY/bin.$PTARCH.

