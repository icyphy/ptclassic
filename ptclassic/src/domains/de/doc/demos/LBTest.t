.\" $Id$
.NA LBTest
.SD
Simulates leaky bucket network rate monitors.
.DE
.SV $Revision$ 11/9/92
.AL "Paul Haskell."
.LD
The
.c Poisson
and
.c FloatRamp
blocks output an increasing sequence of numbers spaced
randomly in time.
This demonstration shows how leaky bucket controllers
discard input samples that violate their rate and burstiness
bounds.

With multiple leaky buckets, several rate and burstiness bounds
can be enforced at once.
This simulation graphs the data that gets rejected by
several leaky bucket controllers and shows the effective
capacity of each leaky bucket as well.
.SA
LeakBucket
.ES
