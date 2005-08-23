defcore {
    name { Cos }
    domain { ACS }
    coreCategory { FPSim }
    corona { Cos } 
    desc { This star computes the cosine of its input, assumed to be an angle in radians. }
    version { @(#)ACSCosFPSim.pl	1.4 09/08/99}
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
		corona.output%0 << cos (double(corona.input%0));
	}
}
