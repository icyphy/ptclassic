.\" $Id$
.NA quantize
.SD
Demonstrate the use of the Quantizer star.
.DE
.LO "$PTOLEMY/src/domains/sdf/demo"
.DM SDF Universe
.SV $Revision$ $Date$
.AL "Edward A. Lee"
.LD
This trivial system shows how to use the
.c Quantizer
star.
.Se Quantizer
Three thresholds, at -1.0, 0.0, and 1.0 are used to produce
one of four levels, -1.5, -0.5, 0.5, 1.5.
The
.c FloatRamp
star is used to provide inputs over the range of interest.
.SA
Quantizer, Sgn.
.ES
