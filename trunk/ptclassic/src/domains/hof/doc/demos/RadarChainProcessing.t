.\" $Id$
.NA RadarChainProcessing
.SD
The radar chain processing demonstration simulates radar without
beamforming.
The system has been converted from a data parallel form that uses
a five-dimensional data array to a functional parallel form
that uses higher-order functions to manipulate streams of streams.
.DE
.EQ
delim off
.EN
.LO "$PTOLEMY/src/domains/hof/demo"
.EQ
delim $$
.EN
.DM SDF Universe
.SV $Revision$	$Date$
.AL "Karim P. Khiar"
.LD
The universe (global) parameters for this demonstration are:
\fINf\fR number of filters,
\fINp\fR number of pulses,
\fINa\fR number of sensors,
\fINs\fR radar window size in samples (range bins), and
\fINtargets\fR number of targets.

The Matlab star plots the Doppler axis versus a combination of the others.
It can be used to determine the target speed by looking for the highest filter
response.
Thus, this filter will give the Doppler range frequency for the targets.

We give dimensions of the data processed by the bank of
.c RadarAntenna
galaxies.
Each
.c RadarAntenna
galaxy produces \fINp\fR streams of \fINs\fR x \fINa\fR x \fINtargets\fR
samples.
Thus, the MATLAB matrix is
\fINp\fR streams of \fINs\fR x \fINa\fR x \fINtargets\fR.
The hypercube dimension is
\fINa\fR . [ \fINtargets\fR . [  \fINs\fR . \fINf\fR ] ].
.IR "radar"
.IR "Doppler radar"
.IP [1]
K. Khiar and E. A. Lee, "Modeling Radar Systems Using Hierarchical Dataflow,"
.i "Proc. IEEE Int. Conf. on Acoustics, Speech, and Signal Processing",
Detroit, MI, May 1995, pp. 3259-3262.
.SA
GenTarget,
OneDoppler,
PulseComp,
RadarAntenna,
RadarTargets,
SubAntenna.
.ES
