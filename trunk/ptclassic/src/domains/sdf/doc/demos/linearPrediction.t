.\" $Id$
.NA linearPrediction
.SD
Two mechanisms for linear prediction are compared.
.DE
.LO "~ptolemy/src/domains/sdf/demo"
.DM SDF Universe
.SV $Revision$ "October 31, 1990"
.AL "E. A. Lee"
.LD
This demo performs linear prediction on a test signal consisting
of three sinusoids in colored, Gaussian noise.
.Ie "linear prediction"
.Ie "prediction, linear"
.Ie "system identification"
The first (upper) method uses Burg's algorithm and second (lower)
method uses an adaptive filter, with the LMS stochastic gradient
adaptation algorithm.
.Se LMS
.Ie "adaptive filter"
.Ie "filter, adaptive"
.Ie "filter, LMS adaptive"
.Ir "stochastic gradient algorithm"
The plots that are generated show the original signal, the predicted
signal, and the difference between these, or the prediction error.
.pp
Burg's method is a block method.
The
.c Burg
star collects 128 input samples, from which it estimates the autoregressive
.Se Burg
(AR) parameters of the input process.  These parameters are then loaded
into the
.c BlockFIR
star, which filters the input signal, delayed by one, in order
to construct the minimum mean-square-error prediction.
An initial transient, where the prediction is poor,
occurs because the tapped delay line of the
.c BlockFIR
needs to fill up with input samples before accurate predictions
can be made.
.Se BlockFIR
If the system is run for two iterations (two blocks), there
will be no transient at the start of the second block because
the tapped delay line is full.
.pp
The \fIblockSize\fR parameter of the
.c BlockFIR
star and the \fInumInputs\fR parameter of the
.c Burg
star must be equal.  Together they determine how many samples are
processed by each iteration of the overall schedule.  In this demo,
they are both 256, so 256 input samples are processed on each iteration.
Because of the relatively large number of samples plotted, you may
wish to zoom in on the plots for more detail.
.pp
The adaptive filter method is more dynamic than the Burg method.
Inputs are examined one-by-one
and the current FIR filter coefficients are used to predict the next
sample.  The difference between the prediction and the actual
signal is then fed back to the
.c LMS
star, which adjusts its FIR filter coefficients to try to improve
the next prediction.  The plots show that that the prediction is
not as good as that of the Burg method, and that the initial transient
is considerably longer, giving time for the LMS filter to converge.
However, the adaptive filter method may perform better on
some non-stationary signals, such as those with slowly varying statistics.
Furthermore, parameters like filter order, step-size, and the length of
the run can be varied to improve the performance.
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
LMS,
powerSpectrum.
.ES
