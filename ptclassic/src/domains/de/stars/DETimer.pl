defstar
{
    name { Timer }
    domain { DE }
    descriptor { Measure real elapsed time between events. }
    version { $Id$ }
    author { T.M. Parks }
	copyright {
Copyright (c) 1990, 1991, 1992 The Regents of the University of California.
All rights reserved.
See the file ~ptolemy/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
    location { DE main library }

    hinclude { "Clock.h" }

    state
    {
	name { timeScale }
	type { float }
	default { 1.0 }
	desc { Duration of a time unit in seconds. }
    }

    input
    {
	name { trigger }
	type { anyType }
    }

    input
    {
	name { reset }
	type { anyType }
    }

    output
    {
	name { elapsedTime }
	type { float }
    }

    protected
    {
	Clock clock;
    }

    start
    {
	clock.reset();
    }

    go
    {
	TimeVal now = clock.elapsedTime();

	if (trigger.dataNew)
	{
	    trigger.dataNew = FALSE;
	    if (reset.dataNew)
	    {
		elapsedTime.put(arrivalTime) << 0.0;
	    }
	    else
	    {
		elapsedTime.put(arrivalTime) << (now / timeScale);
	    }
	}
	if (reset.dataNew)
	{
	    reset.dataNew = FALSE;
	    clock.reset();
	}
    }
}
