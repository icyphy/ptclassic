.\" $Id$
.NA convolve
.SD
Convolve two rectangular pulses in order to demonstrate the
use of the Convolve block.
.DE
.LO "$PTOLEMY/src/domains/sdf/demo"
.DM SDF Universe
.SV $Revision$ $Date$
.AL "Kennard White"
.LD
This trivial system uses two
.c Rect
blocks to generate rectangular pulses, and then convolves them.
Note that the \fItruncationDepth\fR parameter of the
.c Convolve
block must be set larger than the number of output samples of interest.
.SA
Convolve,
Rect.
.ES
