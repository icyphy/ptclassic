.\" $Id$
.NA router
.SD
Demonstrates the Select star.
.DE
.SV 1.1 "September 27, 1990"
.AL "E. A. Lee"
.LD
.ES
.IE Select
This rather trivial demo shows what the
.c Select
star does.
The
.c Select
star has one control input and two data inputs.
Depending on the value of control boolean, either trueInput
or falseInput is routed to the output.  In this demo, the control
boolean is provided by the threshold device whose input is random.
Two ramp generators with different slopes (1.0 and 3.0)
are attached to the data inputs of the
.c Select
star.  Thus two ramps are randomly interleaved in the display output.
.SA
Select.
.ES

