.\" $Id$
.NA DpcmImage
.SD
A universe that does differential pulse code modulation (DPCM).
.DE
.IE DpcmImage
.SV $Revision$ "August 19, 1991"
.AL "P. E. Haskell"
.LO "~ptolemy/src/domains/sdf/demo"
.LD
This universe reads images from a file and performs DPCM compression
on the image sequence.
The difference-coded image sequence is run-length encoded.
The coded image, final image, and difference
between the original and final images are all displayed.
.SA
DisplayImage
ReadImage
DpcmImage
RunLen
.ES
