.\" $Id$
.NA "ATMPrevCellSub"
.SD
This demonstration implements a voice-cell recovery technique.
Cells which have been dropped during transmission are replaced with
the cell which arrived at the receiver immediately prior to the dropped cell.
.SE
.LO "$PTOLEMY/src/domains/de/demo"
.SV $Revision$ $Date$
.AL "G. S. Walter"
.LD
This demonstration reads 8-kHz voice samples, converts
them into 64 kbps PCM using CCITT Recommendation G.711,
and then loads them into a
.c SeqATMCell
star.
.Ir "CCITT Recommendation G.711"
.Ir "PCM"
.Ir "speech coding"
.Ir "ATM"
The
.c SeqATMCell
stars have sequence numbers which are set using a modulo-8
numbering pattern.
The loaded
.c SeqATMCell
stars are placed into the object
.c NetworkCell
which is used as the information carrier over the network.
During transmission, cells will be dropped with a probability
determined by the 
.c LossyInput
galaxy.
The
.c SeqATMCell
stars are removed from the 
.c NetworkCell
stars at the receiver's side of the network.
Next, a 
.c SeqATMSub
star checks the sequence numbers of the incoming 
.c SeqATMCells
to determine if any were dropped during transmission.
If it is discovered that a cell is missing, the bits from
the cell which arrived just prior to the missing cell
are substituted for the contents of the missing cell.
The bits are fed to a
.c PCMBitDecoder
star where they are converted to their proper PCM levels.
.pp
The default parameters in this demonstration are set so that
50 samples are encoded every iteration which will fill
approximately 1.04 cells.
Running the demonstration for 580 iterations will produce 604 cells
consuming about 29,000 voice samples.
.SA
.c ATMZeroCellSub,
.c ATMPattRecover.
.ES
