.\" $Id$
.NA freqsample
.SD
Filter design via frequency sampling.
.DE
.SV $Revision$ "October 20, 1990"
.AL "J. Buck and E. A. Lee"
.LO "~ptolemy/src/domains/sdf/demo"
.LD
This system designs filters using the frequency sampling method.
The frequency domain specification is given using the
.c WaveForm
star, and using some of the more elaborate features of the parameter
interpreter.
The filter specification in this demo reads:
.pp
1 [6] 0.3904 0 [19] 0.3904 1 [5]
.pp
which translates to six samples with magnitude 1.0, one transition
sample with magnitude 0.3904, 19 samples with magnitude 0.0,
the same transition sample again, and five sample with magnitude 1.0.
This specifies a lowpass filter with the cutoff at about 1/3
of the Nyquist frequency.
The frequency specification is first modulated by a sequence
of alternating $+- 1$.
This effectively translates the impulse response so that it
is symmetric about the center sample, number 15.
Modulation in the frequency domain yields translation in the time domain.
It is an interesting experiment to remove this modulation and compare
the resulting frequency response.
.pp
After this modulation,
an inverse FFT gives the impulse response of the filter.
Both the real and imaginary parts of this are plotted.
Symmetry in the specification will result in the imaginary parts being zero.
To compute the frequency response, a forward FFT of much higher order
is computed (256, in this case).  The plot therefore has
enough resolution to see how the frequency response behaves
in-between the specified samples.
Note that the \fIsize\fR parameter of the FFT is 32,
indicating that only 32 input samples should be read.
Of course, as is standard with the frequency sampling method, the magnitude
response of the resulting filter exactly passes through the given
samples at discrete frequency intervals.
This can be easily verified with the plot.
.pp
To obtain a filter with a real-valued impulse response, the
frequency domain specification
must be symmetric, as with the default parameters.
To understand the symmetry precisely, note
that the frequency domain specification is actually only one
cycle of a periodic specification.
With this observation, and the fact that the first sample specifies
the d.c. response, the symmetry is obvious.
.pp
To design a filter of a different order
from that in the demo, you must modify the \fIsize\fR and \fIorder\fR
parameters of the first
.c ComplexFFT
star, and the \fIsize\fR parameter of the second.
.SA
WaveForm,
ComplexFFT.
.ES
