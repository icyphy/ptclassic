defcore {
    name { PolarToRect }
    domain { ACS }
    coreCategory { FPSim }
    corona { PolarToRect } 
    desc { Convert magnitude and phase to rectangular form. }
    version { @(#)ACSPolarToRectFPSim.pl	1.4 09/08/99}
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
		double m = corona.magnitude%0;
		double p = corona.phase%0;
		corona.x%0 << m * cos(p);
		corona.y%0 << m * sin(p);
	}
}
