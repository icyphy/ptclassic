.\" $Id$
.NA telephoneChannelTest
.SD
A sinusoidal test signal is sent over a telephone channel simulation.
.DE
.SV $Revision$	$Date$
.AL "Edward A. Lee"
.LO "$PTOLEMY/src/domains/sdf/demo"
.LD
Assuming a sampling rate of 8kHz, a sinusoid at 500 Hz
is transmitted through a simulation of a telephone channel with
additive Gaussian noise, nonlinear distortion, and phase jitter.
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
