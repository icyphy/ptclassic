.\" $Id$
.NA plldemo
.SD
Simulation of an optical phase-locked loop.
.DE
.LO "~ptolemy/src/domains/sdf/demo"
.DM SDF Universe
.SV $Revision$	$Date$
.AL "John R. Barry"
.LD
.IE "analog systems"
.IE "continuous-time systems"
.IE "phase-locked loops"
.IE "PLL"
.IE "optical communications"
.IE "optical heterodyne"
This is a discrete-time model of a continuous-time system.  Specifically,
the system is a fourth-power optical phase-locked loop suitable for
synchronous detection of heterodyne optical QPSK.
Simulation of such a phase-locked loop is hampered by two obstacles:
(1) the signals are continuous-time, and (2) the signals are passband,
some as high as 200 THz.  We hide the passband nature of the signals 
by representing each signal by its complex-valued baseband envelope.
Moreover, we use the samples of each envelope to convert to discrete
time.  The continuous-time filters were converted to discrete time using
the impulse-invariant method.   For example, a continuous-time filter
with impulse response $h(t)$ is represented by a discrete-time
filter $h sub k$, where $h sub k ~=~ T h(k T)$ and $T$ is
the time step.  Similarly, a continuous-time integrator is represented
by a discrete-time integrator scaled by $T$.
A continuous-time white noise with power-spectral density $N sub 0$ is
represented by a discrete-time white random process with power-spectral
density $N sub 0 / T$.
.pp
There are two sources of noise in an optical phased-lock loop: laser
phase noise and shot noise.
The laser phase noise is modeled as a Brownian motion process, which
is generated by passing a white Gaussian noise process through an 
integrator.  The shot noise in heterodyne detection is of extremely 
high intensity and thus is accurately-modeled as white Gaussian noise.
The phase detector simply raises the QPSK signal to the fourth power,
effectively wiping out the QPSK modulation and isolating the phase error.
This phase-error estimate is passed through a loop filter to generate
a control signal which is fed into the VCO (voltage controlled oscillator).
.IE "VCO"
.IE "voltage controlled oscillator"
.pp
The first graph plots the input phase (laser phase noise) and output
phase versus time.  The input phase takes a random walk, so its variance
grows linearly with time.  The output phase settles near zero degrees,
with a standard deviation of about 3 degrees.  The standard deviation
of the steady-state phase error is a critical performance parameter,
and the primary purpose of these simulations is to determine this
standard deviation.  The last graph plots the estimated standard
deviation versus time; for a sufficiently large number of iterations,
this estimate will get arbitrarily close to its actual value.
.pp
The histogram estimates the probability density function of the phase error,
which (at high signal-to-noise ratios) should approach Gaussian for a
sufficiently large number of iterations.  The performance of the entire
QPSK system can be estimated visually by inspecting the in-phase and
quadrature eye diagrams.
.UH "REFERENCES"
.ip [1]
J. Barry and J. Kahn,
.i "Carrier Synchronization for Heterodyne Detection of Optical Quadriphase-Shift Keying,"
submitted to J. Lightwave Technology, manuscript 1213, February 1992.
.SA
.ES
