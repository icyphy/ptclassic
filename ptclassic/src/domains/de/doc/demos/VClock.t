.\" $Id$
.NA VClock
.SD
Model a network with four inputs and "virtual clock" buffer service.
.DE
.LO "~ptolemy/src/domains/de/demo"
.SV $Revision$ 11/9/92
.AL "Paul Haskell."
.LD
The
.c Poisson
and
.c PseudoCell
stars provide four random input streams of data cells.
The
.c VirtClock
star implements four queues and the "virtual clock" queue
service discipline.
The virtual clock discipline is similar to the "round-robin"
queue service discipline.
With round-robin service, time is divided into slots, and in
each timeslot, the queue for one input channel is served.
All input channels are served in turn.
The virtual clock discipline differs in that if a queue is empty,
the next queue in sequence is served immediately rather than
in the next timeslot.
.pp
The output graph shows the length of the longest input
queue vs. time, the output times for successfully
delivered data cells, and the times at which cell losses occurred.
.pp
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
