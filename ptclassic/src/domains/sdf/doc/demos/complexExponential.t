.\" $Id$
.NA complexExponential
.SD
Generate and plot a complex exponential.
.EQ
delim off
.EN
.LO "$PTOLEMY/src/domains/sdf/demo"
.DM SDF Universe
.SV $Revision$	$Date$
.AL "Edward A. Lee"
.EQ
delim $$
.EN
.LD
.Ir "complex exponential"
.Ie "expgen"
.Ie "CxReal"
.Ie "XMgraph"
A complex exponential with frequency $pi$/50 (radians per second)
is generated and plotted.
The sampling frequency is normalized to $2 pi$ radians per second.
.SA
CxToFloat,
expgen,
XMgraph.
.ES
