defstar
{
    name { Timeout }
    derivedFrom { TimeoutStar }
    domain { DE }
    descriptor { Detect a timeout condition and generate an alarm. }
    version { $Id$ }
    author { T. M. Parks }
    copyright { 1992 The Regents of the University of California }
    location { DE main library }

    input
    {
	name { Set }
	type { anyType }
	desc { Start the timer from zero. }
    }

    input
    {
	name { Clear }
	type { anyType }
	desc { Stop the timer. }
    }

    output
    {
	name { alarm }
	type { float }
	desc { Indicates that the timer has reached timeout. }
    }

    go
    {
	if (Clear.dataNew)
	{
	    Clear.dataNew = FALSE;
	    clear();
	}

	if (Set.dataNew)
	{
	    Set.dataNew = FALSE;
	    set();
	}

	if (expired)
	{
	    alarm.put(arrivalTime) << arrivalTime;
	}
    }
}
