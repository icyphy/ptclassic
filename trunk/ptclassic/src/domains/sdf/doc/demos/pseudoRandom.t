.\" $Id$
.NA pseudoRandom
.SD
Generate a pseudoRandom sequence of zeros and ones
.DE
.LO "~ptolemy/src/domains/sdf/demo"
.DM SDF Universe
.IE psuedoRandom
.SV $Revision$	$Date$
.AL "E. A. Lee"
.LD
.Ir "Pseudo-random sequences"
This universe generates a psuedo-random sequence using a maximal-length
shift register.  The
.c WaveForm
star feeds an initial sequence into the delay line.
.ES
