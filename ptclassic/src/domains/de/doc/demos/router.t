.\" $Id$
.NA router
.SD
Randomly route an irregular but monotonic
signal (a Poisson counting process) through
two channels with random delay, and merge the
channel outputs.
.DE
.LO "$PTOLEMY/src/domains/de/demo"
.SV $Revision$ $Date$
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
