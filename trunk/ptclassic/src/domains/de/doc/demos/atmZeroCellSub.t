.\" $Id$
.NA ATMZeroCellSub
.SD
This demonstration implements a simple voice-cell recovery technique.
Cells which have been dropped during transmission are replaced with all 0 bits.
.EQ
delim off
.EN
.LO "$PTOLEMY/src/domains/de/demo"
.EQ
delim $$
.EN
.SV $Revision$ $Date$
.AL "G. S. Walter"
.LD
This demonstration reads 8 kHz voice samples, converts
them into 64 kbps PCM using CCITT Recommendation G.711,
and then loads them into a
.c SeqATMCell
star.
.Ir "CCITT Recommendation G.711"
.Ir "PCM"
.Ir "speech coding"
The
.c SeqATMCell
stars have sequence numbers which are set using a modulo-8
numbering pattern.
The loaded
.c SeqATMCells
are placed into the object
.c NetworkCell
which is used as the information carrier over the network.
During transmission, cells will be dropped with a probability
determined by the
.c LossyInput
galaxy.
The 
.c SeqATMCells 
are removed from the
.c NetworkCells
at the receiver's side of the network.
Next, a star
.c SeqATMZero
checks the sequence numbers of the incoming
.c SeqATMCell
stars to determine if any were dropped during transmission.
If it is discovered that a cell is missing, all 0-valued
bits are substituted for the contents of that cell.
The
.c SeqATMZero
star outputs all the bits from the arrived cells in proper
order.
They are decoded to PCM levels using a
.c PCMBitDecoder
star.
.pp
The default parameters in this demonstration are set so that
50 samples are encoded every iteration which will fill
approximately 1.04 cells.
Running the demonstration for 580 iterations will produce 604 cells
consuming about 29,000 voice samples.
.SA
ATMPrevCellSub,
ATMPattRecover.
.ES
