.\" $Id$
.NA MotionComp
.SD
A universe that does motion compensation image coding.
.EQ
delim off
.EN
.LO "$PTOLEMY/src/domains/sdf/demo"
.DM SDF Universe
.SV $Revision$	$Date$
.AL "Paul E. Haskell"
.EQ
delim $$
.EN
.LD
This universe reads images from a file and performs motion compensation
on the image sequence.
.Ie "motion compensation"
.Ie "image, motion compensation"
.Se MotionCmp
.Se RunLen
The difference-coded sequence is run-length encoded.
.Ie "run length encoding"
.Ie "encoding, run length"
The final image is displayed.
.SA
DisplayImage,
ReadImage,
MotionCmp,
RunLen.
.ES
