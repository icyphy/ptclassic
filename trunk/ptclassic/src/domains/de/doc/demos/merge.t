.\" $Id$
.NA merge
.SD
This rather trivial demo demonstrates the Merge star.
It is fed two streams of regular arrivals,
one a ramp beginning at 10.0, and one a ramp beginning at 0.0.
The two streams are merged into one, in chronological order,
with simultaneous events appearing in arbitrary order.
.DE
.LO "$PTOLEMY/src/domains/de/demo"
.SV $Revision$ $Date$
.AL "S. Ha"
.LD
.Se Merge
The simultaneous events are recognized by vertical lines in the
output plot connecting two events with the same time stamp.
.SA
Merge.
.ES
