.\" $Id$
.NA dft
.SD
Compute a discrete Fourier transform of a finite signal
.DE
.SV $Revision$	$Date$
.AL "Edward A. Lee"
.LO "~ptolemy/src/domains/sdf/demo"
.LD
This demo computes a DFT (using the FFT star) of a finite signal.
.Ie "discrete Fourier transform"
.Ie "Fourier transform, discrete"
.Ie "DFT"
.Ie "spectral estimation"
It can be used, for example, to plot the frequency response of an FIR filter
given its impulse response.  This system is so useful that it is
included in the Utilities menu of pigi.
The magnitude (in dB) and phase (unwrapped) are plotted.
.pp
The time-domain signal is read from a file using the WaveForm star.
The filename is given by the universe parameter "signal".
Notice how the syntax "< filename" is used to specify that the signal
should come from a file.  Alternatively, the signal could be
specified directly.  For example, a square pulse with with 20
could be specified using the syntax "1.0 [20] 0.0" for the value of "signal".
.pp
Immediately after the signal is read it is multiplied by another signal
consisting of alternating + and - one.  This trick shifts the d.c. component
of the signal to the Nyquist frequency, and hence is an efficient way to
get a spectral plot with the d.c. component in the center of the plot.
Without this multiplication, d.c. would be at the left, and the sampling
frequency at the right.
With it, the output plot is centered at d.c. (frequency 0.0), and ranges
from -PI to PI.  The proper labeling of the x axis is achieved by using
the FloatRamp star to generate the X input of the XYgraph stars.
.pp
Note that the conversion from the FLOAT output of the FIR filter
to the COMPLEX input of the ComplexFFT is implicit.
The default time-domain signal is an impulse response of an
equiripple lowpass filter designed using the optfir
command.
.Ir "optfir program"
.Ie "FFT"
.Ie "Unwrap"
.SA
FFT,
ComplexFFT,
optfir,
Unwrap,
WaveForm,
XYgraph.
.ES
