.\" $Id$
.NA ColorImage
.SD
A universe that displays a color image.
.DE
.SV $Revision$ "August 19, 1991"
.AL "P. E. Haskell"
.LO "~ptolemy/src/domains/sdf/demo"
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
DisplayRgb
ReadRgb
Rgb2Yuv
Yuv2Rgb
.ES
