.\" $Id$
.NA VClock
.SD
Models a network with four inputs and a virtual clock buffer.
.DE
.SV $Revision$ 11/9/92
.AL "Paul Haskell."
.LD
The
.c Poisson
and
.c PseudoCell
blocks provide four random input streams of data cells.
The
.c VirtClock
star queues and outputs the arriving cells in "virtual clock"
fashion.
The output graph shows the length of the longest virtual
clock queue, the output times, and the cell-loss times.

See
H. Zhang and S. Keshav,
"Comparison of Rate-Based Service Disciplines,"
\fIACM SIGCOMM'91 Conference\fR,
vol. 21, #4, September 1991, pp. 113-121.
for a discussion of the virtual clock and other buffer service
disciplines.
.SA
VirtClock
.ES
