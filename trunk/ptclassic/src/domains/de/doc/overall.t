.\" $Id$
.VR 0.$Revision$ "February 4, 1991"
.TI "DE Domain"
.AU
Soonhoi Ha
.AE
.ds DO DE
.H1 "Introduction"
.pp
The discrete event (DE) domain in \*(PT provides a very general
.IE "discrete event domain"
.IE "DE domain"
environment for the time-related simulation of the digital systems,
such as queueing network simulation.  In the \*(DO domain, each
.c Particle
represent an \fIevent\fR which corresponds to the change of system
.IE "event"
state.  The
.c DE\ Scheduler
processes the events in the chronological order.  Since the time interval
of the events is generally not fixed, each particle 
is associated with a \fItime-stamp\fR.
Time stamps are generated by the block
producing the particle ,
using the time stamps of the input particles
and the latency in the block.
.H1 "The \\*(DO Scheduler
.pp
.IE "DE Scheduler"
The scheduler in \*(PT determines the order of execution of
blocks, and also expects a particular behavior on the part of
.c Star s
within the \*(DO domain.
In this section, we describe the operation of the \*(DO scheduler.
.pp
A
.c DE\ Star 
basically models the system response on the change of the system state.
The change of system state,
which is called an \fIevent\fR,
will be the content of a particle 
in the \*(DO domain.  Each particle
is assigned a time stamp telling when it is generated. 
.IE "time stamp"
Since the events are irregularly spaced in time
and the system responses can be very dynamic in general,
all scheduling actions are done at runtime.  
At runtime, the DE scheduler
processes the events in the chronological order until the
global time reaches the "stop time".
.pp
The scheduler
maintains a \fIglobal event queue\fR where
.IE "event queue"
particles
currently in the system are sorted in accordance with time stamps.
Some DE stars
are event-generators which do not consume any events. 
They need to be triggered by the system-generated triggering particles
which are to be placed in the event-queue before the system is executed.
The 
.c Scheduler
fetches an event from the head of the queue, which is the
earlist event, and sends it to its destination block. 
Before executing
the block, it searches the event queue to find out whether there are
any simultaneous events to the same block (user may or may not allow
simultaneous events when defining stars).
After a block is executed it may generate some output events on its
output 
.c PortHole s.
These events are also put into the global event queue.
Then the scheduler fetches another events and repeats its action until
a given stopping condition is met.
.pp
There may have many simultaneous events in the event queue.
Which event to be fetched first?
One possible strategy is to choose one event arbitrarily.
This scheme is the simplest in the implementation.  We can stack up
the simultaneous events in the queue in the order of arrivals.  Since
the events are randomly generated, their arrivals to the event queue
are also random.  This scheme, however, has a pitfall.
Suppose block A produces an output event to block B and block C
respectively.  And block B processes the incoming event and
generates an output event to block C.  If block B takes
zero time (or insignificant time), block C should process two input
events (one from block B, and one from block A) at the same time
to generate the right result.  What happens if we stack the output
events from block A into the event queue in the order in which
block C is executed before block B.  Before block B is executed,
block C is not aware of any possibility of simultaneous input events.
To overcome this difficulty, the usual trick is to put an
infinitesimal delay in the arc from block A to block C.
This delay is enough to schedule block B before block C, thus
enables block C to detect the simultaneous events.  This situation will
be frequent since many stars
in the DE domain are delay-free.  See the following sections for the reason.
This example is a simple one.  If we have many blocks involved in
this kind of delay-free operations, the assignment of
infinitesimal delay is no longer easy but error-prone.
.pp
Our technique is to assign a level (called \fIdepth\fR) to the
.IE "depth, DEStar"
blocks in the DE domain.  The \fIdepth\fR of a block is
defined as the minus of the longest path from the block 
to any "terminating block".
There are three kinds of the terminating blocks.  The first one
is a star
without output ports.  The second one is the stars
on the 
.c Wormhole
boundary where the output ports of the star
corresponds to the output ports of the wormhole.
The third kind is the \fIdelay-star\fRs which will be discussed
in the next section.  By defining the \fIdepth\fR of a terminating star $-1$,
the \fIdepth\fR of any non-terminating star is less than $-1$.
If a star
has a smallest \fIdepth\fR, it means that the star is to be scheduled first
among the destination stars 
of the simultaneous events.
Therefore, when we put an event into the event queue, we first check the
time stamp of the event, second compare the \fIdepth\fRs of the
destination blocks among simultaneous events.
During the process of depth assignment, we can detect any delay-free
loop in the DE domain.
.IE "delay-free loop"
While the delay-free loop in the SDF domain results in a deadlock
state of the system, the delay-free loop in the DE domain may be
intentional.  The delay-free loop in the DE domain is detected when
the \fIdepth\fRs of two stars
depends on each other undecided during the recursive calculation
of the depths.
In other words, if there is a loop of stars
with no terminating star,
the loop is called delay-free.  The difficulty comes from the
fact that the definition of the \fIdelay-star\fR is somewhat
ambiguous.  We define a DE star
as a \fIdelay-star\fR if it always takes a non-zero time
to produce output events from any input event.
But, there are some DE stars
which generate some outputs without delay
with some inputs but some outputs with a non-zero delay with some inputs.
A good example is a 
.c Queue\ Star .
If the \fIdemand\fR input event arrives, the star
generates an output immediately in case there are events stored in the 
queue.  When, a normal input event arrives, however, it stores
the event into the queue if the associated server is not free.
By our definition, the 
.c Queue\ Star
is not a delay-star.  But it is possible to make a delay-free
loop with this
.c Queue\ Star
without satisfying a real deadlock (or infinite loop of events) condition.
Therefore, we just generates a warning message if we detect a delay-free
loop to remind the user to check up the connections.
.pp
\fBTime\fR in the \*(DO
domain means simulated time.  
The time unit may need to be scaled if the DE domain inside a wormhole
is to be synchronized with the outer timed domain.
The procedure of scaling time units is illustrated in 
the "\fBDomains in Ptolemy\fR" document.
.pp
The DE domain is a timed domain.  
If it contains another
timed domain (such as THOR) in a
.c DE\ Wormhole ,
the wormhole can become a "process-star"\** if the
.(f
\** Read document "\fBDomains in Ptolemy\fR" before read this paragraph.
.)f
inner domain is not deadlocked\** when stopping condition is met.
.(f
\** The domain is said deadlocked if there is no more runnable blocks
in the domain.
.)f
In this case, we have to fire the process-star without incoming events
to the block until it is deadlocked.  Therefore, we put the
process-star into the event queue with the time stamp telling when
it should be fired again.  We can differentiate the process-stars
from the events in the queue by marking them.
A process star is inserted into the event queue after
execution as long as it is not deadlocked.
On the other hand, the DE domain may reside inside a wormhole
of another timed domain.  In this case, the scheduler
must check whether it is deadlocked at the end of the execution (meeting
the stopping condition).
The flag, called \fIstopBeforeDeadlocked\fR, 
is set when the DE domain finishes the current execution not
deadlocked.  If the outer domain is a timed domain and the
\fIstopBeforeDeadlocked\fR flag of the inner DE domain is set, then
the wormhole
will be regarded as a process-star in the outer domain.
.pp
If a DE wormhole
contains a timed domain inside, the stop time of the timed domain
is set equal to the current global time of the DE domain, which makes
the wormhole
as a functional block.  This scheme is inevitable for hazardless domain
interface as described in the document "\fBDomains in Ptolemy\fR".
If this scheme is applied, we say that the DE domain operates in the
\fIsynchronized mode\fR.
.IE "synchronized mode, de"
The synchronized mode operation suffers a lot of overhead at 
runtime since the wormhole
should be called at
every time increment in the inner domain.  If we can be sure that
the inner domain can be processed until it finishes the current execution,
stopping the inner domain at each time increment is too conservative.
This situation will be not rare ; for example, if a wormhole
has only one input port, we may execute the wormhole
until it is deadlocked without worrying about the next input event.
Therefore, to improve the runtime efficiency, we take an option to 
ignore the
synchronized mode operation.  Instead, we process the wormhole
until the inner domain is deadlocked or the inner time reaches the global
stoptime.  Then, we say the DE domain operates in the
\fIoptimized mode\fR.
.IE "optimized mode, de"
We may choose the mode by setting a 
.c int\ State ,
called \fIsyncMode\fR in the outermost 
.c Galaxy
of the DE domain.  If the \fIsyncMode\fR is set to zero, we enter into the
optimized mode.  The default value of the \fIsyncMode\fR is one (means
synchronized mode operation).
.H1 "Special \\*(DO Particles
.pp
.c Particle s
are the envelope that encapsulate the data or messages passing
between blocks in \*(PT.
All particles
in \*(PT are derived from a basic
.c class\ Particle .
All particles
defined in the kernel of \*(PT are available 
for use within any particular domain.
However, each domain typically has some special particles
that are particular to that domain.
.pp
Currently, no \fBDE\fR-specific particles
are defined.
.H1 "Programming Stars in the \\*(DO Domain
.pp
There is an attempt to maintain uniformity across
different domains in \*(PT.
However, in each domain there are typically special needs
for the writers of stars
in that domain.
This section describes how stars
are programmed in the \*(DO domain.
.pp
A 
.c DE\ Star
.IE "DE Star"
can be viewed as an event-processing unit, which receives
events from the outside, processes them, and generates output events after
some latency. In the \*(DO domain, the management of the
time stamps of the particles
is as important as the functionality of a star.
For time-management, the
.c DEStar\ class
has two DE-specific members : 
\fIarrivalTime\fR and \fIcompletionTime\fR.
The \fIarrivalTime\fR
indicates when the current event arrives while
the \fIcompletionTime\fR 
tells when the last execution of the star
is completed.  
Since the functionality and the time management are
two orthogonal tasks of the DE star ,
we dedicate some DE stars, 
so-called \fIdelay-star\fRs,
.IE "delay-star, de"
to the task of time-management (\fIDelay, Server\fR).
The other stars,
so-called \fIfunctional-star\fRs,
.IE "functional-star, de"
are stripped of the time-management task, generating the output
events at the same time when it receives input events.
The DE star
has a flag, \fIdelayType\fR, to be set if it is categorized
into the delay star, or reset otherwise.
The delay-stars and the functional-stars compose two orthogonal
sets of the DE stars.
By combining a delay-star and a functional-star, we can
realize a general DE star 
(combining two stars means concatenating them).
.H2 "Delay Stars
.pp
\fIDelay-star\fRs are in charge of time-management task
in the \*(DO domain.  The more strict definition is already
given in the previous section.
We can classify the delay-stars
further into two types.  One type is a \fIpure-delay\fR.
.IE "pure-delay type, de"
A delay-star of \fIpure-delay\fR type is that the output
event is generated a user-given latency after the input
event comes.  The following example will help the understanding.
It is the preprocessor format of the
.c Delay\ Star 
in the \*(DO domain.  For more details on the preprocessor,
look at the document, "\fBThe Ptolemy Preprocessor Language\fR".
.(c
defstar {
	name {Delay}
	domain {DE}
	version { @(#)DEDelay.pl	1.3	1/28/91}
	author { E. A. Lee }
	copyright { 1991 The Regents of the University of California }
	location { DE main library }
	desc {
Delays its input by an amount given by the delay parameter.
	}
	input {
		name {input}
		type {anytype}
	}
	output {
		name {output}
		type {anytype}
	}
	defstate {
		name {delay}
		type {float}
		default {"1.0"}
		desc { Amount of time delay. }
	}
	constructor {
		input.inheritTypeFrom(output);
		delayType = TRUE;
	}
	go {
	   completionTime = arrivalTime + double(delay);
	   Particle& pp = input.get();
           output.put(completionTime) = pp;
	}
}
.)c
Inside the \fIgo()\fR method description, the \fIcompletionTime\fR
is determined by the arrival time of the current event plus
a \fIdelay\fR.
The last two lines will be explained in subsection 4.2.
.pp
The other type is a \fIserver\fR.  
.IE "server type, de"
In a delay-star of \fIserver\fR type, the input event should wait
until the previous execution is completed.  It emulates a server that
can process only one event at a time.  Here is the example of a
\fIserver\fR type : 
.c Server\ Star .
.(c
ident {
/**************************************************************************

 This star emulates a server.  If input events arrive when it is not busy,
 it delays them by the service time.  If they arrive when it is busy,
 it delays them by more.  It must become free, and then serve them.

**************************************************************************/
}
defstar {
	name {Server}
	domain {DE}
	desc {
	   "This star emulates a server.\n"
	}
	input {
		name {input}
		type {anytype}
	}
	output {
		name {output}
		type {anytype}
	}
	defstate {
		name {serviceTime}
		type {float}
		default {"1.0"}
		desc { "Service time" }
	}
	constructor {
		input.inheritTypeFrom(output);
		delayType = TRUE;
	}
	go {
	   // No overlapped execution. set the time.
	   if (arrivalTime > completionTime)
		completionTime = arrivalTime + double(serviceTime);
	   else
		completionTime += double(serviceTime);

	   Particle& pp = input.get();
           output.put(completionTime) = pp;
	}
}
.)c
If the \fIarrivalTime\fR is later than the \fIcompletionTime\fR, or
an input event arrives when the server is free, the server processes
the input event immediately and takes some \fIserviveTime\fR.
Otherwise, or an input event arrives when the server is busy, it
is delayed until the server is available and processed.
.H3 "Event Generators
.pp
A
.c DE\ Star
is fired only when an event is fed into the star. 
In the \*(DO domain, we need another class of DE star,
called \fIself-scheduling star\fR.
A self-scheduling star generates an event to itself.
The feedback event triggers the star itself, which emulates
an event generator.  An event generator is a special case of a delay star
.IE "event generator, de"
in that its role is mainly to control the time spacing of source events.
The values of the source events can be determined by a functional block
attached to the end of the event generator (e.g. \fIFloatDC, FloatRamp\fR,
etc).  
.pp
A self-scheduling star is derived from 
.c class\ DERepeatStar
which is again a derived class of
.IE "DERepeatStar"
.c class\ DEStar .
The
.c DERepeatStar\ class
has some special methods to facilitate the self-scheduling function :
\fIrefireAtTime()\fR and \fIcanGetFired()\fR.  Let's look at an example :
.c Poisson\ Star.
.(c
ident {
/**************************************************************************

 This star generates events according to a Poisson process.
 The mean inter-arrival time and magnitude of the events are
 given as parameters.

**************************************************************************/
}
defstar {
	name {Poisson}
	domain {DE}
	derivedfrom { RepeatStar }
	desc {
	   "Generates events according to a Poisson process.\n"
	   "The first event comes out at time zero."
	}
	output {
		name {output}
		type {float}
	}
	defstate {
		name {meanTime}
		type {float}
		default {"1.0"}
		desc { "The mean inter-arrival time" }
	}
	defstate {
		name {magnitude}
		type {float}
		default {"1.0"}
		desc { "The magnitude of samples generated" }
	}
	hinclude { <NegativeExpntl.h> }
	ccinclude { <ACG.h> }
	protected {
		NegativeExpntl *random;
	}
// declare the static random-number generator in the .cc file
	code {
		extern ACG gen;
	}
	constructor {
		random = new NegativeExpntl(double(meanTime),&gen);
	}
	destructor {
		delete random;
	}
	start {
		DERepeatStar :: start();
		random->mean(double(meanTime));
	}
	go {
	   // Generate the output event
	   // (Recall that the first event comes out at time 0).
	   // (The double cast is because of a gnu compiler bug)
	   output.put(completionTime) << float(double(magnitude));

	   // and schedule the next firing
	   refireAtTime(completionTime);

	   // Generate an exponential random variable.
	   double p = (*random)();

	   // add to completionTime
	   completionTime += p;
	}
}
.)c
The
.c Poisson\ Star
emulates a Poisson arrival process.  The inter-arrival time of events
is exponentially distributed by parameter \fImeanTime\fR.
The method,
\fIrefireAtTime()\fR launches a event on the feedback arc which is
invisible to the users.  The feedback event triggers the 
self-scheduling star.
.pp
To initiate the event generation, an event is placed on the feedback
arc before the scheduler
runs.  In other words, at time 0, some events are placed in the
global queue to trigger the self-scheduling stars.  Also it is
noteworthy that the feedback event for the next execution is
generated in the current execution.
.pp
Another method, \fIcanGetFired()\fR is seldom used in the star
definitions.  The method
checks the feedback event, and return TRUE if there is
a new event or FALSE otherwise.
.H2 "Functional Stars
.pp
In the \*(DO domain, a star
is \fIrunnable\fR (ready for execution), if any input portHole
has a new event (Particle).  
And, the star
has the responsibility to detect which input portHole
a new event arrives at.  The input portHole
containing a new particle
has the \fIdataNew\fR flag set by the 
.IE "dataNew, flag"
scheduler
when it sends the event to the star.
Then, the star
can check up the \fIdataNew\fR flag to detect the event path.
And, it processes the input event and generates an output event
without any latency.  The latency of a \fIfunctional-star\fR can 
be modeled by concatenating a \fIdelay-star\fR which is explained
in the previous subsection.
Now, let's look at how to write the body of a functional star 
with an example : 
.c Switch\ Star .
.(c
ident {
/**************************************************************************

 This star switches input events to one of two outputs, depending on
 the last received control input.

**************************************************************************/
}
defstar {
	name {Switch}
	domain {DE}
	desc {
	   "Switches input events to one of two outputs, depending on"
	   "the last received control input."
	}
	input {
		name {input}
		type {anytype}
	}
	input {
		name {control}
		type {int}
	}
	output {
		name {true}
		type {anytype}
	}
	output {
		name {false}
		type {anytype}
	}
	constructor {
		true.inheritTypeFrom(false);
		input.inheritTypeFrom(true);
	}

	go {
	   if (input.dataNew) {
	   	completionTime = arrivalTime;
		Particle& pp = input.get();
		int c = int(control%0);
		if(c)
           	   true.put(completionTime) = pp;
		else
           	   false.put(completionTime) = pp;
	   }
	}
}
.)c
The
.c Switch\ Star
has two input portHoles
: \fIinput\fR, and \fIcontrol\fR.
When an event arrives at the \fIinput\fR portHole, 
it routes the event to either \fItrue\fR or \fIfalse\fR portHole
depending the value of the last received \fIcontrol\fR input.
First of all, we have to check
whether a new input event arrives. 
It is done by looking at the \fIdataNew\fR
flag of the \fIinput\fR portHole.
.pp
There are three methods to access a particle
from  input or output ports. First, we may use \fI%\fR operator followed
by a integer, which
is common to \fBSDF\fR domain.  
.c control%0
returns the recentest particle
from the \fIcontrol\fR portHole,
and
.c control%2
would return the second previous particle.
The second method, \fIget()\fR, is specific to 
.IE "get(), de"
.c InDEPort .
It resets the \fIdataNew\fR member of the port as well as returns the
recentest particle
from an input port. If you need to reset the \fIdataNew\fR member of a
input port after read the newly arrived event, which is a usual case,
you have to use \fIget()\fR method instead of \fI%0\fR operator.
This is why we use \fIget()\fR method for the \fIinput\fR portHole.
The last method, \fIput(float)\fR, is specific to 
.IE "put(), de"
.c OutDEPort .
It sets the \fItimeStamp\fR member of the port as well as returns the
recentest particle
from an output port. Let's look at a line in the above example.
.(c
true.put(completionTime) = pp;
.)c
It says that we copy the particle \fBpp\fR
to the output port with \fItimeStamp = completionTime\fR.
.sp
.pp
This rather long section describes the method how to design a
DE star.
The good starting point to design a star
would be to look at the existing stars.		
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
are also fully discussed in the document.
Refer to the document for more detailed explanations on the domain interface.
In this section, \*(DO-specific features on the domain
interface will be discussed.
.H2 "DE Wormhole
.pp
.IE "DE Wormhole"
The \*(DO domain may have a wormhole which contains another domain.
An input port of a
.c DE\ Wormhole
consists of a 
.c DEtoUniversal\ EventHorizon
and a
.c (in-domain)fromUniversal\ EventHorizon .  
Similarly, an output port consists of a 
.c (in-domain)toUniversal 
and a
.c DEfromUniversal\ EventHorizon .
Since the DE wormhole
looks exactly like a DE star
from the \*(DO domain, it is fired when any input port
has an event. When it is fired, it initiates the scheduler
of the inner domain.
It is always regarded as a functional star. 
The DE wormhole
redefined the \fIsumUp()\fR method to detect whether the wormhole 
is a process-star or not.
If the DE domain is the outer domain and the inner domain is also
timed, this method
examined the \fIstopBeforeDeadlocked\fR flag of the inner scheduler.
If the flag is set, the wormhole
is regarded as a process star in the DE domain.
.H2 "DEtoUniversal EventHorizon
.pp
.IE "DEtoUniversal
It transfers the incoming data packets from the DE domain to the paired 
.c FromEventHorizon
of the other domain.  It also passes the timing information
of the DE domain to the other domain by setting the time stamps.
Refer to the "\fBDomains in Ptolemy\fR" for the standard
functions of the
.c ToEventHorizon\ class .
.H2 "DEfromUniversal EventHorizon
.pp
.IE "DEfromUniversal
The
.c DEfromUniversal\ EventHorizon
receives the data packets from the associated 
.c ToEventHorizon 
of the other domain and transfers them to the DE domain.
The time stamp is copied from that of its counterpart after scaled.
According to the time stamp, the received events are put into
the global queue of the \*(DO domain.
