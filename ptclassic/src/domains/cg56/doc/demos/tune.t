.\" $Id$
.NA tune
.SD
A tune generator using FM modulation.
.DE
.SV 1.1 "November 24, 1992"
.AL "Chih-Tsung Huang (ported from Gabriel)"
.LO "~ptolemy/src/domains/cg56/demo/sim"
.LD
.pp
.Id "tune generator"
This system produces DSP 56000 code for a program that
plays a sequence of notes given in a table.
It uses an FM modulation scheme.
.SH BUGS
The sounds produced are not particularly musically appealing,
partly because the modulation index is not variable and the attack and
decay profiles are too limited.
.ES
