.VR 0.$Revision$ "$Date$"
.TI "SDF Domain"
.AU
J. T. Buck
E. A. Lee
D. G. Messerschmitt
.AE
.H1 "Introduction"
.pp
Synchronous dataflow (SDF)
.Id "synchronous dataflow"
.Id "SDF"
.Id "SDF domain"
.Id "domain, SDF"
is a data-driven, statically scheduled domain in \*(PT.
It is a direct implementation of the techniques given in [1] and [2].
"Data-driven" means that the availability of
.c Particle s
at the inputs of a star enables it.
Stars with no input ports are always enabled.
"Statically scheduled" means that the firing order of the stars
.Id "statically scheduled"
is determined once, during the startup phase.
The firing order will be periodic.
.H2 "Delays"
.pp
In the SDF domain, a unit delay is simply an initial particle on an arc.
.Id "delays, SDF"
.Id "SDF delays"
This initial particle may enable a star, assuming the star requires
one particle in order to fire.
Hence, to avoid deadlock, all feedback loops much have delays.
The SDF scheduler will flag the error if it finds a loop with no delays.
There is currently no mechanism for setting the value of the initial
particle.
For most particle types, the initial value will be zero.
.H2 "Number of iterations"
.pp
When running an SDF universe or wormhole, the number of iterations
.Id "iterations (SDF)"
.Id "SDF iterations"
may not be the same as the number of times a star in the system is fired.
The number of invocations of each star depends
on all the stars in the universe or wormhole.
In order to be able to repeat the iterations, each iteration
should leave all arcs (geodesics) in the same state after the iteration ends
that they were in before the iteration began.
In other words, the total number of particles stored
on an arc should be the same after the iteration as before.
Furthermore, every star should fire at least once in an iteration.
These two requirements together determine the number of firings
of a star in an iteration.
.pp
Consider for example a universe A-->FFT-->B,
where FFT is the
.c ComplexFFT
star.
.Ie "FFT"
This star consumes some number of particles (given by its \fIsize\fR parameter)
and produces some number of tokens ($2 sup n$, where
$n$ is its \fIorder\fR parameter).
Hence, one iteration consists of \fIsize\fR firings of A, one firing of FFT,
and $2 sup n$ firings of B.
No fewer invocations would satisfy the above requirements.
Note therefore that if you wish to compute only one FFT,
then this system should be run through just one iteration.
.H2 "Inconsistencies"
.pp
It is not always possible to find a consistent number of firings
for all stars in the system that returns the geodesics to their
original state.
.Id "inconsistences, SDF"
.Id "SDF inconsistences"
For instance, suppose that the input and output of the ComplexFFT
are added together using a ComplexAdd star.
Also suppose that $2 sup n ~!=$ \fIsize\fR.
Then no periodic schedule is possible.
One interpretation is that we have tried to add two signals with
different sample rates.  The SDF scheduler detects this error,
and refuses to run the system.
It is sometimes possible to run the system under the DDF domain.
.H1 "Writing SDF stars"
.pp
.Id "writing SDF stars"
.Id "SDF, writing stars"
All stars in the SDF domain
must follow the basic SDF principle:
the number of
particles consumed or produced on any
.c Porthole
does not change while the simulation runs.
These numbers are given for each porthole
as part of the star definition.
Most stars consume just one particle on each input and produce
just one particle on each output.
In these cases, no special action is required, since the porthole
SDF properties will be set to unity by default.
However, if the numbers differ from unity, the star definition
must reflect this.
For example, the
.c ComplexFFT
star has a \fIsize\fR state (parameter) that
specifies how many input samples to read.
The value of that state specifies the number of samples
required at the input in order for the star to be enabled.
The following line in the start() function of the star
is used to make this information available to the scheduler:
.(b
	input.setSDFParams (int(size), int(size)-1);
.)b
.Id "SDF parameters"
.Id "setSDFParams"
.IE "past particles"
.IE "consumed particles"
.IE "produced particles"
The name of the input porthole is "input".
The first argument to \fIsetSDFParams\fP specifies how many samples
are consumed by the star when it fires, which is the same
as the number of samples required in order to enable the star.
The second argument to \fIsetSDFParams\fR specifies how many past
samples (before the most recent one) will be accessed by the star
when it fires.
.pp
If the number of particles produced or consumed is a constant
independent of any states, then it may be declared right along
with the declaration of the input.  For example,
.(c
input {
	name { signalIn }
	type { complex }
	numtokens { 2 }
	desc { A complex input that consumes 2 input particles. }
}
.)c
declares an input that consumes two particles.
.H1 "Accessing inputs and Outputs"
.pp
The mechanism for accessing inputs and outputs is explained in the
section of the Almagest on the \*(PT preprocessor language.
.H1 "The Scheduler"
.pp
The SDF scheduler determines the order of execution of stars
in a system at start time.
.Id "SDF scheduler"
.Id "scheduler, SDF"
It performs most of its computation during its setup() phase.
It exactly implements the method described in [1] for sequential schedules.
.UH "References"
.ip [1]
E. A. Lee and
D. G. Messerschmitt,
"Static Scheduling of
Synchronous Data Flow Programs for Digital Signal Processing",
\fIIEEE Trans. on Computers\fR,
January 1987,
\fBC-36(2)\fR.
.ip [2]
E. A. Lee and
D. G. Messerschmitt,
"Synchronous Data Flow"
\fIIEEE Proceedings\fR,
September, 1987.
