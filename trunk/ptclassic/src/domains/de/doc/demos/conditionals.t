.\" $Id$
.NA conditionals
.SD
Demonstrates the use of various blocks that compare the values
of input events with floating-point values.
.DE
.LO "~ptolemy/src/domains/de/demo"
.SV $Id$	$Revision$
.AL "Rolando Diesta and Edward A. Lee"
.LD
.Se TestGE DE
.Se TestGT DE
.Se TestEQ DE
.Se TestNE DE
This demonstration shows various features of
four stars in the DE domain that compare
the value of input events with floating-point
values.  The stars are
.c TestGE,
.c TestGT,
.c TestEQ,
and
.c TestNE,
where the names stand for "greater than or equal to", "greater than", etc.
They are wired up to perform the obvious tests
as well as the
.c LT
and
.c LE
tests (which simply require reversing the wires on the inputs of appropriate
stars).
.pp
The input test signal is a pair of ramps with each event repeated once after some
delay.  Since the ramps have different steps, they will cross.
.pp
A comparison is performed whenever an event arrives on either input of the stars.
The most recent arrival on each input is used.
This has the effect of a zero-order hold on each input: a value persists until a new
event sets a new value.  The plots generated may be a little confusing because
they show linear interpolation between points.
.SA
TestGE,
TestGT,
TestEQ, and
TestNE.
.ES
