.\" $Id$
.NA chaos
.SD
A simple demonstration of chaos.
.DE
.IE chaos
.SV $Revision$	$Date$
.AL "E. A. Lee"
.LO "~ptolemy/src/domains/sdf/demo"
.LD
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
