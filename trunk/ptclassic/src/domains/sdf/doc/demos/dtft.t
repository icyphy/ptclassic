.\" $Id$
.NA dtft
.SD
Demonstrate the DTFT star.
.DE
.SV $Revision$ "October 28, 1990"
.AL "E. A. Lee"
.LO "~ptolemy/src/domains/sdf/demo"
.LD
.pp
.Ie "spectral estimation"
.Ie "discrete time Fourier transform"
.Ie "DTFT"
.Ie "Fourier transform, discrete time"
This demo compares the function of the
.c DTFT
star to that of the
.c ComplexFFT
star.
.Ie "ComplexFFT"
While both stars output samples of the discrete-time Fourier
transform of a finite-length signal, the
.c FFT
star can only produce $2 sup N$ samples, where $N$ is the integer \fIorder\fR.
By contrast, the
.c DTFT
star can produce any number of samples, without any constraint that
the number be a power of two.
Furthermore, the FFT samples must be uniformly spaced, and cover the
entire spectrum from d.c. to the sampling frequency.
The DTFT
samples can be spaced at arbitrary intervals, and can cover any
subset of the spectrum.
An outside signal source (in this case a
.c FloatRamp
star) provides the values of $omega$ at which to sample the DTFT.
.pp
In both cases, the magnitude (in dB) and phase are plotted.
The DTFT output is plotted using the
.c XYgraph
star, so that the frequency axis of the plot can be properly labeled.
The labels are normalized frequency, where $2 pi$ is the sampling frequency.
.SA
ComplexFFT,
DTFT,
XYgraph.
.ES
