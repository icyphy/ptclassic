.\" $Id$
.NA reverb
.SD
Reverberation demo.
.DE
.SV 1.1 "November 24, 1992"
.AL "Chih-Tsung Huang (ported from Gabriel)"
.EQ
delim off
.EN
.LO "$PTOLEMY/src/domains/cg56/demo/s56x"
.EQ
delim $$
.EN
.LD
.pp
.Id "reverberation system"
This system implements a reverberation system by feeding
the input into a bulk delay, scaling the delayed signal,
and adding it back in before the delay.
.ES
