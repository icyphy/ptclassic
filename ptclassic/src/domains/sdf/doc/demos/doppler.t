.\" $Id$
.NA doppler
.SD
Doppler shift
.DE
.LO "~ptolemy/src/domains/sdf/demo"
.DM SDF Universe
.SV $Revision$ $Date$
.AL "Edward A. Lee"
.LD
.Id "doppler shift"
A sine wave of frequency 2*PI/40 radians
is subjected to four successive amounts of doppler shift.
The first 100 samples are not shifted at all (in the first 64 samples
transients are dying out).
The next 160 samples are shifted up in frequency by 2*PI/160
radians (so that there is one extra cycle for every four
cycles of the original sinusoid).
The next 160 samples are shifted down in frequency by 2*PI/160
radians (so that there is one fewer cycle for every four
cycles of the original sinusoid).
The remaining samples are not shifted at all.
.pp
The doppler shift is accomplished by the phaseShift galaxy,
which forms an analytic signal (using a Hilbert transform)
which modulates a complex exponential.
.Ie "phase shift"
.Ie "analytic signal"
.Ie "Hilbert transform"
.Se phaseShift
The input is assumed to be real, and the output
produced is real.  The frequency shift produced by the phaseShift
galaxy is the slope of the phase control input.  This input is
designed to be flat for the first 100 samples, rising for the next 160
samples, falling for the next 160 samples, and flat thereafter.
To get this waveform, the WaveForm star is combined with an Integrator.
The outputs of the WaveForm star, therefore, give the frequency offset
in radians.
.pp
Note that because the Hilbert filter in the phaseShift galaxy
is a 64-tap, linear-phase FIR filter,
there is a transient of 64 samples while the delay line of the filter
fills.  Since the group-delay of this filter is 32 samples, the
original sinusoid is delayed by 32 samples before being displayed.
.SA
phaseShift.
.ES
