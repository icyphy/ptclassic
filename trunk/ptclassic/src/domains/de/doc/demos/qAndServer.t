.\" $Id$
.NA qAndServer
.SD
Demonstrates the use of the Queue block together with a Server.
.DE
.SV $Revision$ $Date$
.AL "E. A. Lee"
.LD
.IE Queue
.IE Server
A regular sequence of input events with intevals 1.0 arrive at a
.c Queue.
The output of the queue
is connected to a
.c Server
with service time 1.1.
Note that because the \fInumDemandsPending\fR state of the queue is initialized
to unity (the default), the first input event passes immediately through
the queue to the server.  When the server is finished serving that event
(at time 1.1),
it outputs the particle, which is routed to the \fIdemand\fR input
of the queue.  A queued event (which arrived at time 1.0)
is passed on to the server.
.pp
The first event passes directly through the queue, not affecting its size.
The next 10 input events are queued, raising the size of the queue to unity.
The next 11 arrivals raise the queue size to 2,
and the next 11 to 3.
The arrival after this (the 34th) will be lost because it arrives when the queue
is full.  This is evident in the queue output plot, which is missing
the particle with value 33.  It is also evident in the queue size plot,
which shows two successive values at the queue capacity.
After this, for every 11 samples that are successfully served, one
will be lost.
.SA
delayVsServer,
Queue,
Server.
.ES
