.\" $Id$
.NA allPole
.SD
A universe showing two ways to implement an all-pole filter.
.DE
.SV $Revision$	"%D%"
.AL "E. A. Lee"
.LO "~ptolemy/src/domains/sdf/demo"
.LD
A noise signal is fed into two realizations
of an all pole filter.
.Ie "allpole filter"
.Ie "filter, allpole"
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
star, and are identical to the coefficients
of the FIR filter.
The output of the two filters is identical.
The number of samples run is determined in
part by the \fIblockSize\fR parameter of the
.c BlockAllPole
star.
.SA
BlockAllPole,
FIR
.ES
