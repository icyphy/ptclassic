.\" @(#)embedADPCM 1.1 10/20/92
.NA "EmbedADPCM"
.SD
This demonstration shows how embedded-ADPCM with
LSB dropping may be used in an ATM network to
prevent mistracking between the coder and decoder.
.SE
.LO "~ptolemy/src/domains/de/demo"
.SV 1.1 10/20/92
.AL "GSWalter."
.LD
This demonstration reads 8 kHz voice samples and converts
them into a 32 kbps bit stream using Embedded Adaptive Differential
Pulse Code Modulation (ADPCM).  Embedded ADPCM uses the 
.c LMS
star to determine the difference signal which should be
quantized and sent over the network just as regular ADPCM.  The
difference between ADPCM and Embedded ADPCM is that the latter
determines its predictor coefficients and filter
taps with only the knowledge
of the two MSBs in the encoded-error signal.  That is, with
the embedded algorithm, the 16-level encoded-error signal
is requantized to only four levels before feeding into the
.c LMS
star.  The decoder is configured in the same manner as the 
coder.  
.pp
Before the 4-bit error signal is sent over the network, it
is packetized into
.c SeqATMCells
such that the two MSBs from each sample are loaded into
separate cells than the LSBs.  Cells which carry MSBs are
labeled with priority 1 and cells with LSBs are labeled
with priority 0 upon being loaded into
.c NetworkCells.
The network is instructed to drop only priority 0
packets if congestion should occur.  Hence, only LSBs will
be lost during transmission.  This allows the predictor
coefficients in the decoder to exactly track those in the
encoder thereby avoiding mistracking which could ultimately
cause the decoder to become unstable.
.pp
The default parameters in this demonstration are set so that
100 samples are encoded every iteration which will fill
approximately 1.04 cells.  Running the demonstration for 290
iterations will produce 302 cells consuming about 29,000
voice samples.
.UH REFERENCES
Suzuki, J. and Taka, M., "Missing Packet Recovery Techniques
for Low-Bit-Rate Coded Speech," \fIIEEE J. on Sel. Areas in
Comm.\fR, vol. 7, no. 5, pp. 707 - 717, June 1989.
.SA
.c ADPCMMissTrack
.ES


