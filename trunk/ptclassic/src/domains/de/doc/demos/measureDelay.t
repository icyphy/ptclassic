.\" $Id$
.NA measureDelay
.SD
Demonstrates the use of the MeasureDelay block.
.DE
.LO "~ptolemy/src/domains/de/demo"
.SV 1.1 "October 23, 1990"
.AL "S. Ha"
.LD
.IE MeasureDelay
In the queueing simulation, it is necessary to measure \fIsojourn\fR
time, which is the average delay of a 
.c Particle
traversing a queueing system. The
.c MeasureDelay
block provides a mechanism for measuring time difference of each input
particle. If the same particle arrives twice at the
.c MeasureDelay
block, the block compares the two arrival times of the particle
and generates the time difference as an output. It also passes
the particle.
.pp
This demonstration feeds a queue with a Poisson counting process
with average interarrival time of 1.0.
An output is demanded from the
.c Queue
block at average interval of 1.0.
Hence, the arrivals are departures are precariously balanced.
The same particle is fed into the
.c MeasureDelay
block before entering the
.c Queue
and after served by
.c ExpServer
block. The time difference of the two arrivals will be the sojourn time
of the particle in the queueing system. The sojourn time is
monitored in a histogram.
.SA
queue,
MeasureDelay,
Xhistogram,
Merge,
Queue.
.ES
