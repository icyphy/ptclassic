.\" $Id$
.NA animatedLMSCx
.SD
A complex LMS adaptive filter is configured as in the adaptFilter demo,
but in addition, user-controlled noise is added to the feedback loop using
an on-screen slider to control the amount of noise.  The filter taps are
displayed as they adapt.
.DE
.LO "$PTOLEMY/src/domains/sdf/demo"
.DM SDF Universe
.SV $Revision$ $Date$
.AL "Edward A. Lee"
.LD
A Gaussian white noise signal serves as input to an
.c FIRCx
filter and
an adaptive filter using the LMS algorithm.
Real-valued Gaussian white noise is added to the feedback loop in an
amount controlled by an on-screen slider.  Note that the noise affects
only the real part of the taps.
The resulting error signal displayed using the
.c TkBarGraph
star.
.Se TkBarGraph
.Se TkScale
.Ie "adaptive filter"
.Ie "filter, adaptive"
.Se LMSCx
.Se FIRCx
.Ie "LMS adaptive filter"
.SA
adaptFilter,
animatedLMS,
IIDGaussian,
LMS,
FIR.
.ES
