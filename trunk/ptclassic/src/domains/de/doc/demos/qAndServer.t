.\" $Id$
.NA qAndServer
.SD
Demonstrate the use of the
.c FIFOQueue
and
.c Stack
stars together with
.c Server
stars.
A regular counting process is enqueued on both stars.
The particles are dequeued whenever the server is free.
The
.c Stack
star is set with a larger capacity than the
.c FIFOQueue
star.
so it overflows second.
Overflow events are displayed.
.DE
.LO "$PTOLEMY/src/domains/de/demo"
.SV $Revision$ $Date$
.AL "E. A. Lee"
.LD
.IE FIFOQueue
.IE queue
.IE Server
A regular sequence of input events with interval 1.0 arrive at a
.c FIFOQueue
and
.c Stack.
The outputs of the queue and stack are connected to
.c Server
stars with service time of 1.1.
Note that because the \fInumDemandsPending\fR state of the queue and stack
are initialized to unity (the default), the first input event passes
immediately through to the servers.
When the servers are finished serving that event (at time 1.1),
they output particles which are routed to the \fIdemand\fR inputs
of the queue and stack.
Queued and stacked events (which arrived at time 1.0) are passed on
to the servers.
.pp
The first event passes directly through the queue and stack, not affecting 
their size.
The next 10 input events are stored, raising the sizes of the queue and
stack to unity.
The next 11 arrivals raise the sizes to 2, and the next 11 to 3.
The capacity of the
.c FIFOQueue
star is set to 2, so it will overflow at this point, losing one data value.
Lost particles are displayed in a separate trace.
After this, for every 11 samples that are successfully served, one
will be lost.
.SA
delayVsServer,
queue,
FIFOQueue,
Stack,
Server.
.ES
