defstar
{
    name { HandShake }
    domain { DE }
    descriptor
    {
This star cooperates with a (possibly preemptive) arbitrator.
A "request" TRUE is generated in response to new "input" particles.
If no new "input" is available when it comes time to send the next
"output", a "request" FALSE is generated.  When a "grant" TRUE is
received in response to a "request", an "output" particle is sent.
This must be acknowledged by an "ackIn" particle before the next
"output" can be sent.  Any "ackIn" particle which is received in
response to an "output" is sent to "ackOut".  All other "ackIn"
particles are discarded.
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
	desc { Indicates presence or absence of "input" particles. }
    }

    input
    {
	name { grant }
	type { int }
	desc { Enables and disables sending of "output" particles. }
    }

    protected
    {
	int req : 1;	// request has been issued
	int open : 1;	// output is enabled
	int wait : 1;	// waiting for acknowledge
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
	    if (req && open && !wait && input.dataNew)	// send data
	    {
		wait = TRUE;
		output.put(arrivalTime) = input.get();
		ackIn.dataNew = FALSE;	// discard any old particle
	    }
	}

	if (input.dataNew && !req)	// new data received
	{
	    request.put(arrivalTime) << (req = TRUE);
	}

	if (ackIn.dataNew && wait)	// acknowledge received
	{
	    wait = FALSE;
	    ackOut.put(arrivalTime) = ackIn.get();
	    if (!input.dataNew)		// release
	    {
		request.put(arrivalTime) << (req = FALSE);
	    }
	    else if (open)		// send data
	    {
		wait = TRUE;
		output.put(arrivalTime) = input.get();
	    }
	}
    }
}
