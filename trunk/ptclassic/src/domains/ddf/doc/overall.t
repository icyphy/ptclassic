.\" $Id$
.VR 0.$Revision$ "November 16, 1990"
.TI "DDF Domain"
.AU
Soonhoi Ha
.AE
.ds DO DDF
.H1 "Introduction
.pp
The dynamic dataflow (DDF) domain in \*(PT provides a general
.IE "dynamic dataflow"
.IE "DDF domain"
environment for dataflow computation and modeling of the systems.
It is a superset of the synchronous dataflow (SDF) domain.
In the SDF domain, a 
.c Star
consumes and produces a fixed number of
.c Particle s
per each execution.  The deterministic information on the
number of particles
for each star
makes possible a compile-time scheduling.  In the \*(DO domain, 
however, a star
may consume or produce varying number of particles
for each execution.  Therefore, the efficient compile-time
scheduling is no longer applicable.  Instead,
.c DDF\ Scheduler
detects which stars
are runnable at runtime and fires them one by one until no
star is runnable (the system is deadlocked).
.pp
Since the \*(DO domain is a superset of the SDF domain,
all
.c SDF\ Star s
are perfectly reasonable stars
in the \*(DO domain. Besides the
.c SDF\ Star s,
the \*(DO domain has some \*(DO-specific stars
which will be discussed in this document.
The \*(DO-specific stars
overcome the modeling limitation of
the SDF domain.  They can
model the dynamic constructs such as
.IE "dynamic construct"
\fIconditionals\fR, \fIdata-dependent iterations\fR,
and \fIrecursions\fR.
The runtime efficiency of the dynamic scheduling is the cost
that we have to pay for the enhanced modeling power. But,
\*(PT enables us to sacrifice the efficiency little by grouping
the 
.c SDF\ Star s
as a DDF wormhole
which contains a SDF domain.
.sp
.H1 "The \\*(DO Scheduler
.pp
The scheduler in \*(PT determines the order of execution of
blocks, and also expects a particular behavior on the part of stars
within the \*(DO domain.
In this section, we describe the operation of the \*(DO scheduler.
.IE "DDF Scheduler"
.pp
The basic operation of the DDF scheduler
is to repeatedly scan the list of stars
in the domain and execute the runnable stars
until no more stars
are runnable.
There are three groups of runnable conditions for stars
in the \*(DO domain.
The first group of the stars
require the fixed number of particles
on all input arcs.  Most
.c SDF\ Star s
are included in this group.  The scheduler
checks all input
.c PortHole s
for this group of stars.
.pp
The second group of stars
are the \*(DO-specific stars
that requires non-deterministic number of particles
on the input ports.  An example is the 
.c EndCase 
star.  The 
.c EndCase 
star has one control input and one multiport input for data.
The control port receives the control value. 
Depending the control value, 
the star
selects one input port and routes it
to the output port.  Therefore, the number of particles
required for the non-control input ports 
is dependent on the control value.
Such stars
have members called \fIwaitPort\fR and \fIwaitNum\fR.
.IE "waitPort, ddf"
.IE "waitNum, ddf"
To determine whether a star is runnable,
the scheduler checks whether the input arc
pointed by the \fIwaitPort\fR has as many particles
as \fIwaitNum\fR.
In the
.c EndCase 
star,
the \fIwaitPort\fR points to the \fIcontrol\fR input port at the beginning.
If the \fIcontrol\fR input has enough data, the star
is fired. When it is fired, it checks the value of the particle
in the \fIcontrol\fR port, 
and change the \fIwaitPort\fR pointer to the input port
which is supposed to be routed to the output port. 
Then, the star
is fired again if it has enough data on the input port pointed
by the \fIwaitPort\fR.  This time, it actually routes the input
port to the output port.  Another example will be shown in the section 4.
.pp
The third group of stars comprises sources.
Since sources
are always runnable, they makes the system
never deadlocked so that the scheduler will never stop.  
Therefore, we should limit the number of
firing of those stars
in some way.  We also regard stars with initial particles
on all input arcs, as sources.
Recall again that the \*(DO domain is a superset of 
the SDF domain.  It implies that with a given
system topology which contains only
.c SDF\ Star s,
we may run the SDF scheduler
or the DDF scheduler
to get the exactly same results.
In the SDF domain, the number of firings of each source star
is determined after a fairly complicated analysis of the system
topology at the compile time.  To make matters worse in the \*(DO
domain, the non-deterministic behavior of the \*(DO-specific stars
excludes the possibility of using similar complicated analysis
to determine the number of firings of the sources.
The technique we use is the \fIlazy-evaluation\fR technique.
.IE "lazy evaluation"
.pp
In the \fIlazy-evaluation\fR technique, we fire a star
when the successor requires any data from it.
At the beginning of each run of the DDF scheduler, 
we fire all source stars only once, and declare them as not runnable
afterwards.  After that, the scheduler starts scanning the list of stars
in the domain.  If a star
has some particles
on some input arcs, but not runnable yet, then the star
initiates the lazy-evaluation of its ancestors which are connected
to the input ports
expecting more data.  
The lazy-evaluation technique magically generates the same\**
.(f
The result is not exactly same in that the order of execution of stars
may be different.  But, the number of firings of each star is same
in one iteration.
.)f
scheduling result as the SDF scheduler
for any
.c Galaxy consisting of only
.c SDF\ Star s.
Moreover, it clearly defines a "run" (iteration) of the
.IE "iteration, ddf"
DDF scheduler.  Suppose a
.c Wormhole
contains the \*(DO domain.  And, the outer domain
sends the input data into the wormhole
and waits for the output data from it.  With given inputs,
the DDF scheduler
would run once on the \*(DO domain.  Then, a single run
of the DDF scheduler
should generate the output data requested from the outer domain. 
A "run" of the DDF scheduler
means that it fires all source stars once beforehand and repeats scanning
the list of stars
with lazy-evaluation technique until no more is runnable.
Note that one iteration of the DDF scheduler will generate the
same results as one iteration of the SDF scheduler if a given
system has only SDF stars.
.pp
Up to now, we assume that we are given an error-free programs.
In the SDF domain, the complicated compile scheduling may detect
the errors and running the program very efficiently at runtime.
On the other hand, in the DDF domain, the sources of errors are
more diversified and very difficult to be detected at compile time.
Instead, we would detect the errors at runtime relatively easily.
The runtime detection of errors has a couple of disadvantages.
First, it is a very slow mechanism, which will be explained shortly.
Second, it is not easy to isolate the sources of errors since we
are just examining the apparent situation.
.pp
We can think of two types of errors.  One is infinite waiting.
If a program has a delay-free loop, the lazy evaluation stages
will be spinning forever.  We can detect this type of error
rather simply by  limiting the depth of the lazy evaluation.
.IE "inconsistency"
.IE "consistent graph"
The other is inconsistency.  We call a dataflow graph \fIconsistent\fR
if on each arc, in the long run, the same number of tokens are
consumed as produced.  If a dataflow graph is not consistent, it
is called (strongly) inconsistent\**.  
.(f
E. A. Lee, "Consistency in Dataflow Graphs," revised from
MEMO UCB/ERL M89/125, November 9, 1989, to appear in
\fIIEEE Transactions on Parallel & Distributed Computing\fR, 1991.
.)f
One source of inconsistency
is the sample-rate mismatch which is common to the SDF domain.
The DDF domain has more error-sources due to the dynamic behaviour
of the 
.c DDF\ Star s.
In an inconsistent graph, some arc queues an unbounded number
of tokens in the long run.  Therefore, we examine the number
of tokens on each arc to detect whether the number is greater than
a certain limit (default 1024).  If we find an arc with too many tokens,
we consider it as an error condition and signal an error.
We can specify the limit by defining a state
\fImaxBufferSize\fR to the DDF galaxy.
.IE maxBufferSize, ddf
Since the source of inconsistency is not unique, the isolation of
the error is also improbable.  We can just point out which
arc has unbounded number of tokens.  What happens if the
limit is too high?  Some errors would takes very long to
build tokens more than the limit on an arc.  Thus, the runtime
error detection is very slow mechanism.
The enhanced modeling power of the DDF domain has another cost :
inefficient error detection.  Therefore, we suggest the user
to check the program carefully before running it in the DDF domain.
.sp
.H2 "COMPILE-TIME SCHEDULING OF DYNAMIC CONSTRUCTS
.pp
The runtime overhead of dynamic scheduling technique is usually 
very high.  Also, consistency checking process is too slow.
Fortunately, a compile-time scheduling technique has been proposed\**
for well-known dynamic constructs such as \fIif-then-else\fR, \fIfor\fR,
\fIdo-while\fR, and \fIrecursion\fR.
.(f
S. Ha and E. A. Lee, "Compile-time Scheduling and Assignment of Dataflow
Program Graphs with Data-Dependent Iteration",
\fIto appeare in IEEE trans. on Computers\fR.
.sp
S. Ha and E. A. Lee, "Quasi-Static Scheduling for Multiprocessor DSP",
\fIISCAS\fR, 1991.
.)f
.pp
Each dynamic construct is associated with a dedicated scheduler that
checks consistency of the graph at compile-time and generates a
quasi-static schedule.  For example, the scheduler dedicated to 
\fIif-then-else\fR construct generates two star-lists for "true" branch
and "false" branch respectively.  The star-lists correspond to the
compile-time schedules.  At runtime, the scheduler examines the
control value to the construct, then choose the star-list of the chosen
branch.  By doing this, the runtime decision (also overhead) is minimized.
Similarly, a dynamic construct is maximally scheduled at compile-time
except unavoidable runtime decisions.
.pp
Currently, a dynamic construct is identified automatically by following
steps.  First, DDF wormholes of SDF domain are created for connected
SDF stars.  After this step, the DDF scheduler can see only DDF stars
and DDF wormholes of SDF domain.  Now, SDF stars can be scheduled
statically.  Second, the topology of the DDF system is compared
with those of predefined dynamic constructs.  
If a match is found, perform the consistency checking procedure.
Third, replace the DDF scheduler with the dedicated scheduler.
If there is no match, the original dynamic scheduler is invoked.
Four dynamic constructs have been implemented for now.
In DDF domain, a galaxy became a wormhole (
.c DDFDomain
class redefines a method called \fIisGalWorm()\fR to return TRUE.
By default, this method returns FALSE : we compare the outer and the
inner domain if a galaxy, and create a wormhole if they are different.)
This option is helpful to realize a structure of nested dynamic constucts.
If a DDF subsystem is defined as a galaxy, the subsystem can
be identified as a dynamic construct.
.pp
On this line, more changes are expected on the following issues:
.ip 1
Flexibility to add new dynamic constructs without changing the kernels
of the DDF domain.
.ip 2
Systematic method to identify a dynamic construct, rather than crude
topology matching idea.
.sp
.H1 "Programming Stars in the \\*(DO Domain
.pp
There is an attempt to maintain uniformity across
different domains in \*(PT.
However, in each domain there are typically special needs
for the writers of stars
in that domain.
This section describes how \*(DO-specific stars
are programmed in the \*(DO domain.
For
.c SDF\ Star s,
refer to the SDF section of the manual.
.pp
A
.c DDF\ Star
.IE "DDF Star"
has at least one porthole,
either input or output, that receives or sends a non-deterministic
number of particles.
Such portholes
are called \fIdynamic\fR.
.IE "dynamic porthole"
For the
.c DDF\ Star ,
how many particles
to read or write is determined at runtime, in the \fIgo()\fR method.
Let's look at an example\** :
.c LastOfN\ 
star.
.(f
This is a preprocessor format of the star.  Refer to the ptlang section
of the manual for the preprocessor language.  Also some lines are omitted
since they are not relevant to the discussion.
.)f
.(c
defstar {
	name {LastOfN}
	domain {DDF}
	desc {
Outputs the last token of N input tokens,
where N is the control input.
	}
	input {
		name {input}
		type {anytype}
		num  {0}
	}
	input {
		name {control}
		type {int}
	}
	output {
		name {output}
		type {anytype}
	}
	private {
		int readyToGo;
	}
        method {
                name { readClassName }
                access { public }
                type { "const char *" }
                code { return "DownSample"; }
        }
	constructor {
		input.inheritTypeFrom(output);
	}

	start {
		waitFor(control);
		readyToGo = FALSE;
	}

	go {
	   // get control data
	   if (!readyToGo) {
                control.grabData();
                int i = int (control%0);
                waitFor(input, i);
                readyToGo = TRUE;
           } else {
                // skip the N-1 inputs
                int num = int (control%0);
                for (int i = num; i > 0; i--)
                        input.grabData();

                // prepare for the next round.
                waitFor(control);
                readyToGo = FALSE;

                // copy last input to output
                if (num < 1) return;
                output%0 = input%0;

                // generate output, and get ready for next firing
                output.sendData();
           }
	}
}
.)c
The
.c LastOfN
star
discards the first \fIN-1\fR particles 
from the \fIinput\fR porthole
and routes the last one to the \fIoutput\fR porthole .
The value \fIN\fR is read from the \fIcontrol\fR input.
Since the control data is varying, the number of particles
to read from the \fIinput\fR porthole
is non-deterministic, which makes this star a
.c DDF\ Star .
We can specify that the \fIinput\fR porthole
is \fIdynamic\fR by setting the 
.c num
field of the
.c input
declaration to be \fI0\fR in the preprocessor format :
.(c
num {0}
.)c
The firing condition of the star is controlled by the
\fIwaitPort\fR, and \fIwaitNum\fR members of the
.c DDF\ Star
as discussed in section 2.  The \fIwaitFor()\fR method
sets these members.
This star
has also a 
.c private
member called \fIreadyToGo\fR. The \fIreadyToGo\fR flag
is set TRUE when the \fIwaitPort\fR points to the \fIinput\fR porthole .
Initially, the star
waits for the control data, and sets \fIreadyToGo\fR flag FALSE
(see the above).  When it is fired, it first checks the flag.
If the flag is set FALSE, meaning that it waits for the control data,
it reads the control data from the \fIcontrol\fR input and decides
how many particles
it needs from the \fIinput\fR porthole . 
Then, it changes the \fIwaitPort\fR pointer to the \fIinput\fR porthole
with appropriate value for the \fIwaitNum\fR, and set the
\fIreadyToGo\fR flag TRUE.  
If the flag is set TRUE when fired, it means that
the \fIinput\fR porthole 
has enough particles.
Therefore, it discards the first \fIN-1\fR inputs and copies the
last input to the output port.  Then, it goes into the next round :
sets the \fIwaitPort\fR pointer to the \fIcontrol\fR porthole
and set the \fIreadyToGo\fR flag FALSE.
.pp
The next example is a 
.c DDF\ Star
with a dynamic output porthole:
.c DownCounter 
star.
.(c
defstar {
	name {DownCounter}
	domain {DDF}
	desc { A down-counter from input value to zero. }
	input {
		name {input}
		type {int}
	}
	output {
		name {output}
		type {int}
		num  {0}
	}
        method {
                name { readClassName }
                access { public }
                type { "const char *" }
                code { return "UpSample"; }
        }
	go {
	   // get input data from Geodesic
	   input.grabData();

           // generates output
           int in = int (input%0);
           for (int i = in - 1 ; i >= 0; i--) {
                output%0 << i ;
                output.sendData();
           }
	}
}
.)c
The
.c DownCounter 
star has a dynamic \fIoutput\fR porthole
which will generate the down-counter sequence of integer data
starting from the value read through the \fIinput\fR porthole .
The code in the \fIgo()\fR method is self-explanatory.
.H2 "DDFSelf Star For Recursion
.pp
There is a special star
in the \*(DO domain to realize the recursion construct :
.IE "recursion"
.c DDFSelf 
star.
.IE "DDFSelf Star"
At compile time, it is just an atomic star.
But, if it is fired at runtime, it creates a galaxy
dynamically, executes and destroys the galaxy.
The galaxy
is a clone of the galaxy named by the \fIrecurGal\fR parameter.
The
.c DDFSelf
star has a
.c private
member, \fIgal\fR, that points to the recursion-galaxy. 
The \fIgal\fR pointer is set at compile time by matching
the name of the galaxy
with the 
.c StringState\ recurGal
which is given by the programmer.
.pp
At runtime, the
.c DDFSelf
star creates a clone galaxy of the recursion-galaxy pointed by the \fIgal\fR
pointer.  The galaxy may be a clone of the one containing the
.c DDFSelf
star, achieving recursion.
It has its own DDF scheduler as a member which will operate on the
created galaxy.
The portholes
of the
.c DDFSelf
star are connected not only to the other stars
but also to the galaxy
just created.
After connections are set up, it receives the input data from
the outside and pass them in to the galaxy.
It runs the DDF scheduler
in the galaxy
and gather-and-sends the output data from the galaxy 
to the outside.
Finally, it destroys the galaxy which it created.
The runtime efficiency is quite low since a lot of memory
management is involved in the creation and deletion of
the galaxy
and the DDF scheduler.
Further research is in progress to increase the performance.
.H1 "EventHorizon in the \\*(DO Domain
.pp
The mixture of the \*(DO domain with other domains
requires a conversion between different computational
models.
In particular, some domains associate a \fItime stamp\fR
with each particle,
and other domains do not.
Thus, a common function at the
.c EventHorizon
is the addition of time stamps, the stripping off of
time stamps, interpolation between time stamps,
or removal of redundant repetitions of identical particles,
and so forth.
.pp
The general mechanism of the domain interface is described in
the document, "\fBDomains in Ptolemy\fR".
The standard functions of
.c EventHorizon s
are also fully discussed in the document.  Refer to the
document for more detailed explanations on the domain interface.
In this section, \*(DO-specific features on the domain
interface will be discussed.
.H2 "DDF Wormhole
.pp
.IE "DDF Wormhole"
The \*(DO domain may have a wormhole which contains another domain.
An input port of a DDF wormhole
consists of a 
.c DDFtoUniversal\ EventHorizon
and a
.c (in-domain)fromUniversal\ EventHorizon .  
Similarly, an output port
consists of a 
.c (in-domain)toUniversal 
and a
.c DDFfromUniversal\ EventHorizon .
The DDF wormhole
looks exactly like a
.c DDF\ Star
from the \*(DO domain.  But, we require that it be a
.c DDF\ Star
without dynamic input portholes.
Therefore, it is fired when all input ports
have enough particles. When it is fired, it receives the data
from the outside, initiates the scheduler
of the inner domain, and sends the output data packets to the
outer domain.
The \fIgetStopTime()\fR method of this class will return the sum of the
\fIcurrentTime\fR and the \fIschedulePeriod\fR properties of the 
DDF scheduler.
.H2 "DDFtoUniversal EventHorizon
.pp
.IE "DDFtoUniversal"
DDFtoUniveral EventHorizon
transfers the incoming data packets from the DDF domain
to the paired 
.c FromEventHorizon
of the other domain.  
Since the \*(DO domain is an untimed domain, it sets the
\fIcurrentTime\fR of the DDF domain equal to that of the other
domain. 
.H2 "DDFfromUniversal EventHorizon
.pp
The
.c DDFfromUniversal\ EventHorizon
.IE "DDFfromUniversal"
receives the data from the associated 
.c ToEventHorizon 
of the other domain and transfers them to the \*(DO domain.
Since the \*(DO domain is untimed, the
.c DDFfromUniversal
ignores the timing information from the other domain. 
Therefore, it is not necessary to set the time stamps for this
.c EventHorizon .
