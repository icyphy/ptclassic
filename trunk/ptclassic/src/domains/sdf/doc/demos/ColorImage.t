.\" $Id$
.NA ColorImage
.SD
A universe that converts an RGB color image to YUV and back,
and then displays it on the workstation screen.
.DE
.EQ
delim off
.EN
.LO "$PTOLEMY/src/domains/sdf/demo"
.EQ
delim $$
.EN
.DM SDF Universe
.SV $Revision$	$Date$
.AL "Paul E. Haskell"
.LD
.Ir "image, color"
.Ie "image format, RGB"
.Ie "image format, YUV"
.Se ReadRgb
.Se Rgb2Yuv
.Se Yuv2Rgb
This universe reads a color image in RGB format from a file.
The image is transformed into YUV color format, and then back
to RGB format.
Finally, the RGB image is displayed.
.SA
DisplayRgb,
ReadRgb,
Rgb2Yuv,
Yuv2Rgb.
.ES
