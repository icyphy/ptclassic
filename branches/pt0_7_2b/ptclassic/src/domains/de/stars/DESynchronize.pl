defstar
{
    name { Synchronize }
    derivedFrom { RealTimeStar }
    domain { DE }
    descriptor { Synchronize clocks before passing data. }
    version { $Id$ }
    author { T.M. Parks }
    copyright { 1991 The Regents of the University of California. }
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
