.\" $Id$
.NA adaptFilter
.SD
A demo of the LMS adaptive filter star
.DE
.SV $Revision$ "October 15, 1990"
.AL "Edward A. Lee"
.LO "~ptolemy/src/domains/sdf/demo"
.LD
A Gaussian white noise signal serves as input to an FIR filter and
an adaptive filter using the LMS algorithm.
The output of the two filters and the error are displayed
after the run.
By examining the parameters of the two filters, you can find
the filenames for the FIR filter coefficients and the initial tap values.
The final tap values can be saved by specifying a saveTapsFile
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
