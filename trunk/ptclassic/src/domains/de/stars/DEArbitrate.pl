defstar
{
    name { Arbitrate }
    domain { DE }
    descriptor
    {
This star acts as a non-preemptive arbitrator, granting requests for
exclusive control.  If simultaneous "requests" arrive, priority is
given to port A.  When control is released, any pending "requests" on
the other port will be serviced.  The "requestOut" and "grantIn"
connections allow interconnection of multiple arbitration stars for
more intricate control structures.
    }
    version { $Id$ }
    author { T.M. Parks }
	copyright {
Copyright (c) 1990, 1991, 1992 The Regents of the University of California.
All rights reserved.
See the file ~ptolemy/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
    location { DE main library }

    input
    {
	name { requestA }
	type { int }
    }

    input
    {
	name { requestB }
	type { int }
    }

    output
    {
	name { requestOut }
	type { int }
    }

    output
    {
	name { grantA }
	type { int }
    }

    output
    {
	name { grantB }
	type { int }
    }

    input
    {
	name { grantIn }
	type { int }
    }

    protected
    {
	InDEPort *request;	// request for current port
	OutDEPort *grant;	// grant for current port

	int idle : 1;	// arbitrator is idle
	int req : 1;	// request is pending
	int rel : 1;	// release is pending
    }

    constructor {
	requestA.triggers(requestOut);
	requestB.triggers(requestOut);
	grantIn.triggers(grantA);
	grantIn.triggers(grantB);
	grantIn.before(requestA);
    }
    start
    {
	idle = TRUE;
	req = rel = FALSE;
    }


    go
    {
	if (grantIn.dataNew)	// response to earlier "requestOut" output
	{
	    if (req && int(grantIn.get()))	// grant request
	    {
		req = FALSE;
		grant->put(arrivalTime) << TRUE;
	    }
	    if (rel && !int(grantIn.get()))	// grant release
	    {
		rel = FALSE;
		idle = TRUE;
		grant->put(arrivalTime) << FALSE;
		request->dataNew = FALSE;	// give other port a chance
	    }
	}

	if ((requestA.dataNew || requestB.dataNew) && idle)	// new request
	{
	    if (requestA.dataNew)	// assign port A
	    {
		request = &requestA;
		grant = &grantA;
	    }
	    else			// assign port B
	    {
		request = &requestB;
		grant = &grantB;
	    }

	    if (int(request->get()))	// generate request
	    {
		idle = FALSE;
		req = TRUE;
		requestOut.put(arrivalTime) << TRUE;
	    }
	}

	if (request->dataNew && !idle && !req && !rel)	// new release
	{
	    if (!int(request->get()))	// generate release
	    {
		rel = TRUE;
		requestOut.put(arrivalTime) << FALSE;
	    }
	}
    }
}
