.\" $Id$
.NA psServer
.SD
Demonstrate the processor-sharing server.
Unlike other servers, this server accepts new inputs at any time,
regardless of how busy it is.
Accepting a new input, however, slows down the service to all
particles currently being served.
.DE
.LO "$PTOLEMY/src/domains/de/demo"
.SV $Revision$ $Date$
.AL "E. A. Lee"
.LD
A regular stream of particles, spaced at intervals of 0.8,
is sent to a
.c PSServer
star, or processor-sharing server star.
.IE PSServer
.IE "processor-sharing server"
In this demo, the nominal service time is 1.0,
to slow for the interarrival period of 0.8.
The actual service time is 1.2 for all arrivals because of
the overlapped time they spend sharing the server.
.SA
PSServer.
.ES
