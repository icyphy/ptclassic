.\" $Id$
.NA complexExponential
.SD
Generate and plot a complex exponential.
.DE
.EQ
delim off
.EN
.LO "$PTOLEMY/src/domains/sdf/demo"
.EQ
delim $$
.EN
.DM SDF Universe
.Ir "complex exponential"
.Ie "expgen"
.Ie "CxReal"
.Ie "XMgraph"
.SV $Revision$	$Date$
.AL "Edward A. Lee"
.LD
A complex exponential with frequency $pi$/50 (radians per second)
is generated and plotted.
The sampling frequency is normalized to $2 pi$ radians per second.
.SA
CxToFloat,
expgen,
XMgraph.
.ES
