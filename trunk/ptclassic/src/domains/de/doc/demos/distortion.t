.\" $Id$
.NA distortion
.SD
This demo shows the effects on real-time signals of a highly
simplified packet-switch network.
Packets can arrive out of order, and they can also arrive too late
to be useful.
In this simplified system, a sinusoid is generated in the SDF domain,
launched into the communication network implemented in the DE domain,
and compared to the output of the communication network.
Plots are given in the time and frequency domain of the sinusoid before and
after the network.
.DE
.EQ
delim off
.EN
.LO "$PTOLEMY/src/domains/de/demo"
.EQ
delim $$
.EN
.SV $Revision$ $Date$
.AL "Edward A. Lee"
.LD
On the time domain plot, two phenomena can be observed,
reordering and reuse.
Reordering is where two samples have exchanged places, thereby causing
unexpected jagged edges in the waveform.
Reuse indicates that a sample appeared twice and means that the next sample
did not arrive in time, so the receiver simply re-used the sample it had.
.pp
The communication network itself is implemented in a wormhole
(which looks like a galaxy).
It is a discrete event system consisting of a random router,
and exponential delay, a merge, and a sampler.
Events arrive from the SDF universe at intervals of 10,240/256 or
40 time units.
(We can change this by changing the \fIschedulePeriod\fR parameter of
the SDF universe, whose default value is 10,000.
Currently, it is set to be 10,240.
Because of the FFT stars, the number of invocations of the reorder
wormhole is 256 or $2 sup 8$).
In the domain inside the \fIWormhole\fR,
we may change the time scale by defining a parameter called
\fItimeScale\fR for the galaxy.
By default, no time scale is performed (\fItimeScale\fR is 1.0). 
In this demo, we set the \fItimeScale\fR to 2.0, which means one time unit
in the SDF domain corresponds to two time units in the DE domain.
The
.c Router
sends input events to one of two outputs with equal probability.
The lower output goes to an
.c ExpDelay
star, which delays input events a random amount, according
to IID exponential random variables.
The mean of the exponential is set to equal the scaled sampling
period, 40 x 2 = 80.
The delayed and undelayed streams are merged and sampled.
The
.c Sampler
star is required in order to ensure that this wormhole always produces
an output when given an input.
The outer SDF system expects this.
Note that the DE scheduler decides the firing order of the sampler
and the merge deterministically in the case that simultaneous events
appear at those stars.
It prevents the use of an infinitesimal delay on the clock input
to the sampler which may be required for some commercial DE schedulers.
.SA
distortionQ,
ExpDelay,
Router,
Sampler.
.ES
