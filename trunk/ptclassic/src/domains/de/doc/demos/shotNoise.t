.\" $Id$
.NA shotNoise
.SD
Generates a shot noise process.
.DE
.LO "~ptolemy/src/domains/de/demo"
.SV $Revision$ "October 22, 1990"
.AL "E. A. Lee"
.LD
.IE Filter
Shot noise is generated by feeding a Poisson process into a
.c Filter
block.
The
.c Filter
block interprets the input events as impulses, and produces outputs
whenever it receives an event on its \fIclock\fR input.
The outputs are samples of what a true, continuous-time filter
would produce if its inputs were ideal impulses.
This is, of course, exactly shot noise.
.IE "shot noise"
In this case, the filter parameters make a simple, one-pole filter,
so the impulse response is a decaying exponential.
.SA
Filter,
hdShotNoise,
Poisson.
.ES
