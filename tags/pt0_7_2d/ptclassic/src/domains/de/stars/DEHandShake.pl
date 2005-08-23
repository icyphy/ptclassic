defstar
{
    name { HandShake }
    domain { DE }
    descriptor
    {
Cooperate with a (possibly preemptive) arbitrator through the
"request" and "grant" controls.  "Input" particles are passed to
"output", and an "ackIn" particle must be received before the next
"output" can be sent.  This response is made available on "ackOut".
    }
    explanation
    {
A <i> request </i> TRUE is generated in response to a new <i> input.</i>
After a <i> grant </i> TRUE response is received, the <i> input </i> will
be passed to the <i> output.</i>  After this <i> output </i> is
acknowledged by an <i> ackIn </i> response, the next <i> input </i> is
passed to the <i> output </i> if it is available, and the process
repeats.  If no new <i> input </i> is available when the <i> ackIn </i>
is received, a <i> request </i> FALSE is generated.  When the <i> grant </i>
FALSE response is received, the star reverts to its original idle state.
.lp
Note that the star will not generate any spurious outputs on either <i>
output </i> or <i> request </i> while it is waiting for a response on
the <i> grant </i> or <i> ackIn </i> inputs.  Also, only those <i> ackIn </i>
particles received in response to an <i> output </i> will be sent to <i>
ackOut.</i>
    }
    version { @(#)DEHandShake.pl	1.12 10/03/96 }
    author { T.M. Parks }
	copyright {
Copyright (c) 1990-1997 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
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
	int idle : 1;	// arbitration is idle
	int req : 1;	// request is pending
	int rel : 1;	// release is pending
	int open : 1;	// output is enabled
	int wait : 1;	// waiting for acknowledge
    }

    constructor
    {
	input.triggers(request);
	input.before(ackIn);
	ackIn.triggers(output);
	ackIn.triggers(ackOut);
	grant.triggers();
	grant.before(ackIn);
    }

	setup
    {
	idle = TRUE;
	req = rel = open = wait = FALSE;
    }

    go
    {
	if (grant.dataNew)	// new control received
	{
	    open = int(grant.get());	// enable or disable output
	    if (req && open)		// request acknowledged
	    {
		req = FALSE;
	    }
	    else if (rel && !open)	// release acknowledged
	    {
		rel = FALSE;
		idle = TRUE;
	    }
	}

	if (ackIn.dataNew && wait)	// acknowledge received
	{
	    wait = FALSE;
	    ackOut.put(arrivalTime) = ackIn.get();
	    if (!input.dataNew)		// generate release
	    {
		rel = TRUE;
		request.put(arrivalTime) << FALSE;
	    }
	}

	if (input.dataNew)	// new data available
	{
	    if (idle)		// generate request
	    {
		idle = FALSE;
		req = TRUE;
		request.put(arrivalTime) << TRUE;
	    }
	    else if (open && !req && !rel && !wait)	// send data
	    {
		wait = TRUE;
		output.put(arrivalTime) = input.get();
		ackIn.dataNew = FALSE;	// discard any old particle
	    }
	}
    }
}
