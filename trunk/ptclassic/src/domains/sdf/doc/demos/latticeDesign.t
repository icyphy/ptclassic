.\" $Id$
.NA latticeDesign
.SD
Use of Levinson-Durbin algorithm to design a lattice filter with a specified
transfer function.
.DE
.LO "$PTOLEMY/src/domains/sdf/demo"
.DM SDF Universe
.SV $Revision$	$Date$
.AL "Edward A. Lee"
.LD
.Ie "filter design, lattice"
.Ie "lattice filter design"
This demo generates the impulse response of an all-pole
(autoregressive, AR) filter using an
.c FIR
filter in a feedback loop,
and the uses the Levinson-Durbin algorithm to design a lattice
.Ir "Levinson-Durbin algorithm"
filter with the same transfer function and the inverse transfer function.
The transfer function of the AR filter is
.EQ
H(z) ~=~ 1 over { 1 ~-~ 2 z sup -1 ~+~ 1.91z sup -2 ~-~ 0.91z sup -3 ~+~
0.205z sup -4 } ~.
.EN
A biased autocorrelation estimate is computed and fed into the
.c LevDur
star.
.Ir "autocorrelation"
.Ir "biased autocorrelation"
The FIR and lattice predictor coefficient outputs of the
LevDur star are plotted.
.Se LevDur
The FIR predictor coefficients should correspond exactly
to the negative of all but the first coefficient in the denominator
of $H(z)$ above.
The lattice predictor coefficients are loaded into the
.c BlockLattice
and
.c BlockRLattice
stars
.Se BlockLattice
.Se BlockRLattice
The
.c BlockLattice
star therefore has transfer function $H sup -1 (z)$.
To verify this, the impulse response of the AR filter is fed into it,
producing an impulse at the output.
The
.c BlockRLattice
star will have transfer function $H(z)$.
This is verified by plotting its impulse response together with that
of the AR filter.
They should be identical, so the two plots should exactly overlap.
.SA
Autocor,
BlockLattice,
BlockRLattice,
Lattice,
lattice,
LevDur,
RLattice.
.ES
