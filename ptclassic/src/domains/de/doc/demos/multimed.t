.\" $Id$
.NA multimed
.SD
Shows a network transporting video, audio, and other data.
.DE
.SV $Revision$ 11/9/92
.AL "Paul Haskell."
.LD
This demo combines data from several types of sources into a single
network.
Within the network, the various data streams are routed from
switch to switch
(also see \fISwitch4x4\fR).

One video source and one audio source are decoded and played;
other data streams are eventually discarded.

Data losses may occur within the network which will affect the
final quality of the presented video and audio streams.
.ES
