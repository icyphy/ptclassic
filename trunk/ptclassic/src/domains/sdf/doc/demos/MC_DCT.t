.\" $Id$
.NA MC_DCT
.SD
A universe that performs motion compensation and DCT encoding of video.
.DE
.LO "$PTOLEMY/src/domains/sdf/demo"
.DM SDF Universe
.SV $Revision$ $Date$
.AL "Paul E. Haskell"
.LD
This universe reads images from a file and performs motion compensation,
the discrete cosine transform (DCT), zig-zag scanning of the DCT
coefficients, and run-length coding of the DCT coefficients.
.Ie "motion compensation"
.Ie "image, motion compensation"
.Ie "discrete cosine transform"
.Ir "DCT"
.Ir "video processing"
.Se MotionCmp
.Se DCTImage
.Se DCTImageCode
The final video sequence is displayed.
.pp
The DisplayVideo star needs programs from the Utah Raster Toolkit
.EQ
delim off
.EN
to be in your $path variable to work.
.EQ
delim $$
.EN
These programs are not included with Ptolemy.
The manual page for the SDF \fIDisplayVideo\fR star tells how to
get the Utah Raster Toolkit for free.
.Ir "Utah Raster Toolkit"
.SA
DisplayVideo
ReadImage
MotionCmp
DCTImage
DCTImageCode
.ES
