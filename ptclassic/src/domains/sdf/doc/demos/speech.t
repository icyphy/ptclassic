.\" $Id$
.NA speech
.SD
This system reads a speech signal from a file, and encodes
it at two bits per sample using adaptive differential pulse
code modulation (ADPCM) with a feedback-around-quantizer structure.
It then reconstructs the signal from the quantized data.
The original and reconstructed speech are played over the
workstation speaker.
.DE
.LO "$PTOLEMY/src/domains/sdf/demo"
.DM SDF Universe
.SV $Revision$	$Date$
.AL "Tom Parks"
.LD
Unlike commercial ADPCM systems, there is no adaptive
quantizer in this implementation.
.SA
LMS,
Play.
.ES
