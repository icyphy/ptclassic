.\" $Id$
.NA interp
.SD
This system uses an FIR filter to upsample by a factor of 8
and linearly interpolate between samples.
.DE
.LO "~ptolemy/src/domains/sdf/demo"
.DM SDF Universe
.SV $Revision$ "October 15, 1990"
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
