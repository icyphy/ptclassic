defstar
{
    name { Timer }
    domain { DE }
    descriptor {
Upon receiving a "trigger" input, output the elapsed real time
in seconds, divided by "timeScale", since the last reset input,
or since the start of the simulation if no reset has
been received.  The time stamp on the output is the same as
that of the "trigger" input. The time in seconds is related to the
scheduler time through the scaling factor "timeScale".
    }
    version { $Id$ }
    author { T.M. Parks }
	copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
    location { DE main library }

    hinclude { "Clock.h" }

    state
    {
	name { timeScale }
	type { float }
	default { 1.0 }
	desc { Duration of a scheduler time unit in seconds. }
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

	setup
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
