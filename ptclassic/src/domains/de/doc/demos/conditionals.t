.\" $Id$
.NA conditionals
.SD
Demonstrate the use of the
.c Test
block in it various configurations to
compare the values of input events with floating-point values.
The input test signal is a pair of ramps, with each event repeated once after
some delay.
Since the ramps have different steps, they will cross.
.DE
.LO "$PTOLEMY/src/domains/de/demo"
.SV $Revision$ $Date$
.AL "Rolando Diesta and Edward A. Lee"
.LD
.Se Test DE
A comparison is performed whenever an event arrives on either input
of the stars.
The most recent arrival on each input is used.
If no event has arrived, then a zero is assumed.
This has the effect of a zero-order hold on each input: a value
persists until a new event sets a new value.
The use of the
.c BarGraph
star to display the outputs emphasizes this.
.SA
Logic.
.ES
