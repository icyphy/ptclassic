.\" $Id$
.NA interp
.SD
Linear interpolation.
.SV $Revision$ $Date$
.AL "E. A. Lee"
.EQ
delim off
.EN
.LO "$PTOLEMY/src/domains/cgc/demo"
.EQ
delim $$
.EN
.LD
.Ie "interpolation, linear"
.Ie "linear interpolation"
.Ie "sample rate conversion"
This universe is identical to the
.c interp
demo in the SDF domain.
This system uses an FIR filter to upsample by a factor of 8
and linearly interpolate between samples. 
.Se FIR
.SA
FIR.
.ES
