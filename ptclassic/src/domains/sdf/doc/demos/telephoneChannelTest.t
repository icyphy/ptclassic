.\" $Id$
.NA telephoneChannelTest
.SD
Assuming a sampling rate of 8 kHz, a sinusoid at 500 Hz
is transmitted through a simulation of a telephone channel with
additive Gaussian noise, nonlinear distortion, and phase jitter.
.DE
.LO "~ptolemy/src/domains/sdf/demo"
.DM SDF Universe
.SV $Revision$	$Date$
.AL "Edward A. Lee"
.EQ
delim off
.EN
.EQ
delim $$
.EN
.LD
The time domain plot of the output clearly shows the phase jitter
and the noise, but only the most discerning eye can see the nonlinear
distortion.  A periodogram power spectrum estimate, however, clearly
shows the second harmonic distortion, plus a DC offset.
.Se TelephoneChannel
.Ir "phase jitter"
.Ir "additive Gaussian noise"
.Ir "nonlinear distortion"
.SA
TelephoneChannel
.ES
