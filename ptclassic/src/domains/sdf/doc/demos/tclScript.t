.\" $Id$
.NA tclScript
.SD
Demonstrate the
.c TclScript
star by generating two interactive X window follies that consist of
circles that move in a playing field.
.EQ
delim off
.EN
.LO "$PTOLEMY/src/domains/sdf/demo"
.DM SDF Universe
.SV $Revision$ $Date$
.AL "Edward A. Lee"
.EQ
delim $$
.EN
.LD
This system uses the Tk X window toolkit to generated
an animated display.
The Tcl script it uses can be found in
.EQ
delim off
.EN
$PTOLEMY/src/domains/sdf/tcltk/stars/tkScript.tcl.
.EQ
delim $$
.EN
Instructions for writing such Tcl scripts can be found
in the programmer's manual.
.Se "TclScript"
.SA
TclScript.
.ES
