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
    copyright { 1991 The Regents of the University of California }
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

    private
    {
	enum STATE { idle, request, grant, release };
	enum PORT { a, b };

	STATE state;
	PORT port;
    }

    start
    {
	state = idle;
	port = b;
    }


    go
    {
	switch(state)
	{
	case idle:
	    if ( (requestA.dataNew && int(requestA.get()))
		|| (requestB.dataNew && int(requestB.get())) )
	    {
		port = (int(requestA.get())) ? a : b;
		state = request;
		requestOut.put(arrivalTime) << TRUE;
	    }
	    break;

	case request:
	    if (grantIn.dataNew && int(grantIn.get()))
	    {
		state = grant;
		switch(port)
		{
		case a:
		    grantA.put(arrivalTime) << TRUE;
		    break;
		case b:
		    grantB.put(arrivalTime) << TRUE;
		    break;
		}
	    }
	    break;

	case grant:
	    if ( ((port == a) && requestA.dataNew && !int(requestA.get()))
		|| ((port == b) && requestB.dataNew && !int(requestB.get())) )
	    {
		state = release;
		requestOut.put(arrivalTime) << FALSE;
	    }
	    break;

	case release:
	    if (grantIn.dataNew && !int(grantIn.get()))
	    {
		state = idle;
		switch(port)
		{
		case a:
		    grantA.put(arrivalTime) << FALSE;
		    if (requestB.dataNew && int(requestB.get()))
		    {
			port = b;
			state = request;
			requestOut.put(arrivalTime) << TRUE;
		    }
		    break;
		case b:
		    grantB.put(arrivalTime) << FALSE;
		    if (requestA.dataNew && int(requestA.get()))
		    {
			port = a;
			state = request;
			requestOut.put(arrivalTime) << TRUE;
		    }
		    break;
		}
	    }
	    break;
	}
    }
}
