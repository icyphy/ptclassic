.\" $Id$
.NA comparison
.SD
Compare two sinusoidal signals using the Test star.
.DE
.LO "~ptolemy/src/domains/sdf/demo"
.DM SDF Universe
.IE comparison
.SV $Revision$	$Date$
.AL "E. A. Lee"
.LD
.Ir "Zero crossing"
.Ir "Signal crossing"
.Ir "Comparing signals"
This universe generates two sinusoids, 90 degrees out of phase
with one another, and compares them using the
.c Test
star with the
.c condition
parameter set to "GT".
The first comparison simply identifies when signal A is greater
than signal B.
The second comparison identifies when the signals cross.
.ES
