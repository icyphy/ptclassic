.\" $Id$
.NA multimed
.SD
A network that transports video, audio, and data.
.DE
.SV $Revision$ $Date$
.AL "Paul Haskell, Greg Walter."
.LD
This demo feeds data from several types of sources into a single
network.
Within the network, the various data streams are routed from
switch to switch
(also see
.c Switch4x4
).
One video stream and one audio stream are decoded and played.
Data losses may occur within the network which reduce the
final quality of the presented video and audio streams.

Every iteration, three video frames and 800
speech samples are processed.
The input video sequence originally was sampled at 30 frames per
second, and the input audio sequence was sampled 8000 times
per second.
Thus, each iteration of the simulation corresponds to one tenth
of a second of real-time.

The
.c PseudoCell
stars feed empty cells into the network whenever the
.c Poisson
stars attached to their inputs fire.
These empty cells are only used to load the network.

The video and audio coders are taken from other Ptolemy demonstrations.
The video coder combines motion compensation, the discrete
cosine transform, and run-length coding to compress
the video from 8 bits per pixel to about 1 bit per pixel.
The audio coder uses simple PCM coding to cut the audio bit-rate
in half.
.ES
