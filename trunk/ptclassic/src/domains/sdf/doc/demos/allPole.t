.\" $Id$
.NA allPole
.SD
Two realizations of an all-pole filter are shown to be equivalent.
One uses an
.c FIR
filter in a feedback path, and the other uses the
.c BlockAllPole
star.
.DE
.LO "$PTOLEMY/src/domains/sdf/demo"
.DM SDF Universe
.SV $Revision$	"%D%"
.AL "Edward A. Lee"
.LD
A noise signal is fed into two realizations of an all pole filter.
.Ie "all-pole filter"
.Ie "filter, all-pole"
The lower one uses an
.c FIR
filter in a feedback loop.
The upper one uses the
.c BlockAllPole
star.
.Se BlockAllPole
The coefficients for the
.c BlockAllPole
star are supplied by the
.c WaveForm
star, and are identical to the coefficients of the FIR filter.
The outputs of the two filters are identical.
The number of samples run is determined in
part by the \fIblockSize\fR parameter of the
.c BlockAllPole
star.
.SA
BlockAllPole,
FIR.
.ES
