.\" $Id$
.NA router
.SD
Randomly route an irregular but monotonic signal (a Poisson counting process)
through two channels with random delay, and merge the channel outputs.
.DE
.EQ
delim off
.EN
.LO "$PTOLEMY/src/domains/de/demo"
.EQ
delim $$
.EN
.SV $Revision$ $Date$
.AL "E. A. Lee"
.LD
.IE Poisson counting process
A Poisson counting process is randomly routed through one
of two possible random delays, one with a uniform distribution
and one with an exponential distribution.
The
.c Router
star does the random routing.
The random delays sometimes cause samples to arrive out of order.
.SA
ExpDelay,
Router,
UniDelay.
.ES
