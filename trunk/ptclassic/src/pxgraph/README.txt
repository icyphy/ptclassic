Version: $Id$
$PTOLEMY/src/pxgraph/README.txt

In Ptolemy 0.7.1, the X11 version of pxgraph has been replaced
with a Java version.

The Java pxgraph sources are in $PTOLEMY/tycho/java/ptplot.
The X11 pxgraph sources in in $PTOLEMY/src/pxgraph/pxgraph.

The default installation uses the Java pxgraph.  If you do not have
Java on your platform, or you want to run the X11 pxgraph for
some other reason, then follow the instructions below.

1. You can download prebuilt X11 pxgraph binary for certain platforms
from:
ftp://ptolemy.eecs.berkeley.edu/pub/misc/pxgraph

As of 4/98, binaries for the following platforms were available:
alpha4 hppa hppa9 sol2.5 sun4

If binaries are available for your platform, download the binary
and place it in $PTOLEMY/bin.$PTARCH with the name pxgraph.x11

OR
--

2. If pxgraph binaries for your platform are not available, you
will need to build the binary by following the steps below:
     1. cd to $PTOLEMY/src/pxgraph/pxgraph
     2. Run './configure'
     To see what options are available, run './configure -help'
     3. Run 'make'
     4. cd up a directory to $PTOLEMY/src/pxgraph and run 'make install'

     Note that we are not making the object files in a separate
     directory, so if you are installing for multiple platforms,
     you will need to do a make clean between each build


3. After installing a pxgraph.x11 binary in step 1 or 2 above,
either set the PT_USE_X11_PXGRAPH environment variable with:
	   setenv PT_USE_X11_PXGRAPH yes
in your .cshrc

or edit $PTOLEMY/bin/pxgraph and uncomment the line that sets
that variable.

Note that for this script to work, $PTOLEMY/bin needs to be in your
path earlier than $PTOLEMY/bin.$PTARCH.

If the Java version of pxgraph works and meets most of your needs,
but you occasionally need the X11 version for something like MIF
output, you can install the X11 version as pxgraph.x11 and then
type a 'x' while the mouse is over the Java ptplot to start up
pxgraph.x11 with the same command line arguments.

