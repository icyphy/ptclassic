.\" $Id$
.NA phoneline
.SD
A telephone channel simulator.
.DE
.SV 1.1 "November 24, 1992"
.AL "Chih-Tsung Huang (ported from Gabriel)"
.LO "~ptolemy/src/domains/cg56/demo/sim"
.LD
.pp
.Id "telephone channel simulator"
A tone is passed through some processing which
implements various distortions on a telephone channel.
There is a total of 7 controllable parameters:
.TS
center;
l.
noise
channel filter
second harmonic
third harmonic
frequency offset
phase jitter frequency
phase jitter amplitude
.TE
.PP
Noise is generated using the \fBCG56IIDGaussian\fR star,
which simply averages a number of uniform random numbers.
This is much less expensive than the Box-Muller approach,
which will produce better Gaussians.
The standard deviation of the noise is scaled by 
the value of the \fInoise\fR parameter.
The noise power is the square of that number.
.PP
Linear distortion is introduced by the \fBCG56FIR\fR star
labelled channel filter.
The \fIchannel filter\fR parameters requires the
coefficients of an FIR filter.
The default filter is a null filter,
which simply introduces time delay.
.PP
To introduce frequency offset and phase jitter,
an analytic signal is produced.
Then the line signal is modulated by a complex exponential
that shifts its frequency and jitters its phase.
The \fIphase jitter frequency\fR parameter,
when multiplied by the sampling frequency,
yields the frequency in Hertz of the phase jitter.
The \fIphase jitter amplitude\fR parameter,
when multiplied by 180 degrees,
gives the peak phase jitter in degrees.
Both the \fIphase jitter frequency\fR and
the \fIphase jitter amplitude\fR parameters
are inside the \fBCG56Tone\fR star.
.PP
Nonlinear distortion is introduced by squaring and cubing the signal.
.UH BUGS:
The nonlinear distortion introduces DC components,
which does not accurately reflect the behavior of a telephone channel.
.ES
