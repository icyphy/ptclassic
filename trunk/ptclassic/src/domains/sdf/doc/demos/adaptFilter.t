.\" $Id$
.NA adaptFilter
.SD
An LMS adaptive filter converges so that its transfer function
matches that of a fixed FIR filter.
.DE
.EQ
delim off
.EN
.LO "$PTOLEMY/src/domains/sdf/demo"
.EQ
delim $$
.EN
.DM SDF Universe
.SV $Revision$ "October 15, 1990"
.AL "Edward A. Lee"
.LD
A Gaussian white noise signal serves as input to an FIR filter and
an adaptive filter using the LMS algorithm.
The output of the two filters and the error are displayed
after the run.
By examining the parameters of the two filters, one can find
the filenames for the FIR filter coefficients and the initial tap values.
The final tap values can be saved by specifying a \fIsaveTapsFile\fR
in the adaptive filter star.
.Ie "adaptive filter"
.Ie "filter, adaptive"
.Se LMS
.Ie "LMS adaptive filter"
.SA
IIDGaussian,
LMS,
FIR.
.ES
