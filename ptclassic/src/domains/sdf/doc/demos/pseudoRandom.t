.\" $Id$
.NA pseudoRandom
.SD
Generate a pseudoRandom sequence of zeros and ones using a maximal-length shift register.
.DE
.LO "$PTOLEMY/src/domains/sdf/demo"
.DM SDF Universe
.IE psuedoRandom
.SV $Revision$	$Date$
.AL "Edward A. Lee"
.LD
.Ir "Pseudo-random sequences"
A maximal-length shift register is a delay line whose input is computed as the
exclusive or of a set of taps from the delay line [1].
The
.c WaveForm
star feeds an initial sequence into the delay line.
.UH Reference
.ip [1]
E. A. Lee and D. G. Messerschmitt, \fIDigital Communication\fR, Second Edition,
Kluwer Academic Publishers, Norwood, MA, 1994.
.ES
