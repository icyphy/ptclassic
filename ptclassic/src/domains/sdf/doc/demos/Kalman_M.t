.\" $Id$
.NA Kalman_M
.SD
This demo compares the convergence properties of a Kalman filter to
those of an LMS filter when addressing the problem of adaptive equalization
of a process in noise.
.EQ
delim off
.EN
.LO "$PTOLEMY/src/domains/sdf/matrix/demo"
.DM SDF Universe
.SV $Revision$ $Date$
.AL "Mike J. Chen"
.EQ
delim $$
.EN
.LD
.Se Kalman_M
.Ir "Kalman Filter"
.Ie "linear equalization"
.Ie "adaptive filter"
.Ie "filter, adaptive"
.Ie "filter, LMS adaptive"
.Ie "filter, Kalman adaptive"
The Kalman filter has faster convergence properties than an LMS filter.
The plot that is generated compares the power of the error process for
the two filters.
The
.c Kalman_M
star is used to generate the taps of a block FIR filter.
.pp
The Kalman filter is a state-space method.
The
.c Kalman_M
star takes on data input value, along with a state transition matrix,
a measurement matrix, a process noise correlation matrix, and a measurement
noise correlation matrix.
Initially, the data input values are delayed using the 
.c Trainer
star to allow the 
.c powerEst
star to stabilize.  The plot shows that both filters generate the same
error power initially.
.pp
The demo runs through a large number of iterations, about a third of
which is the delay time to allow the error to stabilize.  If the
\fItrainLength\fR parameter of the
.c Trainer
star is set to zero, the Kalman filter performance converge before
the power estimation star could stabilize, giving us a less accurate
representation of the relative performance.
.pp
If demo is run for an even longer time, the output plot would eventually
show the LMS filter converging to the same minimum mean square error as
the Kalman filter.
.Ir "Kalman, R.E."
.Ir "Haykin, S."
.UH REFERENCES
.ip [1]
R.E. Kalman, "A new approach to linear filtering and prediction problems",
\fITrans. ASME, J. Basic Eng.\fR, Ser 82D, pp. 35-45, March 1960.
.ip [2]
S. Haykin, \fIAdaptive Filter Theory\fR, Prentice-Hall, Inc., Englewood Cliffs,
N.J., ISBN 0130040525, 1986.
.SA
Kalman_M,
LMS.
.ES
