defstar
{
    name { RealTimeStar }
    derivedFrom { RepeatStar }
    domain { DE }
    descriptor { Synchronize the scheduler's clock with a real-time clock. }
    version { $Id$ }
    author { T.M. Parks }
    copyright { 1991 The Regents of the University of California. }
    location { DE main library }

    hinclude { "Clock.h" }

    state
    {
	name { timeScale }
	type { float }
	default { 1.0 }
	desc { Duration of a time unit in seconds. }
    }

    protected
    {
	static Clock clock;	// common clock for all real-time stars
	int late : 1;		// scheduler has fallen behind real-time
    }

    code
    {
	Clock DERealTimeStar::clock;
    }

    virtual method
    {
	name { run }
	access { public }
	type { int }
	code
	{
	    if (canGetFired())	// reset clock at time 0.0
		clock.reset();
	    else		// wait until the real time equals arrivalTime
		late = !clock.sleepUntil(arrivalTime * (double)timeScale);
	    return DEStar::run();
	}
    }
}
