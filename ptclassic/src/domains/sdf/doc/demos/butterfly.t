.\" $Id$
.NA butterfly
.SD
The butterfly curves.
.DE
.IE XYgraph
.SV $Revision$ $Date$
.AL "E. A. Lee"
.LO "~ptolemy/src/domains/sdf/demo"
.LD
.Ir "butterfly curve"
This system computes and plots a curve known as the butterfly curve [1].
In polar form, the magnitude $r$ is given as a function of the phase
$theta$ by the equation
.EQ
r ~=~ e sup { cos ( theta ) } ~-~ 2 cos ( 4 theta ) ~+~
sin sup 5 ( theta / 12) ~.
.EN
.UH REFERENCE
.ip [1]
T. Fay, "The Butterfly Curve," \fIAmerican Math. Monthly,\fR 96(5), pp. 442-443.
.SA
XYgraph.
.ES
