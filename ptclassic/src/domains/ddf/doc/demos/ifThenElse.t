.\" $Id$
.NA ifThenElse
.SD
If-then-else construct in an SDF Wormhole.
.DE
.SV $Revision$ $Date$
.AL "E. A. Lee"
.LO "~ptolemy/src/domains/ddf/demo"
.LD
.Ir "if-then-else"
This demo illustrates the use of an SDF wormhole to implement
a dynamically scheduled construct using the DDF domain.
An if-then-else is such a dynamically scheduled construct.
In the SDF domain, such dynamic constructs are not allowed since
the number of tokens to be produced or consumed by each star is not
deterministic in the dynamic construct.  In this demo,
the if-then-else is
encapsulated in a wormhole.  The wormhole itself behaves
exactly like a SDF star: it consumes one token from each input
and produces one output token.  
Inside the wormhole, however, the DDF domain schedules the stars
dynamically to conditionally invoke the "then" and "else" clauses.
This is done using
a pair of
.c Case
and
.c EndCase
stars.
.Se Case
.Se EndCase
The \fIfalse\fR and \fItrue\fR arms have different gain factors:
1.0 and 2.0 respectively.  The \fIcontrol\fR boolean is provided by
the threshold device, which remains true once the input value
exceeds 0.5.  Thus, for the first 6 inputs, the false arm is
invoked.  Afterwards, the true arm is invoked.
.SA
Case,
EndCase.
.ES
