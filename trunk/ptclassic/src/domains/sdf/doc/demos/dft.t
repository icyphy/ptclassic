.\" $Id$
.NA dft
.SD
Compute a discrete Fourier transform of a finite signal using the
.c FFT
star.
The magnitude and phase (unwrapped) are plotted.
.EQ
delim off
.EN
.LO "$PTOLEMY/src/domains/sdf/utilities"
.DM SDF Universe
.SV $Revision$	$Date$
.AL "Edward A. Lee"
.EQ
delim $$
.EN
.LD
This demo computes a DFT (using the FFT star) of a finite signal.
.Ie "discrete Fourier transform"
.Ie "Fourier transform, discrete"
.Ie "DFT"
.Ie "spectral estimation"
It can be used, for example, to plot the frequency response of an FIR filter
given its impulse response.
This system is so useful that it is included in the Utilities menu of pigi.
The magnitude (in dB) and phase (unwrapped) are plotted.
.pp
The time-domain signal is read from a file using the
.c WaveForm
star.
The filename is given by the universe parameter \fIsignal\fR.
Notice how the syntax "< filename" is used to specify that the signal
should come from a file.
Alternatively, the signal could be specified directly.
For example, a square pulse with with 20 could be specified using the
syntax "1.0 [20] 0.0" for the value of \fIsignal\fR.
.pp
Immediately after the signal is read it is multiplied by another signal
consisting of alternating $+1$ and $-1$, i.e. $exp(j pi n)$.
This trick shifts the DC component of the signal to the Nyquist frequency,
and hence is an efficient way to get a spectral plot with the DC component
in the center of the plot.
Without this multiplication, DC would be at the left, and the sampling
frequency at the right.
With it, the output plot is centered at DC (frequency 0.0), and ranges
from $-pi$ to $pi$.
The proper labeling of the $x$-axis is achieved by using the
.c Ramp
star to generate the X input of the
.c XYgraph
stars.
.pp
Note that the conversion from the FLOAT output of the FIR filter
to the COMPLEX input of the
.c ComplexFFT
is implicit.
The default time-domain signal is an impulse response of an
equiripple lowpass filter designed using the optfir command.
.Ir "optfir program"
.Ie "FFT"
.Se Unwrap
.SA
ComplexFFT,
FFT,
optfir,
Unwrap,
WaveForm,
XYgraph.
.ES
