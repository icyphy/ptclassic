.\" $Id$
.NA MotionComp
.SD
A universe that does motion compensation image coding.
.DE
.IE MotionComp
.SV $Revision$ "August 19, 1991"
.AL "P. E. Haskell"
.LO "~ptolemy/src/domains/sdf/demo"
.LD
This universe reads images from a file and performs motion compensation
on the image sequence.
The difference-coded sequence is run-length encoded.
The final image is displayed.
.SA
DisplayImage
ReadImage
MotionCmp
RunLen
.ES
