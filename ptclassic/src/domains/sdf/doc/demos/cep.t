.\" $Id$
.NA cep
.SD
Stabilization of IIR filters using the cepstrum
.DE
.SV $Revision$ $Date$
.AL "Joseph T. Buck"
.LO "~ptolemy/src/domains/sdf/demo"
.LD
.Id "IIR filter stabilization"
.Id "cepstral analysis"
.Id "filter, IIR, stabilization"
.pp
Given a filter
.EQ
H(z) ~=~ { sum from n=0 to q b(n) z sup -n } over
{sum from n=0 to p a(n) z sup -n } ~=~ B(z) over A(z) ~,
.EN
this demo serves as a tool to determine whether the filter is
stable.  If the filter is not stable, we wish to find a denominator
$A sub s (z)$ that is causal and stable and
is equal to $|A(z)|$ on the unit circle.
The filter
$H sub s (z) ~=~ B(z)/A sub s (z)$ will
therefore evaluate to the same values as $H(z)$ on the unit circle.
Hence, in some sense, it has the frequency response that the unstable
filter should have had.
.pp
This problem depends only on the $a(n)$ (we assume that roots
of $A(z)$ and $B(z)$ do not cancel, so only the parameters $a(n)$
are provided as inputs.  We have to find
the minimum phase signal $a sub s (n)$ that has the same magnitude
spectrum as $a (n)$.
.pp
The demo has a "universe parameter",
.i ncoefs.
The parameter
.i ncoefs
should be set greater than or equal to the number
of coefficients in $a(n)$ (including the 0'th coefficient).
Since 256-point FFT's are used, for good results
.i ncoefs
should be a good deal smaller than this.
.pp
The input coefficients are supplied as the
.i value
parameter of the WaveForm star on the far left.  To read the
parameters from a file instead, give "< filename" as the
parameter value.  The values are the coefficients of $a(n)$,
starting with $a(0)$.
.pp
The cepstrum of the autocorrelation function is computed and
plotted.
This signal is multiplied by a window function.  This
window function should have value 1/2 for $n ~=~ 0$, unity for $n ~>~ 0$,
and zero for $n ~<~ 0$, where $n$ is the time index;
however, the cepstrum "wraps
around" (negative time appears above positive time because of
the periodicity of the DFT)
so the application must compensate for this.  The resulting signal is
the cepstrum of a minimum phase signal $a hat sub s(n)$, and
it can be shown that the autocorrelation of $a sub s(n)$ is
the same as the autocorrelation of $a(n)$.  So we convert it
from the cepstrum domain back to the time domain to obtain
our result.
.pp
The original signal is plotted in red; the modified signal is
plotted in green.  If the original signal was minimum phase
(equivalently, if the original filter was stable) you'll see
only one plot.
.UH "REFERENCES"
.ip [1]
J. Lim,
.i "Two-dimensional signal and image processing" ,
Prentice-Hall, Englewood Cliffs, NJ, 1990, pp. 298-300
.SA
WaveForm, ComplexFFT
.ES
