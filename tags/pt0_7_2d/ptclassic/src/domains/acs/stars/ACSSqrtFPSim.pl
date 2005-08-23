defcore {
    name { Sqrt }
    domain { ACS }
    coreCategory { FPSim }
    corona { Sqrt } 
    desc { This star computes the square root of its input. }
    version { @(#)ACSSqrtFPSim.pl	1.2 09/08/99 }
    author { James Lundblad }
    copyright {
Copyright (c) 1998 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
    }
    location { ACS main library }
     ccinclude { <math.h> }
	go {
		corona.output%0 << sqrt (double(corona.input%0));
	}
}
