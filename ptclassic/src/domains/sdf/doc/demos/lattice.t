.\" $Id$
.NA lattice
.SD
Demonstrate the use of lattice filters to synthesize an
auto-regressive (AR) random process.
.DE
.LO "$PTOLEMY/src/domains/sdf/demo"
.DM SDF Universe
.SV $Revision$	$Date$
.AL "Edward A. Lee"
.LD
.Ie "whitening filter"
.Ie "filter, whitening"
.Ie "lattice filter"
.Ie "filter, lattice"
This demo generates an auto-regressive (AR) random process by filtering
Gaussian white noise with an all-pole filter with transfer function
.EQ
H(z) ~=~ 1 over { 1 ~-~ 2 z sup -1 ~+~ 1.91z sup -2 ~-~ 0.91z sup -3 ~+~
0.205z sup -4 } ~.
.EN
.Ie "all-pole filter"
.Ie "filter, all-pole"
The filter is implemented two ways.
First, it is impmented with an FIR filter in a feedback loop.
Then, in parallel, it is implemented with a recursive lattice filter,
using the
.c RLattice
star.
.Se RLattice
The transfer function of the lattice filter is also $H(z)$, so
the outputs should be identical.
The resulting AR process is then filtered
using an FIR lattice (the
.c Lattice
star) with the same coefficients as in the recursive lattice.
.Se Lattice
This has transfer function $H sup -1 (z)$, which whitens the
random process.
The power spectrum for each process is then estimated using the
.c autocorrelation
galaxy.
.SA
latticeDesign,
Lattice,
RLattice.
.ES
