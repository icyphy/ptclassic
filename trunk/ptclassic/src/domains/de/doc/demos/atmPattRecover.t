.\" @(#)atmPattRecover 1.1 10/10/92
.NA "ATMPattRecover"
.SD
This demonstration implements a voice-cell recovery
technique.  Cells which have been dropped are replaced
by samples determined through a complicated
pattern matching technique.
.SE
.LO "~ptolemy/src/domains/de/demo"
.SV 1.1 10/10/92
.AL "GSWalter."
.LD
This demonstration reads 8 kHz voice samples, converts
them into 64 kbps PCM using CCITT Recommendation G.711,
and then loads them into 
.c SeqATMCElls.
The
.c SeqATMCells
have sequence numbers which are set using a modulo-8
numbering pattern.  The loaded
.c SeqATMCells
are placed into the object
.c NetworkCell
which is used as the information carrier over the network.
During transmission, cells will be dropped with a probability
determined by the 
.c LossyInput
galaxy.  The 
.c SeqATMCells
are removed from the 
.c NetworkCells
at the receiver's side of the network.  Next, a star
.c PCMVoiceRecover
checks the sequence numbers to determine if a cell has
been dropped during transmission.  If a cell has been
dropped, a signal is sent to the galaxy
.c PatternMatch
asking for a replacement cell.  The replacement cell
is determined through the use of a pattern matching
method in which a portion of the previously arrived
samples are used as a template and a larger portion as
a window.  The template is slid over the window while
cross correlations are computed during each increment.
Enough samples are selected to fill
the missing cell.  These samples
are chosen beginning at the point
just following the last sample
in the window which was part of
the "best match" with the template.
They are then packetized and sent
back to the 
.c PCMVoiceRecover
star where they are sent to the \fIoutput\fR in their
proper order.  A 
.c PCMBitDecoder
galaxy produced the correct PCM levels given the input bits.
.pp
The default paramteres in this demonstration are set so that
50 samples are encoded every iteration which will fill
approximately 1.04 cells.  Running the demonstration for 580
iterations will produce 604 cells consuming about 29,000
voice samples.
.UH REFERENCES
Goodman, J., LockHart, G., Wasem, O., and Wong, W.,
"Waveform Substitution Techniques for Recovering
Missing Speech Segment in Packet Voice Communications,"
\fIIEEE Trans. on ASSP\fR, vol ASSP-34, no. 6, pp. 1440-
1448, December 1986.
.SA
.c ATMPrevCellSub,
.c ATMZeroCellSub,
.c PCMVoiceRecover
.ES
