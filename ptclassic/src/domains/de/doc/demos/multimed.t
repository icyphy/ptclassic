.\" $Id$
.NA multimed
.SD
This demo feeds data from several types of sources into a single network.
Within the network, the various data streams are routed from switch to switch.
One video stream and one audio stream are decoded and played.
Data losses may occur within the network and reduce the final quality
of the presented video and audio streams.
The video display requires that the Utah Raster Toolkit be installed
and available in the user's path.
.DE
.LO "$PTOLEMY/src/domains/de/demo"
.SV $Revision$ $Date$
.AL "Paul Haskell and Greg Walter."
.LD
.Ir "video processing"
.Ir "speech processing"
Every iteration, three video frames and 800 speech samples are processed.
The input video sequence originally was sampled at 30 frames per second, and
the input audio sequence was sampled 8000 times per second.
Thus, each iteration of the simulation corresponds to one tenth
of a second of real-time.
.pp
The
.c PseudoCell
stars feed empty cells into the network whenever the
.c Poisson
stars attached to their inputs fire.
These empty cells simulate data traffic within the network.
.pp
The video and audio coders are taken from other Ptolemy demonstrations.
The video coder combines motion compensation, the discrete
cosine transform (DCT), and run-length coding to compress
the video from 8 bits per pixel to about 1 bit per pixel.
.Ir "DCT"
The audio coder uses simple PCM coding to cut the audio bit-rate in half.
.Ir "PCM"
Users may wish to experiment with other network configurations,
other distributions for the data traffic, or other types of traffic altogether.
.UH "Necessary Software"
The audio output is sent to a program called "play", which should
be able read mu-law encoded audio and play the audio over a speaker.
Sun SparcStations typically contain this program.
The video display galaxy requires "getx11" and several other
programs from the Utah Raster Toolkit.
This toolkit is not included with Ptolemy, but it is available free
of charge via anonymous ftp.
See the manual page for the SDF DisplayVideo star to find out how to
obtain the Utah Raster Toolkit.
.Ir "Utah Raster Toolkit"
The "play" program and the Utah Raster Toolkit programs must be in the
user's search-path for the audio and video data to be output.
.SA
DisplayVideo,
PseudoCell,
Switch4x4.
.ES
