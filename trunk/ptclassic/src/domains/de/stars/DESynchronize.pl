defstar
{
    name { Synchronize }
    derivedFrom { RealTimeStar }
    domain { DE }
    descriptor {
Input events are held until the time elapsed on the
system clock since the start of the simulation
is greater than or equal to their time stamp.
Then are passed to the output.
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
	name { input }
	type { anyType }
    }

    output
    {
	name { output }
	type { = input }
    }

    go
    {
	output.put(arrivalTime) = input.get();
    }
}
