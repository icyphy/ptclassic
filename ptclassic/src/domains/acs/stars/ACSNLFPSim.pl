defcore {
	name { NL }
	domain { ACS }
	coreCategory { FPSim }
	corona { NL }
	desc { nonlinear function providing lower threshold, variable gain linear region, and upper threshold. }
	version { @(#)ACSNLFPSim.pl	1.2 09/08/99 }
	author { James Lundblad }
	copyright {
Copyright (c) 1998 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
    	}
    	location { ACS main library }

	defstate {
		name { threshold1 }
		type { float }
		default { 0.0 }
		desc { lower threshold below which output is zero. }
	}
	defstate {
		name { gain }
		type { float }
		default { 0.5 }
		desc { gain of linear region. }
	}
	defstate {
		name { threshold2 }
		type { float }
		default { 15.0 }
		desc { upper threshold above which output saturates. }
	}
	go {
		double t = corona.input%0;
		if ( t < double(threshold1) ) 
			t = 0.0;
		else
			t = double(gain) * t;
		if ( t > double(threshold2) )
			t = double(threshold2);
		corona.output%0 << t;
	}
}
