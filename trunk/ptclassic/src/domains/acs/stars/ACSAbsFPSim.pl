defcore {
	name { Abs }
	domain { ACS }
	coreCategory { FPSim }
	corona { Abs }
	desc { Takes the absolute value of the input. }
	version { @(#)ACSAbsFPSim.pl	1.3 09/08/99 }
	author { James Lundblad }
	copyright {
Copyright (c) 1998 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
    	}
    	location { ACS main library }

	go {
		double t = corona.input%0;
		if (t < 0.0) t = -t;
		corona.output%0 << t;
	}
}
