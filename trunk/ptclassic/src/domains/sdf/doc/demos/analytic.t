.\" $Id$
.NA analytic
.SD
This system uses a ComplexFIR filter to reduce the sample
rate of a sinusoid by a factor of 8/5, and at the same
time produce a complex approximately analytic signal
(one that has no negative frequency components).
.DE
.LO "$PTOLEMY/src/domains/sdf/demo"
.DM SDF Universe
.Se ComplexFIR
.SV $Revision$ "October 15, 1990"
.AL "Edward A. Lee"
.LD
.Ir "analytic signal"
.Ir "sample rate conversion"
The magnitude spectrum of the original sinusoid and the filtered
sinusoid are both displayed.
Note the shift in the peak of the spectrum due to the sample-rate
conversion.  Also note that the symmetry of the original spectrum
is lost in the filtered spectrum.
.SA
ComplexFIR.
.ES
