.\" $Id$
.NA aloha
.SD
A simulation of the ALOHA multiple access protocol
.DE
.SV $Revision$ $Date$
.AL "Rolando Diesta"
.LO "~ptolemy/src/domains/de/demo"
.LD
This is a simulation of the (pure) ALOHA multiple access protocol. The protocol
assumes a composite of independent Poisson sources transmitting on a common channel.
All overlapping transmissions are lost and are retransmitted after a random delay.

Events are generated with exponentially distributed interarrival times to indicate the
start of (first) transmission of a packet. The magnitude of an event is changed to an
exponentially distributed number to indicate the length of the packet or its service
time in the channel. The first packet goes through the FIFOQueue to the server which
remains busy over the packet's service time. When the server is busy, any new packets
that enter the queue go to the overflow output since the queue has zero capacity.

The total number of packets N in the channel is tracked by an Up/Down counter. Packets
entering the channel increment N; packets leaving the channel decrement N. The counter
outputs N every time N changes. A second FIFOqueue is used as a memory element to keep
the previous value of N. The channel's state is set to Collision if N has changed from
1 to 2. The channel's state is set to No Collision if N has changed from 1 to 0. Other
transitions in N do not affect the channel's state.

In a Collision state, the Switches act: a) to direct any packet in the server for
retransmission; and b) to divert all packets entering the channel from reaching the
server. Packets that do not reach the server are directed for retransmission. Packets
directed for retransmission are delayed by a random interval distributed uniformly
from (R+1)*T to (R+K)*T, where T is the mean packet service time, R*T is the round trip
delay, and K is a retransmission parameter. Packets directed for retransmission cannot
be withdrawn. Any packet leaving the server in a No Collision state is deemed received.

The output graphs show: a) time instances of (first) transmissions, retransmissions,
and receptions; b) number of received packets over time; c) an end-to-end delay
histogram; and d) the number of packets in the channel over time.
.SA
FIFOQueue,
Poisson,
RanGen,
VarDelay,
VarServer,
XHistogram,
Test.
.ES
