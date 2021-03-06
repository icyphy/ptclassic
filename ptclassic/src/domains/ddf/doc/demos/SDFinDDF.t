.\" $Id$
.NA SDFinDDF
.SD
This demo illustrates the use of a DDF wormhole whose inside
domain is SDF.
The top-level system (in the DDF domain) has an if-then-else
overall structure, implemented by a matching pair of
.c Case
and
.c EndCase
stars.
The inside system (in the SDF domain) multiplies the data value
by a ramp.
.EQ
delim off
.EN
.LO "$PTOLEMY/src/domains/ddf/demo"
.EQ
delim $$
.EN
.SV 1.6 12/16/92
.AL "E. A. Lee"
.LD
.Ie "wormhole, SDF in DDF"
.Se Case
The control for the if-then-else is generated by applying
a threshold test to a random sequence.
If the control Boolean is FALSE, the input particle to the
.c Case
star goes to the false output(output#1) port, which sends it
to the
wormhole, and finally to the false input(input#1)
port of the
.c EndCase
star.
Otherwise, the wormhole is bypassed. 
.pp
Note that the schedule inside the wormhole is constructed at compile-time,
while the outside DDF domain uses dynamic scheduling.
Using wormholes for SDF systems is recommended, since the SDF scheduler
runs considerably faster than the DDF scheduler when many repeated
runs are executed.
.SA
Case,
EndCase.
.ES
