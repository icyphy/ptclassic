.\" $Id$
.NA window
.SD
Generate and display four window functions and the magnitude
of their Fourier transforms.
The windows displayed are the Hanning, Hamming, Blackman, and steep Blackman.
.DE
.LO "$PTOLEMY/src/domains/sdf/demo"
.DM SDF Universe
.SV $Revision$ $Date$
.AL "Kennard White"
.LD
This system uses the
.c DTFT
block to compute the magnitude of the Fourier transform.
This will be slower than using the
.c FFTCx
but the frequency range over which the computation is done
can be customized.
In this case, only the positive frequencies are computed.
.SA
DTFT,
FFTCx,
Window.
.ES
