.\" $Id$
.NA MC_DCT
.SD
A universe that does motion compensation and DCT image coding.
.DE
.SV $Revision$ "November 12, 1992"
.AL "P. E. Haskell"
.LO "~ptolemy/src/domains/sdf/demo"
.LD
This universe reads images from a file and performs motion compensation,
the discrete cosine transform (DCT), zig-zag scanning of the DCT
coefficients, and run-length coding of the DCT coefficients.
.Ie "motion compensation"
.Ie "image, motion compensation"
.Ie "discrete cosine transform"
.Se MotionCmp
.Se DCTImage
.Se DCTImageCode
The final video sequence is displayed.
.pp
The DisplayVideo star needs programs from the Utah Raster Toolkit
to be in your $path variable to work.
These programs are not included with Ptolemy.
The manual page for the DisplayVideo star tells how to
get the Utah Raster Toolkit for free.
.SA
DisplayVideo
ReadImage
MotionCmp
DCTImage
DCTImageCode
.ES
