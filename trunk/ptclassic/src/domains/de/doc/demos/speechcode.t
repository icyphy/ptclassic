.\" $Id$
.NA speechcode
.SD
Speech compression
.DE
.SV $Revision$ $Date$
.AL "Rachel Bowers and Erick Hamilton"
.LD
This demo performs speech compression with a combination of
silence detection, adaptive quantization, and adaptive estimation.
After speech samples are read from a file, they are coded, packetized,
depacketized, decoded, and played on the workstation speaker.
Packets containing only silence are discarded; the receiver
detects discarded packets and replaces them with zero samples.
