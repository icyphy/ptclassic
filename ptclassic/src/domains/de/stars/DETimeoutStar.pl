defstar
{
    name { TimeoutStar }
    derivedFrom { RepeatStar }
    domain { DE }
    descriptor { Base class for stars which check timeout conditions. }
    version { $Id$ }
    author { T. M. Parks }
    copyright { 1992 The Regents of the University of California }
    location { DE main library }

    state
    {
	name { timeout }
	type { float }
	default { 1.0 }
    }

    protected
    {
	int safe : 1;
	int expired : 1;
	double deadline;
    }

    start
    {
	safe = TRUE;
	expired = FALSE;
    }

    method
    {
	name { set }
	type { void }
	code
	{
	    safe = FALSE;
	    expired = FALSE;
	    deadline = arrivalTime + timeout;
	    refireAtTime(deadline);
	}
    }

    method
    {
	name { clear }
	type { void }
	code
	{
	    safe = TRUE;
	    expired = FALSE;
	}
    }

    method
    {
	name { run }
	type { int }
	access { public }
	code
	{
	    if (canGetFired())	// timeout event
	    {
		if (!safe && arrivalTime == deadline)
		    expired = TRUE;
	    }
	    return DERepeatStar::run();
	}
    }
}
