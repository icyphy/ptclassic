.\" $Id$
.NA telephoneChannelTest
.SD
Assuming a sampling rate of 8 kHz, a sinusoid at 500 Hz
is transmitted through a simulation of a telephone channel with
additive Gaussian noise, nonlinear distortion, and phase jitter.
.DE
.LO "$PTOLEMY/src/domains/sdf/demo"
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
.SE TelephoneChannel
.IR "phase jitter"
.IR "additive Gaussian noise"
.IR "nonlinear distortion"
.IP [1]
E. A. Lee and D. G. Messerschmitt,
.i "Digital Communication" ,
2nd ed., Kluwer Academic Publishers, Boston, MA, 1994, pp. 160-167.
.SA
TelephoneChannel
.ES
