.\" $Id$
.NA multirate
.SD
A universe that upsamples a signal by a ratio of 5/2.
.DE
.LO "~ptolemy/src/domains/sdf/demo"
.DM SDF Universe
.SV $Revision$ "October 15, 1990"
.AL "E. A. Lee"
.LD
This universe generates a sine wave using the
.c singen
galaxy and feeds it into an
.c FIR
star with parameters set to upsample
by a ratio of 5/2 (the lower path).
.Ie FIR
.Ie "multirate signal processing"
.Ie "sample rate conversion"
The FIR star internally uses a polyphase
filter structure for efficient computation.
Along the upper path, a functionally equivalent, but
less efficient implementation uses
.c UpSample
and
.c DownSample
stars.
Because of properties of synchronous dataflow scheduling, when you
run this universe for 25 iterations, you get 50 samples of the input
signal and 125 samples (5/2 times as many) of the output signal.
The filter in the FIR star is a lowpass with its cutoff frequency
at 1/20th of the sample rate.  In this case, the sample rate
in question is five times the input sample rate, which is equal to
twice the output sample rate.  The low cutoff conservatively prevents
aliasing due to the sample rate conversions.
The filter was designed using the optfir program, which can be invoked
through the pigi menu.
.Ir "optfir program"
.SA
optfir,
interp,
analytic,
FIR.
.ES
