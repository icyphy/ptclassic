defstar {
    name { Timeout }
    derivedFrom { TimeoutStar }
    domain { DE }
    descriptor {
This star detects time-out conditions and generates an alarm if too
much time elapses before resetting or stopping the timer.  Events
arriving on the "Set" input reset and start the timer.  Events arriving
on the "Clear" input stop the timer.  If no "Set" or "Clear" events
arrive within "timeout" time units of the most recent "Set", then that
"Set" event is sent out the "alarm" output.
    }
    version { $Id$ }
    author { T. M. Parks }
    copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
    }
    location { DE main library }

    input {
	name { Set }
	type { anyType }
	desc { Reset and start the timer. }
    }

    input {
	name { Clear }
	type { anyType }
	desc { Stop the timer. }
    }

    output {
	name { alarm }
	type { = Set }
	desc
	{
Indicates that the duration of the time-out has elapsed.
The event which last started the timer is output.
	}
    }

    go {
	if (Clear.dataNew) {
	    Clear.dataNew = FALSE;
	    clear();
	}

	if (Set.dataNew) {
	    Set.dataNew = FALSE;
	    set();
	}

	if (expired()) {
	    // Output the event which started the timer.
	    alarm.put(arrivalTime) = Set%0;
	}
    }
}
