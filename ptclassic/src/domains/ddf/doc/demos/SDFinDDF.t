.\" $Id$
.NA SDFinDDF
.SD
Demonstrates a DDF Wormhole whose inside domain is SDF.
.DE
.SV $Revision$ $Date$
.AL "E. A. Lee"
.LD
This rather trivial demo illustrates the use of a wormhole.
.Ie "wormhole, SDF in DDF"
The DDF domain has an \fIif-then-else\fR overall structure, consisting
of a pair of
.c Case
and
.c EndCase
stars.
.Se Case
If the \fIcontrol\fR boolean is FALSE, the input token to the
.c Case
star routes to the false output(output#1) port, then the
wormhole, and finally to the false input(input#1)
port of the
.c EndCase
star.  Otherwise, the wormhole is bypassed.  The inside domain of
the wormhole is SDF.  In the wormhole, the input value
is multiplied by another ramp.
Note that the schedule inside the wormhole is constructed at compile-time.
On the other hand, the outside DDF domain uses dynamic scheduling,
deciding on runnable blocks at runtime.
Using wormholes for SDF systems is recommended, since the SDF scheduler
runs considerably faster than the DDF scheduler.
.SA
Case,
EndCase.
.ES
