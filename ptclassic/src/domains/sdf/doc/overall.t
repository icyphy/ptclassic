.VR 0.$Revision$ "$Date$"
.TI "SDF Domain"
.ds DO "SDF
.AU
J. T. Buck
E. A. Lee
D. G. Messerschmitt
S. Ha
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
.Id "static scheduling"
.Id "scheduling, static"
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
.Id "iterations, SDF"
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
.c FFTCx
star.
.Sr FFTCx
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
.Id "inconsistent SDF system"
.Id "SDF system, inconsistent"
For instance, suppose that the input and output of the FFTCx
are added together using a AddCx star.
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
.c FFTCx
star has a \fIsize\fR state (parameter) that
specifies how many input samples to read.
The value of that state specifies the number of samples
required at the input in order for the star to be enabled.
The following line in the start() function of the star
is used to make this information available to the scheduler:
.(c
	input.setSDFParams (int(size), int(size)-1);
.)c
.Id "SDF parameters"
.Id "setSDFParams"
.IE "past particles, SDF"
.IE "consumed particles, SDF"
.IE "produced particle, SDF"
.IE "particles, SDF"
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
	numTokens { 2 }
	desc { A complex input that consumes 2 input particles. }
}
.)c
declares an input that consumes two particles.
.Ie "numTokens"
.H1 "Accessing Inputs and Outputs"
.pp
The mechanism for accessing inputs and outputs is explained in the
section of the Almagest on the \*(PT preprocessor language.
.IE "inputs and outputs, accessing, SDF"
.H1 "The Scheduler"
.pp
The SDF scheduler determines the order of execution of stars
in a system at start time.
.Id "SDF scheduler"
.Id "scheduler, SDF"
It performs most of its computation during its setup() phase.
The default scheduler
exactly implements the method described in [1] for sequential schedules.
If there are sample rate changes in a program graph, some parts of the
graph are executed multiple times, which may be looped. 
The default scheduler does not consider the looping possibility and
generates a list of blocks to be executed. For example, if star A is
executed 100 times, the generated schedule includes 100 instances of A.
.Id "loop scheduler, SDF"
.Id "SDF loop scheduler"
A loop scheduler will include only one instance on A and indicate 
the repetition count of A somehow, say 100(A).
For simulation, it might be tolerable, but not in the code generation
(The SDF schedulers are also used in the code generation for a 
single processor target). 
.pp
By setting the
.c loopScheduler
.Ir "loopScheduler, parameter"
.Id "Joe's scheduling"
target parameter, we can select a scheduler developed by J. Buck.
Before applying the default scheduling algorithm, J. Buck clusters
blocks as long as the program graph is dead-locked. 
This clustering algorithm consists of 
the "merging" step and the "looping" step repeatedly. 
In the merging step, blocks connected together 
are merged into a cluster if there is no sample rate change between
them. In the looping step, a cluster is looped to make it possible to 
be merged with the neighbor blocks or clusters.
Since this looping algorithm is conservative, some complicated looping
possibilities, if rare,  are not disclosed.
.pp
The more complicated looping algorithm was developed by three colleagues,
called "SJS" (Shuvra-Joe-Soonhoi) scheduling.
.Id "SJS scheduling"
We apply Joe's scheduling as the first pass. In the
second pass, we decompose the graph (S. Bhattacharyya's contribution) so
that the graph becomes acyclic. The decomposed graphs are
expanded to acyclic precedence graphs in which looping structures are
extracted (S. Ha's contribution). This scheduling option is selected
when the
.c loopTarget
.Ir "loopTarget, class"
is chosen instead of the default SDF target.
.pp
The looping result can be seen by setting the
.c logFile
target parameter. That file will contain all the intermediate 
procedures of looping and the final scheduling result.
The loop scheduling algorithms are usually used in code generation domain
not in the simulation SDF domain. Refer to the CG domain documentation
for detailed discussion.
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
