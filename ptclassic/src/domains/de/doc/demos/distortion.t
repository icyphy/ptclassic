.\" $Id$
.NA distortion
.SD
Simulates the effect on a sinusoid of random delay and reordering of samples.
.DE
.SV $Revision$ "October 15, 1990"
.AL "Edward A. Lee"
.LD
This demo shows the effects on real-time signals of
a highly simplified packet-switch network.
Packets can arrive out of order,
and can also arrive too late to be useful.
.pp
In this simplified system,
a sinusoid is generated in the SDF domain, launched into the communication
network, and compared to the output of the communication network.
Plots are given in the time and frequency domain of the sinusoid
before and after the network.
On the time domain plot, two phenomena can be observed,
reordering and reuse.
Reordering is where two samples have exchanged places, causing
unexpected jagged edges in the waveform.
Reuse is where a sample appears twice;
this means that the next sample did not arrive in time,
so the receiver simply re-used the sample it had.
.pp
The communication network itself is implemented in a wormhole
(which looks like a galaxy).
It is a discrete event system consisting of a random router,
and exponential delay, a merge, and a sampler.
Events arrive from the SDF universe at intervals
of 10,000/256 or roughly 39 time units.
(Sorry, there is currently no way to change this;
the default 
.c schedulePeriod
for the SDF universe is 10,000,
and because of the FFT stars, the number of invocations of the reorder
wormhole is 256).
The
.c Router
sends input events to one of two outputs
with equal probability.
The lower output goes to an
.c ExpDelay
star, which delays input events a random amount, according
to IID exponential random variables.
The mean of the exponential is set to roughly equal the sampling
period, 39.
The delayed and undelayed streams are merged and sampled.
The
.c Sampler
is required in order to ensure that this wormhole always produces
an output when given an input.
The outer SDF system expects this.
The
.c clock
input to the sampler is the input, delayed an infinitesimal amount.
The delay ensures that the sampler fires after the merge
when samples are routed through the upper path.
Without the delay, the scheduler could decide to fire the sampler
before the merge, and the input sample will be delayed by one sample
period, or possibly even lost.
The delay prevents this scheduler-dependent non-determinacy.
.SA
distortionQ,
Router,
ExpDelay,
Sampler.
.ES
