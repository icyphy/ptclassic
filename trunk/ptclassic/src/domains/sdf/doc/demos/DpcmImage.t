.\" $Id$
.NA DpcmImage
.SD
 A universe that performs differential pulse code modulation (DPCM)
on an image sequence.
.DE
.LO "~ptolemy/src/domains/sdf/demo"
.DM SDF Universe
.SV $Revision$	"August 19, 1991"
.AL "P. E. Haskell"
.LD
.Ie "image DPCM"
.Ie "DPCM, image"
.Ie "PCM"
 This universe reads images from a file and performs DPCM compression
on the image sequence.
 The difference-coded image sequence is run-length encoded.
.Ie "run-length encoding"
.Ie "encoding, run-length"
.Ie "image coding"
 The coded image, final image, and difference between the original and
final images are all displayed.
.pp
 The DisplayVideo star needs programs from the Utah Raster Toolkit
.Ir "Utah Raster Toolkit"
.EQ
delim off
.EN
to be in your $path variable to work.
.EQ
delim $$
.EN
 These programs are not included with Ptolemy.
 The manual page for the SDF DisplayVideo star tells how to get the
Utah Raster Toolkit for free.
.SA
DisplayVideo
ReadImage
Dpcm
RunLen
.ES
