.\" $Id$
.NA FlushNet
.SD
Shows the use of the "FlushQueue" galaxy.
.DE
.SV $Revision$ 11/9/92
.AL "Paul Haskell."
.LD
The
.c Poisson
and
.c FloatRamp
blocks provide a random increasing input stream of numbers.
The
.c FlushQueue
galaxy queues and outputs the arriving inputs.
If the queue reaches capacity, all new inputs are discarded
until \fBall\fR items in the queue are output.
Compared to strict first-come, first-served (FIFO) queue service,
this queue service discipline can increase the time between
queue overflows, at the expense of the loss of more data
when the queue does overflow.
The output graph shows the inputs that are successfully output.
.SA
FlushQueue
.ES
