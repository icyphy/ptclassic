.\" $Id$
.NA analytic
.SD
Analytic sample rate conversion.
.DE
.Se ComplexFIR
.SV $Revision$ "October 15, 1990"
.AL "Edward A. Lee"
.LO "~ptolemy/src/domains/sdf/demo"
.LD
This system uses a ComplexFIR filter to reduce the sample rate
of a sinusoid by a factor of 8/5, and at the same time
produce a complex, analytic, signal.
.Ie "ComplexFIR"
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
