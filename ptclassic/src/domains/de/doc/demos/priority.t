.\" $Id$
.NA priority
.SD
Demonstrates the use of the PriorityQueue block together with a Server.
.DE
.LO "~ptolemy/src/domains/de/demo"
.SV $Revision$ $Date$
.AL "E. A. Lee"
.LD
.IE PrQueue
.IE "priority queue"
.IE Server
This is similar to the \fIqAndServer\fR demo,
except that a
.c PriorityQueue
is used instead of a regular
.c FIFOQueue.
The
.c PriorityQueue
block can take any number of inputs (two in this example),
and the inputs are prioritized from top to bottom.
Each input has its own queue.
When a demand event arrives, an output will be produced
from the highest priority queue that is not empty.
In this demo, the high priority input is fed events of value 1.0
with interval 5.0 time units between events.
The tokens in this process are plotted
in red if you use a color monitor.
Low priority events with interarrival time of 1.0
and value 2.0 are fed to second input of the
.c PriorityQueue.
Notice on the output plot that high priority arrivals always pass to the
output within the service time, or 1.0 time unit.
The server is not preempted.
.pp
The \fIcapacity\fR parameter of the PriorityQueue star gives the total
storage limit of the star.  When the storage limit is exceeded,
the lowest priority items are removed from the queue sent to the
\fIoverflow\fR output.  Hence, a high priority arrival can displace
a lower priority item that had previously arrived and been queued.
In the demo, the capacity is set to 6.  Once this capacity is reached,
each time a high priority event arrives, a low priority event
will be bumped to the \fIoverflow\fR output.
.SA
delayVsServer,
qAndServer,
queue,
FIFOQueue,
Server.
.ES
