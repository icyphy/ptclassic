.\" $Id$
.NA chaos
.SD
This is simple demonstration of chaos, in which the phase-space
plot of the famous Henon map is given.
.DE
.LO "$PTOLEMY/src/domains/sdf/demo"
.DM SDF Universe
.IE chaos
.SV $Revision$	$Date$
.AL "Edward A. Lee"
.LD
.Ir "Henon map"
.Ir "chaos"
This universe generates the so called Henon map, using
the following nonlinear recurrence:
.EQ
x(n+1) = 1 + y(n) - 1.4x sup 2 (n)
.EN
.EQ
y(n+1) = 0.3x(n) ~.
.EN
Initialized with zero, this system exhibits chaotic behavior.
.ES
