defstar
{
    name { Timeout }
    derivedFrom { TimeoutStar }
    domain { DE }
    descriptor { Detect a timeout condition and generate an alarm. }
    version { $Id$ }
    author { T. M. Parks }
    copyright
    {
Copyright 1992 The Regents of the University of California.
All rights reserved.
See the file ~ptolemy/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
    }
    location { DE main library }

    input
    {
	name { Set }
	type { anyType }
	desc { Reset and start the timer. }
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
	type { = Set }
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

	if (expired())
	{
	    // Output the event which started the timer.
	    alarm.put(arrivalTime) = Set%0;
	}
    }
}
