.\" $Id$
.NA shotNoise
.SD
Generate a continuous-time shot-noise process
and display regularly spaced samples of it.
The shot noise is generated by feeding a Poisson
process into a Filter star.
.DE
.LO "$PTOLEMY/src/domains/de/demo"
.SV $Revision$ $Date$
.AL "E. A. Lee"
.LD
.IE Filter
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
