.\" $Id$
.NA priority
.SD
Demonstrates the use of the PrQueue block together with a Server.
.DE
.SV $Revision$ "October 22, 1990"
.AL "E. A. Lee"
.LD
.IE PrQueue
.IE "priority queue"
.IE Server
This is similar to the \fIqAndServer\fR demo,
except that a
.c PrQueue
is used instead of a regular
.c Queue.
The
.c PrQueue
block can take any number of inputs (two in this example),
and the inputs are prioritized from top to bottom.
Each input has its own queue.
When a demand event arrives, an output will be produced
from the highest priority queue that is not empty.
In this demo, the high priority input is fed a Poisson process
with a relatively high average interarrival time of 10.0.
The tokens in this process have value unity, and are plotted
in red if you use a color monitor.
The low priority input has a high average interarrival time of 1.0,
has value 2.0, and is plotted in green.
The output of the server is plotted in blue.
Notice on the output plot that high priority arrivals always pass to the
output within twice the service time, or 2.0 time units.
This worst case occurs only if the server has just begun to serve
a low priority arrival when a high priority arrival occurs.
The server is not preempted.
.SA
delayVsServer,
qAndServer,
queue,
Queue,
Server.
.ES
