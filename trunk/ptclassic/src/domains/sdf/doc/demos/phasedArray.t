.\" $Id$
.NA phasedArray
.SD
Phased array antenna simulation.
.DE
.LO "~ptolemy/src/domains/sdf/demo"
.DM SDF Universe
.SV $Revision$ "October 15, 1990"
.AL "E. A. Lee"
.LD
.Ie "phased array antenna"
.Ie "antenna, phased array"
This demo simulates a plane wave approaching an array of four antennas
from angles starting from head on and slowly rotating 360 degrees.
The response of the antenna is plotted as a function of direction
of arrival in polar form.  The outputs of the four antennas are
simply added together, so the maximal response occurs when the excitation
is incident to the plane of the antennas.  A more elaborate system
would introduce programmable delays after each antenna element in
order to steer the antenna.
The magnitude response is measured by computing the complex envelope
of the signal.  This is done with a phase splitter, which produces
an analytic signal.  The magnitude of the analytic signal is the complex
envelope.  The
.c Cut
star is used to discard initial transients.
.SA
Cut and
FIR.
.ES
