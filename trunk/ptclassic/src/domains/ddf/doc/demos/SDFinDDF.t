.\" $Id$
.NA SDFinDDF
.SD
Demonstrates a DDF Wormhole whose inside domain is SDF.
.DE
.SV 1.1 "September 27, 1990"
.AL "E. A. Lee"
.LD
.ES
.IE Switch
This rather trivial demo illustrates the usage of a wormhole.
The DDF domain has the \fIif-then\else\fR structure which consists
of a pair of the
.c Switch
and the
.c Select
stars.  If the control boolean is FALSE, the input token to the
.c Switch
star routes to the falseOutput port, wormhole, and to the falseInput
port of the
.c Select
star.  Otherwise, the wormhole is skipped.  The inside domain of
the wormhole is SDF.  Through the wormhole, the input value
is multiplied by another ramp generator in the wormhole.
Note that the schedule inside the wormhole is done at compile-time.
On the other hand, the outside DDF domain uses the dynamic scheduling :
decides the runnable blocks at runtime.
For efficiency reason, it is recommended to use the wormholes (SDF
domain inside) in the DDF domain.
.SA
Switch.
.ES

