.\" $Id$
.NA FlushNet
.SD
Simulate a queue with "input flushing" during overflow.
.DE
.SV $Revision$ 11/9/92
.AL "Paul Haskell."
.LD
The
.c Poisson
and
.c FloatRamp
blocks provide a randomly spaced stream of increasing numbers
to the
.c FlushQueue
galaxy.
The
.c FlushQueue
galaxy queues and services arriving inputs.
If the queue reaches capacity, all new arrivals are discarded
until \fBall\fR items in the queue have been served.
Compared to the strict first-come, first-served (FIFO) queue
service, this queue service discipline can increase the time
between queue overflows, at the expense of the loss of more data
when the queue does overflow.
.pp
The output graph shows the inputs that are successfully output.
The
.c FlushQueue
galaxy graphs its queue length vs. time.
.pp
Users may wish to compare this queue service discipline to other
queue service disciplines, using Poisson and other types of sources.
.SA
Poisson
FlushQueue
.ES
