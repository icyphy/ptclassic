.\" $Id$
.NA overlapAdd/FFT
.SD
Convolution is implemented in the frequency domain using overlap and add.
.DE
.EQ
delim off
.EN
.LO "$PTOLEMY/src/domains/sdf/demo"
.EQ
delim $$
.EN
.DM SDF Universe
.SV $Revision$	$Date$
.AL "E. A. Lee"
.LD
A white noise signal serves as input to an FIR filter that directly implements
convolution in the time domain.
The output of this filter is then compared against a frequency domain
implementation using an overlap-and-add approach.
The implementation begins with a complex FFT.
The output of the
.c FFTCx
star is multiplied by the pre-computed FFT of the impulse response of the
.c FIR
filter.
The
.c overlapAdd
galaxy then uses the 
.c Chop
star to implement the overlapping.
.Ie "frequency domain convolution"
.Ie "convolution, frequency domain"
.Ie "overlap and add convolution"
.Ie "FFT"
.SA
Chop,
FFTCx,
FIR.
.ES
