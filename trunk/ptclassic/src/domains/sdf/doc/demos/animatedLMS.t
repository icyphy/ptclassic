.\" $Id$
.NA animatedLMS
.SD
An LMS adaptive filter is configured as in the
.c adaptFilter
demo, but this time the filter taps are displayed as they adapt.
.EQ
delim off
.EN
.LO "$PTOLEMY/src/domains/sdf/demo"
.DM SDF Universe
.SV $Revision$ $Date$
.AL "Edward A. Lee"
.EQ
delim $$
.EN
.LD
A Gaussian white noise signal serves as input to an FIR filter and
an adaptive filter using the LMS algorithm.
The outputs of the two filters are subtracted, and the power of the
resulting error signal is measured and displayed.
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
