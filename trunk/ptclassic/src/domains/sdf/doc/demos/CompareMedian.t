.\" $Id$
.NA CompareMedian
.SD
A universe that median filters an image to reduce artifacts
due to interleaved scanning of video sequences.
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
This universe reads an image from a file and median filters the image.
.Ie "filter, median"
.Ie "image, median filter"
.Ie "median filtering"
The original image, median filtered image, and difference
between the two are all displayed.
.Se DisplayImage
.Se ReadImage
.Se MedianImage
.SA
DisplayImage,
ReadImage,
MedianImage.
.ES
