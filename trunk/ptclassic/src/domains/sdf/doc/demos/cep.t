.\" $Id$
.NA cep
.SD
Stabilization of IIR filters using the cepstrum
.DE
.IE
.SV $Revision$ "November 12, 1990"
.AL "Joseph T. Buck"
.LO "~ptolemy/src/domains/sdf/demo"
.LD
Given a filter
.EQ
H(z) = { sum from n=0 to q b(n) z sup -n } over
{sum from n=0 to p a(n) z sup -n }
.EN
this demo serves as a tool to determine whether the filter is
stable.  If the filter is not stable, we wish to find a filter
$H sub s (z)$ that is causal and stable and $|A sub s (z)|$
is equal to $|A(z)|$ on the unit circle.

This problem depends only on the $a(n)$ (we assume that roots
of $A(z)$ and $B(z)$ do not cancel.  It is equivalent to finding
the minimum phase signal $a sub s (n)$ that has the same magnitude
spectrum as $a (n)$.

The demo has a "universe parameter",
.i ncoefs.
.i  ncoefs
should be set greater than or equal to the number
of coefficients in $a(n)$ (including the 0'th coefficient).
256-point FFT's are used; for good results,
.i ncoefs
should be a good deal smaller than this.

The input coefficients are supplied as the
.i value
parameter of the WaveForm star on the far left.  To read the
parameters from a file instead, give "< filename" as the
parameter value.  The values are the coefficients of $a(n)$,
starting with $a(0)$.

The cepstrum of the autocorrelation function is computed and
plotted.
This signal is multiplied by a window function.  This
window function is conceptually 1/2 for n = 0, 1 for positive
time, and zero for negative time; however, the cepstrum "wraps
around" so we compensate for this.  The resulting signal is
the cepstrum of a minimum phase signal $a hat sub s(n)$, and
it can be shown that the autocorrelation of $a sub s(n)$ is
the same as the autocorrelation of $a(n)$.  So we convert it
from the cepstrum domain back to the time domain to obtain
our result.

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
