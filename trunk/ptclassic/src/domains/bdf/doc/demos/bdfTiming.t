.\" $Id$
.NA bdfTiming
.SD
This demo is identical to the DDF timing demo, except
that it uses BDF
.c Switch
and
.c Select
stars instead of DDF
.c Case
and
.c EndCase
stars.
The static schedule has some simple if-then constructs to
implement conditional firing.
.DE
.EQ
delim off
.EN
.LO "$PTOLEMY/src/domains/bdf/demo"
.EQ
delim $$
.EN
.SV $Revision$ $Date$
.AL "J. T. Buck"
.Ie "timing recovery, MMSE"
.Ie "MMSE timing recovery"
.Ir "asynchronous signal processing"
.SA
Select,
Switch
timing.
.ES
