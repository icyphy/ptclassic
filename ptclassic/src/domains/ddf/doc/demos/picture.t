.\" $Id$
.NA picture
.SD
Two dimensional random walk with equi-spaced points.
.DE
.SV 1.1 "September 29, 1990"
.AL "E. A. Lee"
.LD
.ES
.IE "Repeater"
This demo demonstrates the hierarchy of wormholes.
The outermost SDF domain has a wormhole of DDF domain, which
by turns has a wormhole of SDF domain.
The
.c Distributor
star receives two uniformly distributed random numbers and 
produces one token to each output.  Two random numbers indicates
a random point in the two-dimensional plane.  The
.c RectPolar
star translates the rectangular coordinates to the polar coordinates
to produce one magnitude output and one angle output.
From the angle output, the unit directional vector is
calculated and is fed into the 
.c Repeater
star in the wormhole of DDF domain.
The magnitude output decides how many times the directional
vector is repeated. The repeated directional vector is
integrated in the innermost wormhole of SDF domain and
the final point is displayed in the two-dimensional plane.
The reset input of the 
.c Integrator
is fed by zero value which does no effect on the integration.
The wormhole of DDF domain
illustrates an example of a data-dependent iteration whose body
is the wormhole of SDF domain.
.SA
Distributer,
RectPolar,
Repeater,
Integrator,
XYgraph.
.ES

