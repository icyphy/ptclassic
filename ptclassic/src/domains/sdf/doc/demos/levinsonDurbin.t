.\" $Id$
.NA levinsonDurbin
.SD
Use the Levinson Durbin algorithm to estimate the parameters of an AR process.
.DE
.SV $Revision$ "October 31, 1990"
.AL "E. A. Lee"
.LO "~ptolemy/src/domains/sdf/demo"
.LD
.IE LevDur
.IE "Levinson-Durbin algorithm"
.IE "system identification"
This demo generates an auto-regressive (AR) random process by filtering
Gaussian white noise with an all-pole filter.
Then it estimates the autocorrelation of the process
and uses the Levinson Durbin algorithm to estimate the parameters
of the process.
The transfer function of the all-pole filter is:
.EQ
H(z) ~=~ 1 over {1 ~+~ d sub 1 z sup -1 ~+~ d sub 2 z sup -2}
.EN
where in this case $d sub 1 ~=~ -0.9$
and $d sub 2 ~=~ 0.5$.
The \fIlp\fR output of the
.c LevDur
star is estimates of the negative of these parameters, as can be verified
using the plot produced by this demo.
These are called the autoregressive or linear predictor parameters.
Thus, if $F(z)$ is the Z-transform of the \fIlp\fR output of
the
.c LevDur
star, then
.EQ
G(z) ~=~ 1 ~+~ z sup -1 F(z)
.EN
is an estimate of the whitening filter,
or the filter that will convert the filtered noise sequence into
white noise.
.pp
The reflection coefficients are also plotted, but it is difficult
in this demo to verify that they are correct.
However, the fact that they become very small after the second one
is reassuring because the AR noise is second order.
.pp
The prediction error power is plotted as a function of the
predictor order.  The first sample corresponds to the zero-th
order predictor, so it is simply the estimate of the power of the
input signal.  The predictor error power fails to decrease further
when the order exceeds two, which again is reassuring because
of the order of the input process.
The last plot is the autocorrelation estimate used in teh Levinson-Durbin
algorithm.  Note that the center lag is the estimate of the power
of the input process, and hence is equal to the zero sample
of the prediction error power.
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
Autocor,
BiQuad,
IIDGaussian,
LevDur.
.ES
