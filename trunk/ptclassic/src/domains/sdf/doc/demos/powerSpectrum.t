.\" $Id$
.NA powerSpectrum
.SD
Compare three methods for estimating a power spectrum.
.DE
.LO "~ptolemy/src/domains/sdf/demo"
.DM SDF Universe
.SV $Revision$ $Date$
.AL "E. A. Lee"
.LD
.Ir "Levinson-Durbin algorithm"
.Ir "Burg's algorithm"
.Ir "system identification"
.Ir "power spectrum"
This demo generates a random process that is a real-valued version
of the test process described in [2] (page 11).
It consists of three sinusoids, two of which are closely spaced,
and colored Gaussian noise.
The power spectrum of this process is estimated using three
methods, the periodogram,
.Ir periodogram
the autocorrelation method
.Ir "autocorrelation method"
(which uses the Levinson Durbin algorithm),
and the Burg method.
Since the input process is Gaussian,
the latter two methods
produce approximate \fImaximum entropy\fR spectral estimates.
.Ir "maximum entropy spectral estimation"
If the autocorrelation of the input process were known
exactly, then these methods would produce exact maximum entropy spectra.
However, the autocorrelation is estimated from observations
of the input, so the two methods yield slightly different results.
.pp
The universe parameters are \fIlog2NumInputs\fR,
the log base 2 of the number of input samples to generate, and \fIorder\fR,
the order of the AR model to use in the autocorrelation and Burg's method
estimates.
.pp
The periodogram method amounts to computing a direct DFT of the observations
of the input process.  The number of observations used is 512.
This type of power spectrum estimate has very high variance,
and increasing the number of observations does not reduce the variance.
This high variance can be observed in the extreme jaggedness
of the output plot.
.pp
The autocorrelation method first estimates the autocorrelation
of the input from the observations.  Per the classical technique,
a biased estimate is used (see references).
Based on this autocorrelation estimate, the parameters of
an all-pole filter that could have produced the observations given
a white noise input are estimated using the Levinson-Durbin algorithm.
The transfer function of the all-pole filter is:
.EQ
H(z) ~=~ 1 over {1 ~+~ sum from n=1 to N d sub n z sup -n }
.EN
The
.c LevDur
star outputs the $d sub n$ parameter estimates, also called the AR parameters.
The autocorrelation method power spectrum estimate is simply
.EQ
| H ( e sup {j omega } ) | sup 2
.EN
so the rest of the autocorrelation galaxy is devoted to computing
this quantity at various values of $omega$.  The number of values
of $omega$ to use (512, in this case) is specified by the \fIresolution\fR
parameter of the galaxy.
The final output is scaled by an estimate of the power of the input
process, extracted from the autocorrelation estimate using the
.c Cut
star.
The
.c Repeat
star is needed to maintain consistent sample rates.
The
.c FloatPad
star is used to prepend the AR coefficients with a one, the value of
the first coefficient, which need not be computed.
.Se FloatPad
.pp
The Burg method does not require first estimating the autocorrelation.
It estimates the AR parameters directly from the input samples.
The parameters are then processed exactly as above to produce
a spectral estimate.
.UH "REFERENCES"
.ip [1]
J. Makhoul, "Linear Prediction: A Tutorial Review",
\fIProc. IEEE\fR, Vol. 63, pp. 561-580, Apr. 1975.
.ip [2]
S. M. Kay, \fIModern Spectral Estimation: Theory & Application\fR,
Prentice-Hall, Englewood Cliffs, NJ, 1988.
.ip [3]
S. Haykin, \fIModern Filters\fR, MacMillan Publishing Company,
New York, 1989.
.SA
Burg,
LevDur,
levinsonDurbin,
linearPrediction.
.ES
