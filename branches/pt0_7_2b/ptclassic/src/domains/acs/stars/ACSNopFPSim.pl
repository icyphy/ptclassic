defcore
 {
	name { Nop }
	domain { ACS }
	coreCategory { FPSim }
	corona { Nop }
	desc { Does nothing }
	version {@(#)ACSNopFPSim.pl	1.0 8/4/99}
        author { Eric K. Pauer }
        copyright {
Copyright (c) 1998-1999 The Regents of the University of California
and Sanders, a Lockheed Martin Company
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { ACS main library }
	go {
	    corona.output%0 = corona.input%0;
	}
}

