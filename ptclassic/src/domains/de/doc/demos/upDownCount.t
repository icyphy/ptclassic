.\" $Id$
.NA upDownCount
.SD
Demonstrate the
.c UDCounter
star, an up/down counter.
Events are generated at two different rates to count up and down.
The up rate is faster than the down rate, so the trend is upwards.
The value of the count is displayed every time it changes.
.DE
.LO "$PTOLEMY/src/domains/de/demo"
.SV $Revision$ $Date$
.AL "S. Ha"
.LD
.IE UDCounter
This demo illustrates the usage of a very general up/down counter (
.c UDCounter
) in the DE domain.
The \fIcountUp\fR input is fed by a clock signal (interval = 2),
and the \fIcountDown\fR input is fed by another clock signal
(interval = 3.33).
The count state of the
.c UDCounter
block is sent to an
.c Xgraph
star whenever it changes.
.SA
UDCounter.
.ES
