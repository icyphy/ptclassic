.\" $Id$
.NA tbus
.SD
This is a simple illustration of the bus facility in Ptolemy,
in which multiple signals are combined onto a single graphical
connection.
.DE
.LO "$PTOLEMY/src/domains/sdf/demo"
.DM SDF Universe
.SV $Revision$ $Date$
.AL "Joseph T. Buck"
.LD
This trivial system connects a
.c Distributor
to an
.c XMgraph
using a single connection that represents an arbitrary
number of connections.  The bus icon has a single parameter
that controls the number of connections.
The default is set to four.
.SA
Distributor, XMgraph.
.ES
