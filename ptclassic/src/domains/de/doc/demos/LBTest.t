.\" $Id$
.NA LBTest
.SD
Simulate leaky bucket network rate monitors.
.DE
.SV $Revision$ 11/9/92
.AL "Paul Haskell."
.LD
The
.c Poisson
and
.c FloatRamp
blocks provide a randomly-spaced increasing sequence of numbers.
This demonstration shows how leaky bucket controllers
discard input samples that violate the leaky buckets'
rate and burstiness bounds.
.pp
Networks
that use asynchronous transfer mode (ATM) transmission
such as the broadband integrated services digital network (BISDN)
can carry variable-rate traffic.
However, for a network to guarantee its service quality,
such as with bounds on a connection's loss rates and delay,
the network needs to ensure that individual connections do not send
data more quickly than allowed.
The \fIleaky bucket\fR controller is one popular method for
monitoring a variable-rate source to ensure that it does
not transmit data too quickly.
.pp
The leaky bucket acts much like a queue with deterministic service.
Without actually enqueueing any data,
the leaky bucket counts how many data cells \fIwould be\fR
in a queue with a specified capacity and deterministic service
rate.
With multiple leaky buckets, several rate and burstiness bounds
can be enforced at once.
.pp
This simulation graphs the data values that get rejected by
several leaky bucket controllers.
Another graph shows the effective capacity of each leaky bucket
vs. time.
Users may wish to see how these graphs change with different
leaky bucket parameters or with non-Poisson data traffic.
.SA
LeakBucket
Poisson
.ES
