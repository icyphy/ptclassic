defcore {
    name { Sub }
    domain { ACS }
    coreCategory { FPSim }
    corona { Sub } 
    desc { Output the difference of the inputs, as a floating-point value. }
    version { @(#)ACSSubFPSim.pl	1.4 09/08/99}
    author { James Lundblad }
    copyright {
Copyright (c) 1998 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
    }
    location { ACS main library }
    go {
		double diff;
		diff = double(corona.pos%0);
		diff -= double(corona.neg%0);
		corona.output%0 << diff;
    }
}
