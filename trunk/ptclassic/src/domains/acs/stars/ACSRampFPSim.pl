defcore {
    name { Ramp }
    domain { ACS }
    coreCategory { FPSim }
    corona { Ramp } 
    desc { Output the product of the inputs, as a floating-point value. }
    version { @(#)ACSRampFPSim.pl	1.4 09/08/99}
    author { James Lundblad }
    copyright {
Copyright (c) 1998 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
    }
    location { ACS main library }

	go {
		double t = corona.value;
		corona.output%0 << t;
		t += corona.step;
		corona.value = t;
	}

}
