.\" $Id$
.NA router
.SD
Random delay and re-ordering demo.
.DE
.LO "~ptolemy/src/domains/de/demo"
.SV $Revision$ "October 22, 1990"
.AL "E. A. Lee"
.LD
A Poisson counting process is randomly routed through one
of two possible random delays, one with a uniform distribution
and one with an exponential distribution.
The
.c Router
star does the random routing.
The random delays sometimes cause samples to arrive out of order.
.SA
ExpDelay,
Poisson counting process,
Router,
UniDelay.
.ES
