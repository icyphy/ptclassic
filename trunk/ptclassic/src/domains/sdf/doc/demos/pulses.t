.\" $Id$
.NA pulses
.SD
Generate raised cosine and square-root raised cosine pulses.
.DE
.LO "~ptolemy/src/domains/sdf/demo"
.DM SDF Universe
.SV $Revision$	$Date$
.AL "E. A. Lee"
.LD
.pp
This demo generates a 25% excess bandwidth raised cosine pulse
using the
.c RaisedCosine
star.
.Se RaisedCosine
.Ie "raised cosine pulses"
.Ie "square-root raised cosine pulses"
It also generates a 25% excess bandwidth square-root raised cosine
pulse, and then filters it with an identical pulse (the matched filter).
.Ir "matched filter"
The cascade of the two filters yields a 25% excess bandwidth raised
cosine pulse.
.pp
Cascading two square-root raised cosine pulses will not always yield
exactly the expected raised cosine pulse.  The reason is that the
pulses are truncated in time.  This effect is particularly noticeable
at low excess bandwidths.
.UH "REFERENCES"
.ip [1]
E. A. Lee and D. G. Messerschmitt,
.i "Digital Communication" ,
Kluwer Academic Publishers, Boston, MA, 1988, pp. 154-172
.SA
RaisedCosine
.ES
