.\" $Id$
.NA testPacket
.SD
This system constructs packets consisting of five sequential
values from a ramp, sends these packets to a server with a
random service time, and then deconstructs the packets by
reading the items in the packet one by one.
.DE
.LO "$PTOLEMY/src/domains/de/demo"
.SV $Revision$ $Date$
.AL "J. T. Buck"
.LD
When five sequential values of the ramp appear at the output,
then an entire packet has been received and deconstructed.
After an entire packet has been deconstructed, it is possible
for the last value to get repeated.
This occurs if no new packet has arrived by the time the outputs
are requested.
In addition, a portion of each packet may be lost.
This occurs if a new packet arrives before a previously arrived
packet has been completely deconstructed.
.ES
