.\" $Id$
.NA comparison
.SD
Compare two sinusoidal signals using the
c. Test
star.
.DE
.LO "$PTOLEMY/src/domains/sdf/demo"
.DM SDF Universe
.IE comparison
.SV $Revision$	$Date$
.AL "Edward A. Lee"
.LD
.Ir "Zero crossing"
.Ir "Signal crossing"
.Ir "Comparing signals"
This universe generates two sinusoids, 90 degrees out of phase
with each other, and compares them using the
.c Test
star with the \fIcondition\fR parameter set to "GT" for greater than.
The first comparison simply identifies when signal A is greater
than signal B.
The second comparison identifies when the signals cross.
.ES
