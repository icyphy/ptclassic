.\" $Id$
.NA qAndServer
.SD
Demonstrates the use of the FIFOQueue and Stack blocks together with Servers.
.DE
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
The outputs of the queue and stack
is connected to
.c Server s
with service time 1.1.
Note that because the \fInumDemandsPending\fR state of the queue and stack
are initialized
to unity (the default), the first input event passes immediately through
to the servers.  When the servers are finished serving that event
(at time 1.1),
they output particles which are routed to the \fIdemand\fR inputs
of the queue and stack.  Queued and stacked events (which arrived at time 1.0)
are passed on to the servers.
.pp
The first event passes directly through the queue and stack, not affecting 
their size.
The next 10 input events are stored, raising the sizes of the queue and
stack to unity.
The next 11 arrivals raise the sizes to 2,
and the next 11 to 3.
The arrival after this (the 34th) will be lost because it arrives when the queue
is full.  This is evident in the queue and stack output plots, which are missing
the particle with value 33.  The lost particles are also displayed
in a separate trace.  It is also evident in the queue size plots,
which show two successive values at the queue capacity.
After this, for every 11 samples that are successfully served, one
will be lost.
.SA
delayVsServer,
queue,
FIFOQueue,
Stack,
Server.
.ES
