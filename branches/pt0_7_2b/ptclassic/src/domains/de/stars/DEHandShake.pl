defstar
{
    name { HandShake }
    domain { DE }
    descriptor
    {
This star cooperates with a (possibly preemptive) arbitrator.
A "request" TRUE is generated in response to new "input" particles,
which are queued internally.  If the queue ever becomes empty,
a "request" FALSE is generated and the star reverts back to its
initial state.

When a "grant" TRUE is received in response to a "request",
an "output" particle is sent.  This must be acknowledged by an
"ackIn" particle before the next "output" can be sent. Any "ackIn"
particle which is received in response to an "output" is sent to
"ackOut".  All other "ackIn" particles are discarded.
    }
    version { $Id$ }
    author { T.M. Parks }
    copyright { 1991 The Regents of the University of California }
    location { DE main library }

    input
    {
	name { input }
	type { anyType }
    }

    output
    {
	name { output }
	type { = input }
    }

    input
    {
	name { ackIn }
	type { anyType }
	desc { Acknowledges receipt of most recent "output" particle. }
    }

    output
    {
	name { ackOut }
	type { = ackIn }
	desc { Response to most recent "output" particle. }
    }

    output
    {
	name { request }
	type { int }
	desc { Indicates presence or absence of queued "input" particles. }
    }

    input
    {
	name { grant }
	type { int }
	desc { Enables and disables sending of "output" particles. }
    }

    defstate
    {
	name { capacity }
	type { int }
	default { 0 }
	desc
	{
Capacity of internal queue. Specify capacity <= 0 for unbounded queueing.
	}
    }

    ccinclude { "DataStruct.h" }

    protected
    {
	int req : 1;	// request has been issued
	int open : 1;	// output is enabled
	int wait : 1;	// waiting for acknowledge
	Queue queue;
    }

    start
    {
	req = open = wait = FALSE;
    }

    go
    {
	if (grant.dataNew)	// new control received
	{
	    open = int(grant.get());	// enable or disable output
	    if (req && open && !wait && queue.length() > 0)	// send data
	    {
		wait = TRUE;
		output.put(arrivalTime) = *(Particle*)queue.get();
		ackIn.dataNew = FALSE;	// clear out any old particles
	    }
	}

	if (input.dataNew)	// new data received
	{
	    Particle& pp = input.get();
	    if (int(capacity) <= 0 || queue.length() < int(capacity))
	    {
		queue.put(pp.clone());	// put data in queue
		if (!req)		// generate request
		{
		    request.put(arrivalTime) << (req = TRUE);
		}
		else if (open && !wait)	// send data
		{
		    wait = TRUE;
		    output.put(arrivalTime) = *(Particle*)queue.get();
		    ackIn.dataNew = FALSE;	// clear out any old particles
		}
	    }
	}

	if (ackIn.dataNew && wait)	// acknowledge received
	{
	    wait = FALSE;
	    ackOut.put(arrivalTime) = ackIn.get();	// pass data through
	    if (queue.length() == 0)	// release
	    {
		request.put(arrivalTime) << (req = FALSE);
	    }
	    else if (open)	// send data
	    {
		wait = TRUE;
		output.put(arrivalTime) = *(Particle*)queue.get();
	    }
	}
    }
}
