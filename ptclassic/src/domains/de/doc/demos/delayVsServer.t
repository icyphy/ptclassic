.\" $Id$
.NA delayVsServer
.SD
Illustrates the difference between the Delay and Server blocks.
.DE
.SV $Revision$ "October 22, 1990"
.AL "E. A. Lee"
.LD
A deterministic
.c Delay
block (with delay 1.0)
and a deterministic
.c Server
block (with service time 1.0) are fed the same Poisson counting process.
.IE Delay
.IE Server
The Poisson counting process is generated by feeding a
.c FloatRamp
star with Poisson arrivals.
.IE "Poisson counting process"
The output plot shows that the Server does not accept new inputs
while it is serving previous inputs, and hence may delay
some samples more than the Delay block.
Inputs to the server are queued on the input Geodesic, which
has effectively infinite capacity.
.SA
Delay,
FloatRamp,
Poisson,
Server.
.ES
