.\" $Id$
.NA testServers
.SD
This simple system demonstrates servers with random
service times (uniform and exponential).
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
.IR Sampler
This demo feeds a regular stream of events to the
.c UniServer
and
.c ExpServer.
The original process is compared to the processed delayed by the servers.
.SA
ExpServer,
UniServer.
.ES
