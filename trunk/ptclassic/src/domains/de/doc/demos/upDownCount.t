.\" $Id$
.NA upDownCount
.SD
Demonstrates the usage of the up/down counter
.DE
.LO "~ptolemy/src/domains/de/demo"
.SV 1.1 "December 3, 1990"
.AL "S. Ha"
.LD
.IE UDCounter
This demo illustrates the usage of a very general up/down counter (
.c UDCounter
) in the DE domain.
The \fIcountUp\fR input is fed by a clock signal (interval = 2),
and the \fIcountDown\fR input is fed by another clock signal
(interval = 3.33).  The count state of the
.c UDCounter
block is disclosed at the demand input.  
We merge two input events to the demand input to display the
count at every input.
The count state is reset
by the reset input.  In this demo, no event is delivered to the reset
input, so connected to the
.c Null
block.  
.SA
Null,
UDCounter.
.ES
