.\" $Id$
.NA animatedLMS
.SD
A demo of the LMS adaptive filter star with animation
.DE
.LO "~ptolemy/src/domains/sdf/demo"
.DM SDF Universe
.SV $Revision$ $Date$
.AL "Edward A. Lee"
.LD
A Gaussian white noise signal serves as input to an FIR filter and
an adaptive filter using the LMS algorithm.
The output of the two filters are subtracted, and the power of the
resulting error signal is measured and displayed.
By examining the parameters of the two filters, you can find
.Ie "adaptive filter"
.Ie "filter, adaptive"
.Se LMS
.Ie "LMS adaptive filter"
.SA
adaptFilter,
IIDGaussian,
LMS,
FIR.
.ES
