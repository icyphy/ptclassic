.\" @(#)adpcmMissTrack 1.1 10/20/92
.NA "ADPCMMissTrack"
.SD
This demonstrations represents the pitfalls of
using a simple voice-recovery technique such as
zero-sample substitution in an ADPCM packet-switched
network. 
.SE
.LO "~ptolemy/src/domains/de/demo"
.SV 1.1 10/20/92
.AL "GSWalter."
.LD
This demonstrations reads 8 kHz voice samples and converts
them into a 32 kbps stream using Adaptive Differential
Pulse Code Modulation (ADPCM).  The ADPCM coding is
implemented using an
.c LMS
star.  The bits in the coder output stream are loaded into a
.c SeqATMCell
in which sequence numbers will be set using a modulo-8
numbering scheme.  The
.c SeqATMCells
are then loaded into
.c NetworkCells
for transport over the network.  During transmission over
the network, it is possible that some cells may be dropped
by the 
.c LossyInput
galaxy.  A zero-sample substitution method is used to fill
in for the dropped cells.  This causes mistracking to occur
between the coder and the decoder since transmitted codewords
are determined sequentially.  This mistracking is often
fatal and will cause the decoder to become unstable.  A 
.c limit 
star has been used in the decoder to keep unstable samples
from reaching infinity.  A graphical output shows a comparison
between the quantization-error power and the output-error
power.  Given no lost cells, these curves would be identical.
However, the output-error curve is likely to be headed
for infinity if a cell is dropped.
.pp
The default parameters in this demonstration are set so that
100 samples are encoded every iteration which will fill
approximately 1.04 cells.  Running the demonstration for 290
iterations will produce 302 cells consuming about 29,000
voice samples.
.SA
.c EmbedADPCM
.ES
