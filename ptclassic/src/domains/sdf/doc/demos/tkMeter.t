.\" $Id$
.NA tkMeter
.SD
Demonstrate the
.c TkMeter
star by creating three bar meters.
The first oscillates sinusoidally.
The second displays a random number between zero and one.
The third displays a random walk.
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
This system uses the Tk X window toolkit to generate an animated display.
.Se "TkMeter"
.SA
TkMeter.
.ES
