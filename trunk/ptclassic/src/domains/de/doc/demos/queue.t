.\" $Id$
.NA queue
.SD
Demonstrate the use of the
.c FIFOQueue
and
.c Stack
stars.
A Poisson counting process is enqueued on both stars,
and is dequeued at a regular rate, every 1.0 time units.
The output of the
.c FIFOQueue
is always monotonically increasing,
because of the FIFO policy, but the output of the
.c Stack
need not be.
The
.c Stack
is set with a smaller capacity than the
.c FIFOQueue
star, so it overflows first.
Overflow events are displayed.
.DE
.LO "$PTOLEMY/src/domains/de/demo"
.SV $Revision$ $Date$
.AL "E. A. Lee"
.LD
.IE Queue
.IE FIFOQueue
.IE Stack
.IE Poisson counting process
This demonstration feeds a
.c FIFOQueue
and a
.c Stack
with a
.c Poisson
counting process with average interarrival time of 1.0.
An output is demanded from the queue block at regular intervals of 1.0 using a
.c Clock
star.
Hence, the arrivals and departures are precariously balanced.
.pp
When an output is demanded and the queue or stack is empty, then no output
is produced until the next arrival at the queue input.
This shows up in the plots as intervals of greater than 1.0 between
outputs of the queue and stack.
If more demand inputs arrive while the queue is still empty,
they are ignored.
This shows up as successive zero-valued outputs in the size plots.
(Notice that the sizes of the stack and the queue should always be the same.)
.pp
It is possible for the demand inputs to be accumulated until they can
be satisfied.
Just set the \fIconsolidateDemands\fR parameter of the
.c FIFOQueue
or
.c Stack
block to FALSE.
This will have the effect of keeping the sizes much smaller,
because during large interarrival times the
.c FIFOQueue
or
.c Stack
accumulates many demands that can later be used up
when the interarrival times are small.
.pp
To keep the queue size from growing indefinitely it is constrained
to a maximum size of 4 (for the
.c FIFOQueue
star) and 3 (for the
.c Stack
star).
When the queue or stack reaches its capacity, it accepts no further inputs,
and the input events are sent to the overflow output.
This shows up on the "Data queued" and "Data stacked" plots
as a second trace.
Notice also the gaps in the queue and stack outputs.
For instance, if value 29 is followed by value 32,
then two input events, with values 30 and 31, were lost due
to queue overflow, and will not appear at the queue or stack output.
.pp
.IE Geodesic
In \*(PT, the arc connecting two blocks is implemented with a
\fIGeodesic\fR, which behaves like a FIFO queue with capacity only bounded
by the amount memory in your computer.
However, the geodesic will normally not hold particles after the simulation
time has passed the time stamp of the data.
The queue block holds particles until they are demanded by an event
at the demand input.
The time stamp of the output will be equal to that of the demand input,
which may be delayed relative to the arrival of an input.
So unlike a geodesic, the queue can introduce time delay.
Furthermore, the
.c Queue
block provides a mechanism for simulating the effects of queues
with bounded size.
And it provides a mechanism for easily monitoring the amount of data
in a queue.
.SA
Clock,
qAndServer,
Stack,
FIFOQueue.
.ES
