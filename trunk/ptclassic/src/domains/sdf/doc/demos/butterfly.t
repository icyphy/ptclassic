.\" $Id$
.NA butterfly
.SD
This system uses sines and cosines to compute a curve
known as the butterfly curve, invented by T. Fay.
The curve is plotted in polar form.
.DE
.EQ
delim off
.EN
.LO "$PTOLEMY/src/domains/sdf/demo"
.EQ
delim $$
.EN
.DM SDF Universe
.SV $Revision$ $Date$
.AL "Edward A. Lee"
.LD
.Ir "butterfly curve"
The butterfly curve was described in [1].
In polar form, the magnitude $r$ is given as a function of the phase
$theta$ by the equation
.EQ
r ~=~ e sup { cos ( theta ) } ~-~ 2 cos ( 4 theta ) ~+~
sin sup 5 ( theta / 12) ~.
.EN
.Se XYgraph
.UH REFERENCES
.ip [1]
T. Fay, "The Butterfly Curve," \fIAmerican Math. Monthly,\fR 96(5), pp. 442-443.
.SA
XYgraph.
.ES
