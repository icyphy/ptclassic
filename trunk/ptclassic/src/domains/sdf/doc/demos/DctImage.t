.\" $Id$
.NA dctimage
.SD
A universe that performs discrete cosine transform (DCT) coding
of an image sequence.
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
Dct,
DctInv,
DisplayImage,
ReadImage.
.ES
