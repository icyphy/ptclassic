defcore {
        name { Max }
	domain { ACS }
	coreCategory { FPSim }
	corona { Max }
	desc { Produces the maximum of two inputs }
	version { @(#)ACSMaxFPSim.pl	1.0 12/13/00 }
	author { J. Ramanathan }
	copyright {
Copyright (c) 1998 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
    	}
    	location { ACS main library }
	go {
	    if ((double)(corona.input1%0) > (double)(corona.input2%0))
		corona.output%0 << (double)(corona.input1%0);
	    else
		corona.output%0 << (double)(corona.input2%0);
	}
}


