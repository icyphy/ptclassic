.\" $Id$
.NA blockage
.SD
Demonstrate a blocking strategy in a queueing network.
In a cascade of two queues and servers, when the second queue
fills up, it prevents any further dequeuing of particles from
the first queue until it once again has space.
.DE
.LO "$PTOLEMY/src/domains/de/demo"
.SV $Revision$ $Date$
.AL "S. Ha and E. A. Lee"
.LD
.IE TestLevel
.IE PassGate
The
.c Queue
block discards arrivals after it reaches capacity.
This demonstration shows how to implement a \fIblocking\fR
strategy in the queueing network.
This is done in a modular way so that a variety of blocking strategies
are equally easy to implement.
.pp
The implementation of a simple blocking function is done by a pair of
blocks:
.c TestLevel
which checks the \fIsize\fR output of the
.c Queue against its capacity, and
.c PassGate
which blocks particles when the blocking
.c Queue
has reached capacity.
When the blocking queue reaches capacity, the
.c TestLevel
block sends a signal (via an
.c Inverter
) to the
.c PassGate
thereby preventing further particles from arriving
at the
.c Queue
star.
.pp
Two successive
.c Queue
and
.c Server
pairs are implemented here.
Only the second one is a blocking queue.
The capacity of the first one is set to 10, and after some time,
this capacity is reached, and input particles are lost.
The capacity of the second one is 4.
When this capacity is reached, the
.c PassGate
is closed, preventing any further \fIdemand\fR events at the first queue.
While the gate is closed, further arrivals will accumulate in the first queue.
When the second queue receives a \fIdemand\fR event, its size will drop
below capacity, and a signal will be sent to the
.c PassGate
to open.
The arrival interval of particles is 1.0, and the service time of
the second server is 3.0, so the size of the first
.c Queue
grows until it reaches capacity.
.SA
ExpServer,
Gate,
queue,
Queue,
Threshold.
.ES
