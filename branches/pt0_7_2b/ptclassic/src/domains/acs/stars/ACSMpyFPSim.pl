defcore {
    name { Mpy }
    domain { ACS }
    coreCategory { FPSim }
    corona { Mpy } 
    desc { Output the product of the inputs, as a floating-point value. }
    version { @(#)ACSMpyFPSim.pl	1.4 09/08/99}
    author { James Lundblad }
    copyright {
Copyright (c) 1998 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
    }
    location { ACS main library }

    go {
		MPHIter nexti(corona.input);
		PortHole *p;
		double product = 1.0;
		while ((p = nexti.next()) != 0)
			product *= double((*p)%0);
		corona.output%0 << product;
    }
}
