.\" $Id$
.NA integrator
.SD
Demonstrate the features of the integrator star.
.DE
.LO "~ptolemy/src/domains/sdf/demo"
.DM SDF Universe
.SV $Revision$ "October 15, 1990"
.AL "Edward A. Lee"
.LD
This system shows the various features of the
.c Integrator
star.
.Se Integrator
The leftmost integrator has limits so that its output is a sawtooth.
The
.c Quantizer
star resets the rightmost integrator when
the sawtooth is at its peaks.  The rightmost integrator also has
leakage, so its output signal is bowed.
.Se Quantizer
Note that a
.c FloatDC
star is connected to the INT reset input
of the leftmost integrator.  The type conversion is automatic,
converting the FLOAT 0.0 into the INT 0.
.SA
Integrator.
.ES
