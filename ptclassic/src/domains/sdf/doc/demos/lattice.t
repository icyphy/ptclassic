.\" $Id$
.NA lattice
.SD
Demonstrate the use of lattice filters.
.DE
.SV $Revision$	$Date$
.AL "E. A. Lee"
.LO "~ptolemy/src/domains/sdf/demo"
.LD
.Ie "whitening filters"
.Ie "lattice filter"
.Ie "filter, lattice"
This demo generates an auto-regressive (AR) random process by filtering
Gaussian white noise with an all-pole filter with transfer function
.EQ
H(z) ~=~ 1 over { 1 ~-~ 2 z sup -1 ~+~ 1.91z sup -2 ~-~ 0.91z sup -3 ~+~
0.205z sup -4 } ~.
.EN
.Ie "allpole filter"
.Ie "filter, allpole"
The filter is implemented two ways.
First, it is impmented with an FIR filter in a feedback loop.
Then, in parallel, it is implemented with a recursive lattice filter,
using the
.c RLattice
star.
.Ie "RLattice"
The transfer function of the lattice filter is also $H(z)$, so
the outputs should be identical.
The resulting AR process is then filtered
using an FIR lattice (the
.c Lattice
star) with the same coefficients as in the recursive lattice.
.Ie "Lattice"
This has transfer function $H sup -1 (z)$, which whitens the
random process.  The power spectrum for each process is then
estimated using the
.c autocorrelation
galaxy.
.SA
latticeDesign,
Lattice,
RLattice.
.ES
