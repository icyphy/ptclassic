.\" $Id$
.NA prioritized
.SD
This is a high-level simulation of a shared memory
with prioritized arbitration.
.EQ
delim off
.EN
.LO "$PTOLEMY/src/domains/de/demo"
.EQ
delim $$
.EN
.SV $Revision$ $Date$
.AL "T.M. Parks"
.LD
A set of
.c Poisson
stars generates addresses (read requests) for a shared memory at random times.
These address events are passed through a
.c prioritized4
galaxy which provides arbitration and routes the events to the shared memory.
The data responses from the memory are passed back through the
.c prioritized4
galaxy which routes them to the appropriate output where they are
graphed.
One graph shows the data traffic on the shared bus, while the other graphs
monitor each output to verify that responses have been routed properly.
.SA
Poisson,
prioritized4,
rom,
roundRobin.
.ES
