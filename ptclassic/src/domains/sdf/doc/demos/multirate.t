.\" $Id$
.NA multirate
.SD
Upsample a sinusoidal signal by a ratio of 5/2 using a
polyphase lowpass interpolating
.c FIR
filter.
.EQ
delim off
.EN
.LO "$PTOLEMY/src/domains/sdf/demo"
.DM SDF Universe
.SV $Revision$	$Date$
.AL "Edward A. Lee"
.EQ
delim $$
.EN
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
at 1/20th of the sample rate.
In this case, the sample rate in question is five times the input sample
rate, which is equal to twice the output sample rate.
The low cutoff conservatively prevents aliasing due to the sample rate
conversions.
The filter was designed using the optfir program, which can be invoked
through the pigi menu.
.Ir "optfir program"
.SA
analytic,
FIR,
interp,
optfir.
.ES
