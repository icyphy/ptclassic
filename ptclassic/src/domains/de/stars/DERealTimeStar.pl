defstar
{
    name { RealTimeStar }
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

    start
    {
	clock.reset();
    }

    virtual method
    {
	name { run }
	access { public }
	type { int }
	code
	{   // wait until the real time equals arrivalTime
	    late = !clock.sleepUntil(arrivalTime * (double)timeScale);
	    return DEStar::run();
	}
    }
}
