.\" $Id$
.NA router
.SD
This is a simple illustration of the
.c EndCase
star.
.EQ
delim off
.EN
.LO "$PTOLEMY/src/domains/ddf/demo"
.EQ
delim $$
.EN
.SV $Revision$ $Date$
.AL "E. A. Lee"
.LD
.Se EndCase
This rather trivial demo shows what the
.c EndCase
star with two data inputs does.
The
.c EndCase
star has one control input and two data inputs.
Depending on the value of control, either the true input (input#2)
or the false input (input#1) is routed to the output.
In this demo, the control Boolean is provided by a threshold device
whose input is random.
Two ramp generators with different slopes (1.0 and 3.0)
are attached to the data inputs of the
.c Select
star.
Thus two ramps are randomly interleaved in the output display.
.SA
EndCase.
.ES
