.\" $Id$
.NA DpcmImage
.SD
A universe that does differential pulse code modulation (DPCM).
.DE
.SV $Revision$ "August 19, 1991"
.AL "P. E. Haskell"
.LO "~ptolemy/src/domains/sdf/demo"
.LD
.Ie "image DPCM"
.Ie "DPCM, image"
This universe reads images from a file and performs DPCM compression
on the image sequence.
The difference-coded image sequence is run-length encoded.
.Ie "run length encoding"
.Ie "encoding, run length"
The coded image, final image, and difference
between the original and final images are all displayed.
.SA
DisplayImage
ReadImage
Dpcm
RunLen
.ES
