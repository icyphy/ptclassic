.\" $Id$
.NA timing
.SD
This demo illustrates the use of the DDF domain
to implement asynchronous signal processing systems.
In this case, the system performs baud-rate timing
recovery using an approximate minimum mean-square-error
(MMSE) technique.
.DE
.LO "$PTOLEMY/src/domains/ddf/demo"
.SV $Revision$ $Date$
.AL "E. A. Lee"
.LD
.Ie "timing recovery, MMSE"
.Ie "MMSE timing recovery"
.Ir "asynchronous signal processing"
The approximate minimum mean-square-error
timing recovery technique is described in [1].
.Ir "amplitude shift keying"
.Ir "ASK"
.Ir "modulation, amplitude shift keying"
An amplitude-shift keyed (ASK) signal is generated by
the
.c ask
galaxy on the left.
The modulation format is a simple, baseband, binary-antipodal signal
with a 100% excess bandwidth raised cosine pulse.
.Ir "raised cosine pulse"
The sample rate is eight times the baud rate, as controlled by the
\fIdecimation\fR and \fIinterpolation\fR parameters of the pulse
shaping FIR filter.
Other ratios can be easily generated to test the convergence of the
timing recovery.
.pp
The derivative of the ASK signal is estimated using an FIR filter
in the top-level facet (the universe).
A second-order difference is used as an estimate.
The derivative and the signal sample itself are
then either discarded (about 7 out of 8 times), or passed on
to the baud rate subsystem (about 1 out of 8 times).
This baud-rate subsystem estimates the timing error
and uses this estimate to control a phase locked loop (PLL).
.Ir "phase locked loop"
.Ir "PLL"
The estimate of the timing error is formed by multiplying
the estimated derivative by the error across the decision device
(the slicer).
.pp
The timing error estimate is upsampled to the sample rate
of the input signal by interleaving zero-valued samples.
Then it is used to adjust a PLL implemented in the
.c PLLControl
galaxy.
In there, a loop filter realizes a second order PLL.
The voltage-control oscillator (VCO) is implemented using an integrator
with its \fIsaturate\fR parameter set to NO and a wraparound detector
that issues a Boolean TRUE each time the integrator overflows and wraps around.
That Boolean TRUE then triggers a sample taken from the input
signal and the estimated derivative, thereby closing the loop.
.pp
The universe in this system is in the DDF domain because of the
dynamically controlled 
.c Case
and 
.c EndCase
stars.
The three galaxies, however, are all actually wormholes, internally
using the SDF domain.
This results in much more efficient execution, since the scheduling
decisions within these galaxies can be made statically.
More information is given in [2].
.UH REFERENCES
.ip [1]
E. A. Lee and D. G. Messerschmitt,
\fIDigital Communication\fR, Kluwer Academic Press,
Norwood, MA, 1988.
.ip [2]
J. Buck, S. Ha, E. A. Lee, and D. G. Messerschmitt,
"Multirate Signal Processing in Ptolemy",
\fIProc. of the Int. Conf. on Acoustics, Speech,
and Signal Processing\fR, Toronto, Canada, April, 1991.
.SA
Select.
.ES
