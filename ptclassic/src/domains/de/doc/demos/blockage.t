.\" $Id$
.NA blockage
.SD
Demonstrates the simulation of a blocking strategy in the queueing network.
.DE
.SV 1.1 "October 23, 1990"
.AL "S. Ha"
.LD
.IE BlockingAsk
.IE BlockingAccept
The
.c Queue
block in the library is a generic one without any special feature.
If a Queue reaches capacity, further arrivals are lost silently.
This demonstration shows how to implement a \fIblocking\fR
strategy in the queueing network. Since we may have several
blocking strategies, we isolate the blocking strategy into
a separate module from the generic queueing network. We may
change the module if want to use another blocking starategy.
.pp
The implementation of a blocking function is done by a pair of
blocks :
.c BlockingAsk
block and
.c BlockingAccept
block.
The
.c BlockingAsk 
block monitors the size of the Queue. If the Queue is saturated,
the
.c BlockingAsk
block generates a block-request output to the
.c BlockingAccept
block, which is connected to the Queue
(\fIthreshold\fR state of the
.c BlockingAsk
block should be matched to the \fIcapacity\fR of the second Queue.)
Without block-request from the
.c BlockingAsk 
block, the
.c BlockingAccept
block is a trivial buffer without any latency. If the block-request
input arrives, it does not send any output (it is blocked).
Look at the demand input of the first Queue. If the
.c BlockingAccept
block is blocked, then the first server is also blocked since
no data can be fetched from the first Queue. Therefore, the
size of the first Queue keeps growing during the blocking period.
Once the Queue is freed from the saturation, the 
.c BlockingAsk
block generates a block-release
output to the
.c BlockingAccept
block, which then resumes the normal operation.
.pp
Note that if we want to realize the chain of blockage, we have to
manually insert this pair of blocks around the server.
In the demonstration, the second server is slower (meanServiceTime = 2.0)
then the first server (constant serviceTime = 1.0). Also, the capacity
of the second Queue is pretty small (4) comparing with the first Queue (50).
The large capacity of the first Queue guarantees that we do not lose
any data from the source. The interesting display is the size of the
second Queue. Once it reaches saturation, the size is bounced up and down
between 3 and 4. It is because once the second Queue is freed from 
saturation, the first server send the data to the Queue which makes
it again saturated right away.
.SA
queue,
ExpServer,
Queue.
.ES
