.\" $Id$
.NA distortion
.SD
Simulates the effect on a sinusoid of random delay and reordering of samples.
.DE
.LO "~ptolemy/src/domains/de/demo"
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
of 10,240/256 or 40 time units.
(We can change this by changing the
.c schedulePeriod
parameter of the SDF universe, whose default value is 10,000.
Currently, it is set to be 10,240.
Because of the FFT stars, the number of invocations of the reorder
wormhole is 256).
In the domain inside the
.c Wormhole ,
we may change the time scale by defining a parameter called
.c timeScale .
for the
.c Galaxy .
By default, no time scale is performed (
.c timeScale
is 1.0). 
In this demo, we set the state at 2.0, which means
one time unit in the SDF domain corresponds to two time units
in the DE domain.
The
.c Router
sends input events to one of two outputs
with equal probability.
The lower output goes to an
.c ExpDelay
star, which delays input events a random amount, according
to IID exponential random variables.
The mean of the exponential is set to equal the scaled sampling
period, 40x2=80.
The delayed and undelayed streams are merged and sampled.
The
.c Sampler
is required in order to ensure that this wormhole always produces
an output when given an input.
The outer SDF system expects this.
Note that the DE scheduler decides the firing order of the sampler
and the merge deterministically in case of simultaneous events to those
stars.  It prevents the use of an infinitesimal delay on the clock input
to the sampler which may be required for some commercial DE schedulers.
.SA
distortionQ,
Router,
ExpDelay,
Sampler.
.ES
