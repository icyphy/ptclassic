.\" $Id$
.NA ifThenElse
.SD
If-then-else construct in the SDF Wormhole.
.DE
.SV 1.1 "September 27, 1990"
.AL "E. A. Lee"
.LD
.ES
.IE "if-then-else"
This demo illustrates the usage of a SDF wormhole to implement
a dynamic construct with DDF domain.
In the SDF domain, a dynamic construct is not allowed since
the number of tokens to be produced or consumed is not
deterministic in the dynamic construct.  In this demo,
a typical example of the dynamic construct (if-the-else) is
encapsulated in the wormhole.  The wormhole itself behaves
exactly like a SDF star : consumes one token from each input
and produces one output token.  
Inside the wormhole, the DDF domain has the \fIif-then\else\fR 
construct which consists of a pair of the
.c Switch
and the
.c Select
stars.  The false arm and the true arm have different gain factors :
1.0 and 2.0 respectively.  The control boolean is provided by
the threshold device, which remains true once the input value
exceeds 0.5.  Thus, during the first 6 inputs, the false arm is
selected. Afterwards, the true arm is selected.
.SA
.ES

