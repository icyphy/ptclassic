.\" $Id$
.NA interp
.SD
This system uses an FIR filter to upsample by a factor of 8
and linearly interpolate between samples.
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
.AL "Edward A. Lee"
.LD
The polyphase multirate capabilities of the
.c FIR
filter star are exploited here.
.Ie "interpolation, linear"
.Ie "linear interpolation"
.Ie "sample rate conversion"
.Se FIR
.SA
FIR.
.ES
