.\" $Id$
.NA overlapAdd/FFT
.SD
Convolution implemented in the frequency domain using overlap and add.
.DE
.LO "$PTOLEMY/src/domains/sdf/demo"
.DM SDF Universe
.SV $Revision$	$Date$
.AL "E. A. Lee"
.LD
A white noise signal serves as input to an FIR filter that directly implements
convolution in the time domain.  The output of this filter is then compared
against a frequency domain implementation using overlap-and-add.
The implementation begins with a complex FFT.  The output of the FFTCx
star is multiplied by the pre-computed FFT of the impulse response of the
FIR filter.  The "overlapAdd" galaxy then uses the 
.c Chop
star to implement the overlapping.
.Ie "frequency domain convolution"
.Ie "convolution, frequency domain"
.Ie "overlap and add convolution"
.Ie "FFT"
.SA
FFTCx,
Chop,
FIR.
.ES
