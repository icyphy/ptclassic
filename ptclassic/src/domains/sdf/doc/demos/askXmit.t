.\" $Id$
.NA askXmit
.SD
Synthesize an amplitude-shift keyed (ASK) signal
with a 100% excess bandwidth raised cosine pulse.
.DE
.LO "$PTOLEMY/src/domains/sdf/demo"
.DM SDF Universe
.SV $Revision$ $Date$
.AL "Edward A. Lee"
.LD
An amplitude-shift keyed (ASK) signal is generated by
the
.c ask
galaxy on the left.
.Id "amplitude shift keying"
.Id "modulation, ASK"
The modulation format is a simple, baseband, binary-antipodal signal
with a 100% excess bandwidth raised cosine pulse.
.Ir "raised cosine pulse"
The sample rate is eight times the baud rate.
.UH REFERENCES
.ip 1
Simon Haykin, \fICommunication Systems\fR,
Wiley & Sons, 3rd ed., 1994, ISBN 0-471-57176-8.
.ES
