.\" $Id$
.NA queue
.SD
Demonstrates the use of the Queue block.
.DE
.SV $Revision$ "October 22, 1990"
.AL "E. A. Lee"
.LD
.IE Queue
.IE Geodesic
In \*(PT, the arc connecting two blocks is implemented with a
.c Geodesic,
which behaves like a FIFO queue with capacity only bounded
by the amount memory in your computer.
The
.c Queue
block provides a mechanism for simulating the effects of queues
with bounded size.
It also provides a mechanism for easily monitoring the amount of
data in a queue.
This demonstration feeds a queue with a Poisson counting process
with average interarrival time of 1.0.
An output is demanded from the
.c Queue
block at regular intervals of 1.0.
Hence, the arrivals are departures are precariously balanced.
.pp
When an output is demanded and the queue is empty, then no output
is produced until the next arrival at the queue input.
This shows up in the plot as intervals of greater than 1.0 between
outputs of the queue.
If more demand inputs arrive while the queue is still empty,
they are ignored.
This shows up as successive zero-valued outputs in the Queue-size plot.
.pp
To keep the queue size from growing indefinitely it is constrained
to a maximum size of 4.
When the queue reaches this size, it accepts no further inputs,
and the input events are lost.
This shows up in the Queue-size plot as multiple successive
outputs with value 4.
It shows up on the "Output of queue" plot as gaps in the counting
processing.  For instance, if value 29 is followed by value 32,
then two input events, with values 30 and 31, were lost due
to queue overflow.
.SA
Clock,
Poisson counting process,
qAndServer,
Queue.
.ES
