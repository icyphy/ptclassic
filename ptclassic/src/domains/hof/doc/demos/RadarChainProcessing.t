.\" $Id$
.NA Chain_Treatment
.SD
The Chain Treatment (Processing) demonstration simulates radar without
beamforming.  The system has been converted from a data parallel form
that uses a five-dimensional data array to a functional parallel form
that uses higher-order functions to produce streams of streams.
.DE
.LO "$PTOLEMY/src/domains/hof/demo"
.DM SDF Universe
.SV $Date$
.AL Karim P. Khiar
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

We give dimensions of the data processed by the bank of Radar Antennas.
Each Radar Antenna produces Np streams of Ns x Na x Ntargets samples.
Thus, the Matlab matrix is Np by (Ns x Na x Ntargets).
The hypercube dimension is Na . [ Ntarget . [  Ns. Nf ] ].
.IR "radar"
.IR "Doppler radar"
.IP [1]
K. Khiar and E. A. Lee, "Modeling Radar Systems Using Hierarchical Dataflow,"
.i "Proc. IEEE Int. Conf. on Acoustics, Speech, and Signal Processing",
Detroit, MI, May 1995, pp. 3259-3262.
.SA
GeneTarget.pal,
One_Doppler,
PulseComp,
Radar_Antenna,
Radar_Targets,
subantenna.pal.
.ES
