.\" $Id$
.NA picture
.SD
Two dimensional random walk with equally spaced points.
.DE
.SV $Revision$ $Date$
.AL "E. A. Lee"
.LO "~ptolemy/src/domains/ddf/demo"
.LD
.Ie "wormholes, nested"
.Ie "nested wormholes"
This demo demonstrates a hierarchy of nested wormholes.
The outermost SDF domain has a wormhole called "drawline"
which internally uses the DDF domain.  That wormhole, in turn,
has a wormhole called "display" which internally uses the SDF domain.
.Se Repeater
.pp
A two-dimensional random vector is generated using two
.c IIDUniform
stars, one for each coordinate.
The
.c RectPolar
star converts the rectangular coordinates of this random vector
to polar coordinates.
The angle is used to generate a normalized vector of length one
with the same direction as the original random vector.
This normalized vector, together with the length of the random
vector, are sent to the "drawline" wormhole, which draws
a line along the random vector with a series of points spaced
one unit apart.
.pp
Inside the "drawline" wormhole, the length of the random vector controls a
.c Repeater
star, which is a DDF star.
This controls how many repetitions of the normalized vector are generated.
The repeated directional vector is
integrated in the innermost wormhole (in the SDF domain) in order to
draw the points in the two-dimensional plane.
The reset input of the 
.c Integrator
is fed by a zero constant, having no effect on the integration.
The wormhole in the DDF domain
illustrates data-dependent iteration where the body of the iteration
is the wormhole of SDF domain.
.SA
RectPolar,
Repeater,
Integrator,
XYgraph.
.ES
