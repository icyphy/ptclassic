.\" $Id$
.NA pseudoRandom
.SD
Generate a pseudo-random sequence of zeros and ones using a
maximal-length shift register.
.EQ
delim off
.EN
.LO "$PTOLEMY/src/domains/sdf/demo"
.DM SDF Universe
.IE psuedoRandom
.SV $Revision$	$Date$
.AL "Edward A. Lee"
.EQ
delim $$
.EN
.LD
.Ir "Pseudo-random sequences"
A maximal-length shift register is a delay line whose input is computed as the
exclusive or of a set of taps from the delay line [1].
The
.c WaveForm
star feeds an initial sequence into the delay line.
Psuedo-random sequences are used to implement direct sequence spreading,
as is used in spread-spectrum communications [2].
.UH REFERENCES
.ip [1]
E. A. Lee and D. G. Messerschmitt, \fIDigital Communication\fR, Second Edition,
Kluwer Academic Publishers, Norwood, MA, 1994.
.ip [2]
Simon Haykin, \fICommunication Systems\fR,
Wiley & Sons, 3rd ed., 1994, ISBN 0-471-57176-8.
.ES
