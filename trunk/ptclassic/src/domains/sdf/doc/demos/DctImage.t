.\" $Id$
.NA DctImage
.SD
 A universe that performs discrete cosine transform (DCT) coding
of am image sequence.
.DE
.LO "~ptolemy/src/domains/sdf/demo"
.DM SDF Universe
.SV $Revision$ "August 19, 1991"
.AL "P. E. Haskell"
.LD
 This universe reads an image from a file and performs DCT compression
on the image.
.Ie "DCT"
.Ie "image coding"
.Ie "discrete cosine transform"
.Se Dct
.Se DctInv
.Se DisplayImage
.Se ReadImage
 Next, the transform is inverted, and the result is displayed.
.SA
DisplayImage
ReadImage
Dct
DctInv
.ES
