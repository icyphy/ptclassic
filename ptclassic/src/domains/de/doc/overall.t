.\" $Id$
.VR 0.$Revision$ $Date$
.TI "DE Domain"
.AU
Soonhoi Ha
.AE
.ds DO DE
.H1 "Introduction"
.pp
The discrete event (DE) domain in \*(PT provides a general
.IE "discrete event domain"
.IE "DE domain"
environment for time-related simulation of systems
such as queueing networks, communication networks,
and high-level computer architectures.
In the \*(DO domain, each
.c Particle
represents an \fIevent\fR that corresponds to the change of system
.IE "event"
state.  The
.c DE\ Scheduler
processes the events in the chronological order.  Since the time interval
of the events is generally not fixed, each particle 
has an associated \fItime-stamp\fR.
.IE "time stamp"
Time stamps are generated by the block
producing the particle,
using the time stamps of the input particles
and the latency in the block.
.H1 "The \\*(DO Scheduler (part I)
.pp
.IE "DE Scheduler"
The scheduler in \*(PT determines the order of execution of
blocks.  It expects particular behavior (operational semantics)
on the part of
.c Star s
within the domain.
In this section, we describe the operation of the \*(DO scheduler.
.H2 "Events and chronology"
.pp
A
.c DE\ Star 
models part of a system response to a change in the system state.
The change of state,
which is called an \fIevent\fR,
is signaled by a particle 
in the \*(DO domain.  Each particle
is assigned a time stamp telling when it is generated, in simulated time.
.IE "simulated time"
Since events are irregularly spaced in time
and the system responses can be very dynamic in general,
all scheduling actions are done at runtime.  
At runtime, the DE scheduler
processes the events in chronological order until the
global time reaches a global "stop time".
.IE "stop time"
.pp
The scheduler
maintains a \fIglobal event queue\fR where
.IE "event queue"
particles
currently in the system are sorted in accordance with time stamps.
Some DE stars
are event-generators that do not consume any events. 
They are triggered by system-generated triggering particles
which are to be placed in the event-queue before the system is executed.
The 
.c Scheduler
fetches an event from the head of the queue, which is the
earlist event in simulated time, and sends it to its destination block. 
Before executing
the block, it searches the event queue to find out whether there are
any simultaneous events to the same block (a user may or may not allow
simultaneous events when defining stars).
After a block is executed it may generate some output events on its
output
.c PortHole s.
These events are also put into the global event queue.
Then the scheduler fetches another events and repeats its action until
a given stopping condition is met.
.H2 "Wormholes"
.pp
\fBTime\fR in the \*(DO
domain means simulated time.  
.IE time
.IE "simulated time"
The time unit may need to be scaled if the DE domain inside a wormhole
is to be synchronized with the outer timed domain.
The procedure of scaling time units is explained in 
the "\fBDomains in Ptolemy\fR" section of the Almagest.
.pp
The DE domain is a timed domain.
If it contains another
timed domain (such as THOR) in a
.c DE\ Wormhole ,
the wormhole can become a "process-star"\** if the
.(f
\** see the Almagest section "\fBDomains in Ptolemy\fR".
.)f
inner domain is not deadlocked\** when the stopping condition is met.
.(f
\** The domain is said deadlocked if there is no more runnable blocks
in the domain.
.)f
In this case, we have to fire the process-star without incoming events
to the block until it is deadlocked.  Therefore, we put the
process-star into the event queue with the time stamp telling when
it should be fired again.  We can differentiate process-stars
from the events in the queue by marking them.
A process star is inserted into the event queue after
execution as long as it is not deadlocked.
.pp
On the other hand, a DE domain subsystem may reside inside a wormhole
of another timed domain.  In this case, the scheduler
must check to see whether it is deadlocked at the end of the execution
(when it meets the stopping condition).
A flag, called \fIstopBeforeDeadlocked\fR, 
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
a zero delay functional block.  This is necessary
as described in the document "\fBDomains in Ptolemy\fR".
If this scheme is applied, we say that the DE domain operates in the
\fIsynchronized mode\fR.
.IE "synchronized mode, DE"
The synchronized mode operation suffers a lot of overhead at 
runtime since the wormhole
should be called at
every time increment in the inner domain.  If we can be sure that
the inner domain can be processed until it finishes the current execution,
stopping the inner domain at each time increment is too conservative.
.pp
This situation will not be rare; for example, if a wormhole
has only one input port, we may execute the wormhole
until it is deadlocked without worrying about the next input event.
Therefore, to improve the runtime efficiency, we make it an option to 
avoid synchronized mode operation.  Instead, we can process a wormhole
until the inner domain is deadlocked or the inner time reaches the global
stoptime.  Then, we say the DE domain operates in the
\fIoptimized mode\fR.
.IE "optimized mode, de"
We may choose the mode by setting a 
.c int\ State ,
called \fIsyncMode\fR in the outermost 
.c Galaxy
of the DE domain.  If the \fIsyncMode\fR is set to FALSE (zero),
we enter into the
optimized mode.  The default value of the \fIsyncMode\fR is TRUE (one)
(which means synchronized mode operation).
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
some latency.  In the \*(DO domain, the management of the
time stamps of the particles
is as important as the functionality of a star.
For time-management, the
.c DEStar
class has two DE-specific members: 
\fIarrivalTime\fR and \fIcompletionTime\fR.
The \fIarrivalTime\fR
indicates when the current event arrives while
the \fIcompletionTime\fR 
tells when the last execution of the star
was completed.  
Since the functionality and the time management are
two orthogonal tasks of the DE star,
we dedicate some DE stars,
so-called \fIdelay-star\fRs,
.IE "delay-star, DE"
to the task of time-management (\fIDelay, Server\fR).
The other stars,
so-called \fIfunctional-star\fRs,
.IE "functional-star, DE"
are stripped of the time-management task, generating the output
events at the same time when it receives input events.
The DE star
has a flag, \fIdelayType\fR, to be set (TRUE or one) if it is categorized
into the delay star, or reset (FALSE or zero) otherwise.
Delay-stars and the functional-stars compose two orthogonal
sets of the DE stars.
By combining a delay-star and a functional-star, we can
realize a general DE star 
(combining two stars means concatenating them).
.H2 "Delay Stars
.pp
\fIDelay-star\fRs are in charge of time-management task
in the \*(DO domain.  A strict definition is given above.
We can classify the delay-stars
further into two types.  One type is a \fIpure-delay\fR.
.IE "pure-delay type, DE"
A delay-star of \fIpure-delay\fR type is that the output
event is generated a user-given latency after the input
event arrives.  Here is an example.
It is the preprocessor format of the
.c Delay\ Star 
in the \*(DO domain.  For more details on the preprocessor,
see the Almagest section "\fBThe Ptolemy Preprocessor Language\fR".
.(d
defstar {
	name {Delay}
	domain {DE}
	version { @(#)DEDelay.pl	2.1	2/21/91}
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
		type {=input}
	}
	defstate {
		name {delay}
		type {float}
		default {"1.0"}
		desc { Amount of time delay. }
	}
	constructor {
		delayType = TRUE;
	}
	go {
	   completionTime = arrivalTime + double(delay);
	   Particle& pp = input.get();
           output.put(completionTime) = pp;
	}
}
.)d
Inside the \fIgo()\fR method description, the \fIcompletionTime\fR
is determined by the arrival time of the current event plus
a \fIdelay\fR.
The last two lines will be explained later in this section.
.pp
The other type of delay star is a \fIserver\fR.
.IE "server type, DE"
In a delay star of \fIserver\fR type, the input event should wait
until the previous execution is completed.  It emulates a server that
can process only one event at a time.  Here is the example of a
\fIserver\fR type : 
.c Server\ Star .
.(d
defstar {
	name {Server}
	domain {DE}
	desc {
This star emulates a server.  If input events arrive when it is not busy,
it delays them by the service time (a constant parameter).
If they arrive when it is busy, it delays them by more.
It must become free, and then serve them.
	}
	version { @(#)DEServer.pl	2.1	2/21/91}
	author { E. A. Lee }
	copyright { 1991 The Regents of the University of California }
	location { DE main library }
	input {
		name {input}
		type {anytype}
	}
	output {
		name {output}
		type {=input}
	}
	defstate {
		name {serviceTime}
		type {float}
		default {"1.0"}
		desc { Service time. }
	}
	constructor {
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
.)d
If the \fIarrivalTime\fR is later than the \fIcompletionTime\fR, or
an input event arrives when the server is free, the server processes
the input event immediately and takes some \fIserviceTime\fR.
Otherwise, when an input event arrives when the server is busy, the handling
of the event is delayed until the server is available.
.H2 "Event generators
.pp
A
.c DE\ Star
is fired only when an event is fed into the star. 
In the \*(DO domain, we need another class of DE star,
called a \fIself-scheduling star\fR.
A self-scheduling star generates an event to itself.
The feedback event triggers the star itself, which emulates
an event generator.  An event generator is a special case of a delay star
.IE "event generator, de"
in that its role is mainly to control the time spacing of source events.
The values of the source events can be determined by a functional block
attached to the end of the event generator (e.g. \fIFloatDC, FloatRamp\fR,
etc).
.pp
A self-scheduling star is derived from class
.c DERepeatStar
which is again derived from class
.IE "DERepeatStar"
.c DEStar .
The
.c DERepeatStar\ class
has some special methods to facilitate the self-scheduling function:
\fIrefireAtTime()\fR and \fIcanGetFired()\fR.  Let's look at an example:
the
.c Poisson
star.
.(d
defstar {
	name {Poisson}
	domain {DE}
	derivedfrom { RepeatStar }
	version { @(#)DEPoisson.pl	1.5	4/16/91}
	author { Soonhoi Ha and E. A. Lee }
	copyright { 1991 The Regents of the University of California }
	location { DE main library }
	desc {
Generates events according to a Poisson process.
The first event comes out at time zero.
	}
	explanation {
This star generates events according to a Poisson process.
The mean inter-arrival time and magnitude of the events are
given as parameters.
	}
	output {
		name {output}
		type {float}
	}
	defstate {
		name {meanTime}
		type {float}
		default {"1.0"}
		desc { The mean inter-arrival time. }
	}
	defstate {
		name {magnitude}
		type {float}
		default {"1.0"}
		desc { The magnitude of samples generated. }
	}
	hinclude { <NegativeExpntl.h> }
	ccinclude { <ACG.h> }
	protected {
		NegativeExpntl *random;
	}
// declare the static random-number generator in the .cc file
	code {
		extern ACG* gen;
	}
	constructor {
		random = NULL;
	}
	destructor {
		if(random) delete random;
	}
	start {
		if(random) delete random;
		random = new NegativeExpntl(double(meanTime),gen);
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

	   // Turn it into an exponential, and add to completionTime
	   completionTime += p;
	}
}
.)d
The
.c Poisson
star generates a Poisson arrival process.  The inter-arrival time of events
is exponentially distributed with parameter \fImeanTime\fR.
The method,
\fIrefireAtTime()\fR launches a event onto a feedback arc which is
invisible to the users.  The feedback event triggers the 
self-scheduling star some time later.
.pp
To initiate event generation, an event is placed on the feedback
arc before the scheduler
runs.  In other words, at time zero, some events are placed in the
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
is \fIrunnable\fR (ready for execution), if any input porthole
has a new event 
.c (Particle ).
Further, the star
has the responsibility to detect on which input porthole
a new event arrives.  The input porthole
containing a new particle
has the \fIdataNew\fR flag set by the 
.IE "dataNew, flag"
scheduler when the event is sent to the star.
The star
can check up the \fIdataNew\fR flag to detect the event path.
It processes the input event and generates an output event if
necessary without any latency.  
If latency is desired in a \fIfunctional-star\fR, it can 
be modeled by concatenating a \fIdelay-star\fR, explained
in the previous subsection.
Now, let's look at how to write the body of a functional star 
with an example: the
.c Switch
star.
.(d
defstar {
	name {Switch}
	domain {DE}
	desc {
Switches input events to one of two outputs, depending on
the last received control input.
	}
	version { @(#)DESwitch.pl	2.3	4/16/91}
	author { E. A. Lee }
	copyright { 1991 The Regents of the University of California }
	location { DE main library }
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
		type {=input}
	}
	output {
		name {false}
		type {=input}
	}
	constructor {
		control.triggers();
		control.before(input);
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
.)d
The
.c Switch
star has two input portholes:
\fIinput\fR, and \fIcontrol\fR.
When an event arrives at the \fIinput\fR porthole, 
it routes the event to either \fItrue\fR or \fIfalse\fR porthole
depending the value of the last received \fIcontrol\fR input.
First, we have to check
whether a new input event arrives. 
It is done by looking at the \fIdataNew\fR
flag of the \fIinput\fR porthole.
.pp
There are three methods to access a particle
from  input or output ports. First, we may use \fI%\fR operator followed
by a integer, which
is similar to the \fBSDF\fR domain mechanism.  
For example,
.c control%0
returns the most recent particle
from the \fIcontrol\fR porthole,
and
.c control%2
returns the previous particle.
The second method, \fIget()\fR, is specific to 
.IE "get(), de"
.c InDEPort .
It resets the \fIdataNew\fR member of the port as well as returning the
most recent particle
from an input port. If you need to reset the \fIdataNew\fR member of a
input port after reading the newly arrived event (the usual case)
you should use the \fIget()\fR method instead of \fI%0\fR operator.
Or, you have to reset the dataNew flag explicitly; 
.(c
input.dataNew = FASLE;
.)c
The last method, \fIput(float)\fR, is specific to 
.IE "put(), de"
.c OutDEPort .
It sets the \fItimeStamp\fR member of the port as well as returns the
most recent particle
from an output port.  Consider a line in the above example.
.(c
true.put(completionTime) = pp;
.)c
It says that we copy the particle \fBpp\fR
to the output port with \fItimeStamp = completionTime\fR.
We can send more than one output events through
the same port by calling the \fIput(float)\fR
method repeatedly.
.pp
A DE star may not generate any output event after invoked
by some input events.  In the above example, the \fIcontrol\fR
event does not "trigger" any output event while the \fIinput\fR event
does.  This behavior is described in the constructor.  
.(c
control.triggers();
.)c
.IE "triggers(), de"
By default, an input
event triggers all output events; so it is not necessary to add
.(c
input.triggers(output);
.)c
Obviously, no input triggers any output in the delay stars.
Possibly, an input event triggers conditionally an output.
Then, first check whether it is probable that
the star runs forever if the output
is connected to the input with a delay free path.  If an infinite computation
is possibly, the \fItriggers()\fR method should be called
explicitly since the detection of the delay-free loop is
based on the triggering relations (refer to the next section).
Even though the delay-free connection from the output to the input
does not result in unbounded executions, the star-writer can
specify the triggering relation to prohibit the possibility of
the delay-free path between the output to the input port.
However, this extra set-up of triggering relation is not
recommended since it may limit the usage of the star.
We can set up multiple triggering relations for a given input if the
input triggers several outputs, but not all outputs.
.pp
What happens when there are simultaneous events on the \fIinput\fR 
and the \fIcontrol\fR porthole?  There are three possibilities
for the invocation of the star.  First, the star is invoked once and
process both events at the same time.  Second, the star
is actually invoked twice, processing the control event first and the
input data next.  Lastly, the star is invoked twice but processing
the input data first.  The first possibility is most desirable while
the second scheme gives us the same result as the first one.  However,
the last scheme gives us a different answer since the output
is generated with the previous control value, not the current one,
which is an error.  To prevent this errorneous case, we define the
execution order of the simultaneous events by specifing the
priority of each input porthole.  We process the event to the
destination port of the highest priority first among all simultaneous
events.  
The \fIbefore()\fR method
defines the order of priorities of two inputs.  The detailed
procedure of assigning the priority will be explained in the next section.
The following code means that the priority of the \fIcontrol\fR
port is \fBnot smaller\fR than that of the \fIinput\fR port.
.IE "before(), de"
.(c
control.before(input);
.)c
Suppose that the control event is fed from another functional star A,
and there are two simultenous events on the \fIinput\fR port of the
.c Switch
star and an input port of star A.  For correct operation, we should
fire star A before processing the input data to the
.c Switch
star.  Now that the priority of the \fIcontrol\fR port is not smaller
than that of the \fIinput\fR port, the priority of the input port
of star A is strictly higher than that of the \fIinput\fR port of the
.c Switch
star; which guarantees that star A is fired before the
.c Switch
star.
Unlike the triggering relation, we may not define multiple "before"
relations.  Instead, we can form a link of the "before" relations
to order all inputs.  
.pp
If there are more than one simultaneous events on the same input
arc, a star should be fired repeatedly for each event.
The complicated situation arises when there are additional
simultaneous events on other input arcs.  How can we order them?
This illustrates the
potential non-determinism of the DE domain.  Some DE stars
would want to process all simultaneous events on an arc before
the events on the other arcs.  The \fIgetSimulEvent()\fR method
can be used as the following example.
.(c
DEUDCounter :: go {
	...
	while (countUp.dataNew) {
		content++;
		countUp.getSimulEvent();
	}
	...
}
.)c
.IE "getSimulEvent(), de"
The \fIgetSimulEvent()\fR method examines the global event queue
to fetch a simultaneous event to the porthole if any and set the
\fIdataNew\fR flag.  Otherwise, it just resets the flag. 
Without fetching the event, we can just
examine whether there are any simultaneous events for an input
porthole by calling "numSimulEvents()" method.  This method
is very cheap and returns the number of the simultaneous events
for the porthole.
.IE "numSimulEvents(), de"
.pp
Although we have tried to give comprehensive instructions
for designing a DE star, a good starting point is to
look at the existing stars.		
They are stored in ~ptolemy/src/domains/de/stars.
.H1 "The \\*(DO Scheduler (part II)
.pp
.H2 "Delay-free loops"
.pp
An event-path consists of the physical arcs between an output
porthole and input portholes and the triggering relations inside the
stars, on which an event flows instantaneously (without delay).
.IE "event path"
If an event-path forms a loop, we call it a delay-free loop.
.IE "delay-free loop"
While a delay-free loop in the SDF domain results in a deadlock
of the system, a delay-free loop in the DE domain potentially
causes an unbounded computation.  Therefore, it is profitable
that the delay-free loop is detected at compile-time.
If a delay-free loop is detected, the error is signaled and
simulation is halted.
.H2 "Simultaneous events"
.pp
There may be simultaneous events in the event queue.
The scheduler needs to prioritize these events to deal with them sequentially.
A common strategy is to choose one event arbitrarily.
This scheme has the simplest implementation.  We can stack up
the simultaneous events in the queue in the order in which they are created.
Since the events are randomly generated, their arrivals to the event queue
are also random.  Thereforem, this scheme is non-deterministic.
Suppose block A sends an output event to blocks B and C.
And suppose block B processes the incoming event and
generates an output event to block C.  If block B takes
zero time (or insignificant time), block C should process two input
events (one from block B, and one from block A) at the same time.
However, after A has finished, B and C both have input events,
so this scheduling strategy might invoke C before B.
This can lead to unexpected results.
Furthermore, the situation is common,
since many stars in the DE domain are delay-free, meaning that
they produce outputs with the same time stamp as their inputs.
.pp
To overcome this difficulty, the usual solution is to put an
infinitesimal time delay on the arc from block A to block C.
This delay is enough to force scheduling of block B before block C.
However, for complicated applications, this demands a great deal
of the user.
.pp
Our technique is to assign priorities to the simultaneous events.
The priority of an event is equal to the "depth"
.IE "depth, de"
of the destination port of the event (destination port of an event
is an input port of a DE star).  We define the "depth" of
an input porthole as the longest path to any termination port.
.IE "termination port"
The termination ports are the input ports that trigger no output
port and the
.c ToEventHorizon
classes inside the domain which correspond to 
the wormhole boundaries.  The depth of the termination ports becomes
$-1$ as the priority.  The depth of an output porthole is
that of the connected input minus 1, which is thus less than $-1$.  
As discussed in the previous section,
the "before" relation imposes other sets of the requirements
for assigning the depths.  
The depth of a non-termination input is determined with the
maximum of the following two values; (1) the maximum value of
the depths of all output portholes which the input triggers, and
(2) the depth of the input porthole of "before" relation with it. 
.H2 "Non-deterministic loops"
.pp
When we assign the depths of the portholes, we start from the
termination ports.  Since the assignment of the depths is performed
after the detection of delay-free loop, we can guarantee that there
is no delay-free loop in the system at this stage.  But, there is
another kind of loop, called \fInon-deterministic loop\fR.
.IE "non-deterministic loop"
A non-deterministic loop is constructed by at least one "before" relation
and the triggering relations.  The input portholes on the
non-deterministic loop can not be assigned their depths in a 
unique way.  In other words, more than one simultaneous
events lauching on a non-deterministic loop can not be ordered in
a unique fashion.  If a programmer can guarantee that there is no
possibility that simultaneous events launch on the non-deterministic
loop, the system may be simulated correctly.  Therefore, it is not
an error unlike a delay-free loop.
.pp
If a non-deterministic loop contains only one "before" relation,
the scheduler assigns depths in a well-defined way, but
unfortunately hidden to the user.  For a non-deterministic loop
with more than one "before" relations, the assignment of the depths
is a non-deterministic procedure.  Therefore, the scheduler
signals a warning message if it finds a non-deterministic loop
indicating a porthole on the loop, and finishes the current simulation
without guarantee of the correctness of the results.
The warning message suggests the programmer puts a delay element
on an arc (usually feedback arc) to break the non-deterministic loop.
The delay element has a totally different meaning from that in
the SDF domain.  In the SDF domain, a delay implies an initial token
on the arc as well as one sample delay.   In the DE domain, however,
a delay element just gives the illusion that the destination
port of the arc looks like a termination port.  Therefore, the source
port of the arc is assigned $-2$ for its depth.  The depth of the
destination port of the arc will be determined seperately.
.pp
It is worth noting that the particle movement is not
through
.c Geodesic s,
but through the global queue in the DE domain.  Since the geodesic
is a FIFO queue, we can not implement the un-chronological arrival
of the incoming events if we put the particles into the geodesics.
Instead, the particles are managed globally in the event queue.
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
or removal of redundant repetitions of identical particles.
.pp
The general mechanism of the domain interface is described in
the Almagest section, "\fBDomains in Ptolemy\fR".
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
redefins the \fIsumUp()\fR method to detect whether the wormhole 
is a process-star or not.
If the DE domain is the outer domain and the inner domain is also
timed, this method
examins the \fIstopBeforeDeadlocked\fR flag of the inner scheduler.
If the flag is set, the wormhole
is regarded as a process star in the DE domain.
.H2 "DEtoUniversal EventHorizon
.pp
.IE "DEtoUniversal
The event horizon
transfers the incoming data packets from the DE domain to
.c FromEventHorizon
another domain.  It also passes the timing information
of the DE domain to the other domain by setting time stamps.
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
The time stamp is copied from that of its counterpart after being scaled.
According to the time stamp, the received events are put into
the global queue of the \*(DO domain.
