.\" $Id$
.NA queue
.SD
Demonstrates the use of the Queue block.
.DE
.SV $Revision$ $Date$
.AL "E. A. Lee"
.LD
.IE Queue
This demonstration feeds a queue with a Poisson counting process
with average interarrival time of 1.0.
An output is demanded from the
.c Queue
block at regular intervals of 1.0 using a
.c Clock.
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
It is possible for the demand inputs to be accumulated until they can
be satisfied.  Just set the \fIconsolidateDemands\fR parameter of the
Queue block to FALSE.  This will have the effect of keeping
the queue size much smaller, because during large interarrival times
the Queue accumulates many demands that can later be used up
when the interarrival times are small.
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
.pp
.IE Geodesic
In \*(PT, the arc connecting two blocks is implemented with a
.c Geodesic,
which behaves like a FIFO queue with capacity only bounded
by the amount memory in your computer.
However, the geodesic will normally
not hold particles after the simulation time
has passed the time stamp of the data.
The queue block holds particles until they are demanded by an event
at the demand input.  The time stamp of the output will be equal to
that of the demand input, which may be delayed relative to the arrival
of an input.  So unlike a geodesic, the queue can introduce time delay.
Furthermore, the Queue block
provides a mechanism for simulating the effects of queues
with bounded size.
And it provides a mechanism for easily monitoring the amount of
data in a queue.
.SA
Clock,
Poisson counting process,
qAndServer,
Queue.
.ES
