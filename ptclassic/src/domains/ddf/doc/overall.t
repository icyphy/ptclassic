.\" $Id$
.VR 0.$Revision$ "October 15, 1990"
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
It is a superset of the untimed\** version of the
synchronous dataflow (SDF) domain.
.(f
\**See the section "\fBSDF Domain\fR" in the Almagest for
\fItimed\fR and \fIuntimed\fR versions of the SDF domain.
.)f
In the SDF domain, a
.c Star
consumes and produces a fixed number of
.c Particle s
per each execution.  The deterministic information on the
number of
.c Particle s
for each 
.c Star
makes possible a compile-time scheduling.  In the \*(DO domain, 
however, a
.c Star
may consume or produce varying number of
.c Particle s
for each execution.  Therefore, the efficient compile-time
scheduling is no longer applicable.  Instead,
.c DDF\ Scheduler
detects which 
.c Star s
are runnable at runtime and fire them one by one until no more
.c Star
is runnable (the system is deadlocked).
.pp
Since the \*(DO domain is a superset of the SDF domain,
all
.c SDF\ Star s
are perfectly reasonable
.c Star s
in the \*(DO domain. Besides the
.c SDF\ Star s,
the \*(DO domain has some \*(DO-specific 
.c Star s
which will be discussed in this document.
The \*(DO-specific
.c Star s
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
as a
.c DDF\ Wormhole
which contains a SDF domain.
.sp
.H1 "The \\*(DO Scheduler
.pp
The scheduler in \*(PT determines the order of execution of
blocks, and also expects a particular behavior on the part of
.c Star s
within the \*(DO domain.
In this section, we describe the operation of the \*(DO scheduler.
.IE "DDF Scheduler"
.pp
The basic operation of the
.c DDF\ Scheduler
is to repeatedly scan the list of
.c Star s
in the domain and execute the runnable
.c Star s
until no more
.c Star s
are runnable.
There are three groups of runnable conditions for
.c Star s
in the \*(DO domain.
The first group of the
.c Star s
require the fixed number of
.c Particles
on all input arcs.  Most
.c SDF\ Star s
are included in this group.  The 
.c Scheduler
checks all input
.c PortHole s
for this group of
.c Star s.
.pp
The second group of 
.c Star s
are the \*(DO-specific
.c Star s
that requires non-deterministic number of
.c Particle s
on the input ports.  An example is the 
.c Select\ Star .
The 
.c Select\ Star
has three input
.c PortHole s
and one output
.c PortHole .
One input port (control port) receives the control value. 
Depending the control value, TRUE or FALSE,
the
.c Star
routes either \fItrue\fR or \fIfalse\fR input port
to the output port.  Therefore, the number of 
.c Particle s
required for the non-control input ports 
(\fItrue\fR or \fRfalse\fR input port)
is dependent on the control value.
The
.c Star s
in the second group has members called \fIwaitPort\fR and \fIwaitNum\fR.
.IE "waitPort, ddf"
.IE "waitNum, ddf"
Then, the scheduler checks whether the input arc
pointed by the \fIwaitPort\fR has as many
.c Particle s
as \fIwaitNum\fR to decide the runnability of the second group
.c Star .
In the
.c Select\ Star ,
the \fIwaitPort\fR points to the \fIcontrol\fR input port at the beginning.
If the \fIcontrol\fR input has enough data, the 
.c Star
is fired. When it is fired, it checks the value of the 
.c Particle
in the \fIcontrol\fR port, 
and change the \fIwaitPort\fR pointer to the input
.c PortHole
which is supposed to be routed to the output port. 
Then, the
.c Star
is fired again if it has enough data on the input port pointed
by the \fIwaitPort\fR.  This time, it actually routes the input
port to the output port.  Another example will be shown in the section 4.
.pp
The third group of stars comprises the source
.c Star s,
which are all 
.c SDF\ Star s.
Since the source
.c Star s
have no input ports, they are always runnable, which makes the system
never deadlocked.  Therefore, we should limit the number of
firing of those
.c Star s
in some way.
Recall again that the \*(DO domain is a superset of the untimed
version of the SDF domain.  It implies that with a given
system topology which contains only
.c SDF\ Star s,
we may run the
.c SDF\ Scheduler
or the
.c DDF\ Scheduler
to bear the exactly same results.
In the SDF domain, the number of firing of each source 
.c Star
is determined after a fairly complicated analysis of the system
topology at the compile time.  To make matters worse in the \*(DO
domain, the non-deterministic behavior of the \*(DO-specific
.c Star s
excludes the possibility of using similar complicated analysis
to determine the number of firing of the source 
.c Star s.
The technique we use is the \fIlazy-evaluation\fR technique.
.IE "lazy evaluation"
.pp
In the \fIlaze-evaluation\fR technique, we fire a
.c Star
when the predecessor requires any data from it.
At the beginning of each run of the
.c DDF\ Scheduler , 
we fire all source stars only once, and declare them as not runnable
afterwards.  After that, the scheduler starts scanning the list of
.c Star s
in the domain.  If a
.c Star
has some
.c Particle s
on some input arcs, but not runnable yet, then the 
.c Star
initiates the laze-evaluation of its ancestors which are connected
to the input
.c PortHole s
expecting more data.  
The lazy-evaluation technique magically generates the same
scheduling result as the
.c SDF\ Scheduler
for any
.c Galaxy 
of
.c SDF\ Star s.
Moreover, it clearly defines a "run" (iteration) of the
.IE "iteration, ddf"
.c DDF\ Scheduler .
Suppose a
.c Wormhole
contains the \*(DO domain.  And, the outer domain
sends the input data into the 
.c Wormhole
and waits for the output data from it.  With given inputs,
the 
.c DDF\ Scheduler
would run once on the \*(DO domain.  Then, a single run
of the
.c DDF\ Scheduler
should generate the output data requested from the outer domain. 
A "run" of the
.c DDF\ Scheduler
means that it fires all source stars once beforehand and repeats scanning
the list of
.c Star s
with lazy-evaluation technique until no more is runnable.
.sp
.H1 "Special \\*(DO Particles
.pp
.c Particle s
are the envelope that encapsulate the data or messages passing
between blocks in \*(PT.
All
.c Particle s
in \*(PT are derived from a basic
.c class\ Particle .
All
.c Particle s
in \*(PT are available for use within any particular domain.
However, each domain typically has some special
.c Particle s
that are particular to that domain.
.pp
Currently, no \fBDDF\fR-specific
.c Particle s
are defined.
.sp
.H1 "Programming Stars in the \\*(DO Domain
.pp
There is an attempt to maintain uniformity across
different domains in \*(PT.
However, in each domain there are typically special needs
for the writers of
.c Star s
in that domain.
This section describes how \*(DO-specific
.c Star s
are programmed in the \*(DO domain.
For
.c SDF\ Star s,
refer to the document, "\fBSDF Domain in Ptolemy\fR".
.pp
A
.c DDF\ Star
.IE "DDF Star"
has at least one 
.c PortHole ,
either input or output, that receives or sends a non-deterministic
number of
.c Particle s.
Such 
.c PortHole s
are called \fIdynamic\fR.
.IE "dynamic PortHole"
For the
.c DDF\ Star ,
how many
.c Particle s
to read or write is determined at runtime, in the \fIgo()\fR method.
Let's look at an example :
.c LastOfN\ Star .
.(c
ident {
/**************************************************************************

 This star outputs the last input data from the N tokens.
 The value N is read from the control input.

**************************************************************************/
}
defstar {
	name {LastOfN}
	domain {DDF}
	desc {
	   "Switches input events to one of two outputs, depending on"
	   "the last received control input."
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
.c LastOfN\ Star
discards the first \fIN-1\fR 
.c Particle s
from the \fIinput\fR
.c PortHole
and routes the last one to the \fIoutput\fR
.c PortHole .
The value \fIN\fR is read from the \fIcontrol\fR input.
Since the control data is varying, the number of
.c Particle s
to read from the \fIinput\fR
.c PortHole
is non-deterministic, which makes this star a
.c DDF\ Star .
We can specify that the \fIinput\fR
.c PortHole
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
This 
.c Star
has also a 
.c private
member called \fIreadyToGo\fR. The \fIreadyToGo\fR flag
is set TRUE when the \fIwaitPort\fR points to the \fIinput\fR
.c PortHole .
Initially, the
.c Star
waits for the control data, and set the flag FALSE
(\fIstart()\fR method).  When it is fired, it first checks the flag.
If the flag is set FALSE, meaning that it waits for the control data,
it reads the control data from the \fIcontrol\fR input and decides
how many
.c Particle s
it needs from the \fIinput\fR
.c PortHole . 
Then, it changes the \fIwaitPort\fR pointer to the \fIinput\fR
.c PortHole
with appropriate value for the \fIwaitNum\fR, and set the
\fIreadyToGo\fR flag TRUE.  
If the flag is set TRUE when fired, it means that
the \fIinput\fR
.c PortHole 
has enough
.c Particle s.
Therefore, it discards the first \fIN-1\fR inputs and copies the
last input to the output port.  Then, it goes into the next round :
sets the \fIwaitPort\fR pointer to the \fIcontrol\fR
.c PortHole
and set the \fIreadyToGo\fR flag FALSE.
.pp
The next example is a 
.c DDF\ Star
with a dynamic output
.c PortHole:
.c DownCounter\ Star .
.(c
ident {
/**************************************************************************

 This star outputs the last input data from the N tokens.
 The value N is read from the control input.

**************************************************************************/
}
defstar {
	name {DownCounter}
	domain {DDF}
	desc {
	   "A down-counter from input value to zero"
	}
	input {
		name {input}
		type {int}
	}
	output {
		name {output}
		type {int}
		num  {0}
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
.c DownCounter\ Star
has a dynamic \fIoutput\fR
.c PortHole
which will generate the down-counter sequence of integer data
starting from the value read through the \fIinput\fR
.c PortHole .
The code in the \fIgo()\fR method is self-explanatory.
.H2 "DDFSelf Star For Recursion
.pp
There is a special
.c Star
in the \*(DO domain to realize the recursion construct :
.IE "recursion"
.c DDFSelf\ Star .
.IE "DDFSelf Star"
At compile time, it is just an atomic
.c Star .
But, if it is fired at runtime, it creates a
.c Galaxy
dynamically, executes and destroy the
.c Galaxy .
The 
.c Galaxy
is a clone of the 
.c Galaxy
(recursion-galaxy)
which represents the recursion construct.
The
.c DDFSelf\ Star
has a
.c private
member, \fIgal\fR, that points to the recursion-galaxy. 
The \fIgal\fR pointer is set at the compile time by matching
the name of the
.c Galaxy
with the 
.c StringState\ recurGal
which is given by the programmer.
.pp
At runtime, the
.c DDFSelf\ Star
creates a clone galaxy of the recursion-galaxy pointed by the \fIgal\fR
pointer.  It also creates the
.c DDF\ Scheduler
dedicated to the created 
.c Galaxy .
The 
.c PortHole s
of the
.c DDFSelf\ Star
are connected not only to the other
.c Star s
but also to the 
.c Galaxy
just created.
After connections are set up, it received the input data from
the outside and pass them in to the 
.c Galaxy .
It runs the
.c DDF\ Scheduler
to the
.c Galaxy
and gather-and-sends the output data from the
.c Galaxy 
to the outside.
At last, it destroys the
.c Galaxy
and the
.c DDF\ Scheduler
which it created.
The runtime efficiency is quite low since a lot of memory
management is involved in the creation and deletion of
the
.c Galaxy
and the
.c DDF\ Scheduler .
A further research is in progress to increase the performance.
.H1 "EventHorizon in the \\*(DO Domain
.pp
The mixture of the \*(DO domain with other domains
requires a conversion between different computational
models.
In particular, some domains associate a \fItime stamp\fR
with each
.c Particle ,
and other domains do not.
Thus, a common function at the
.c EventHorizon
is the addition of time stamps, the stripping off of
time stamps, interpolation between time stamps,
or removal of redundant repetitions of identical
.c Particle s,
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
An input port of a
.c DDF Wormhole
consists of a 
.c DDFtoUniversal\ EventHorizon
and a
.c (in-domain)fromUniversal\ EventHorizon.  Similarly, an output port
consists of a 
.c (in-domain)toUniversal 
and a
.c DDFfromUniversal\ EventHorizon .
The
.c DDF\ Wormhole
looks exactly like a
.c DDF\ Star
from the \*(DO domain.  But, we restrict that it is a
.c DDF\ Star
without dynamic input
.c PortHole s.
Therefore, it is fired when all input ports
have enough data packets. When it is fired, it receives the data
from the outside, initiates the
.c Scheduler
of the inner domain, and sends the output data packets to the
outer domain.
.H2 "DDFtoUniversal EventHorizon
.pp
.IE "DDFtoUniversal"
It transfers the incoming data packets from the DDF domain
to the paired 
.c FromEventHorizon
of the other domain.  
Since the \*(DO domain is an untimed domain, it sets the
\fIcurrentTime\fR of the DDF domain equal to that of the other
domain.  The \fIgetNextStamp()\fR method of this class
returns the sum of the \fIcurrentTime\fR and the \fIschedulePeriod\fR
of the \*(DO domain.
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
totally ignore the timing information from the other domain. 
If it is at an input port of a 
.c Wormhole 
and it has enough data packets,
it sets the stopping condition for the inner \*(DO domain (\fIready()\fR 
method).
The stopping condition is a single "run" (iteration) of the
.c DDF\ Scheduler .
