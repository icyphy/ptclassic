defcore {
    name { Add }
    domain { ACS }
    coreCategory { FPSim }
    corona { Add } 
    desc { Output the product of the inputs, as a floating-point value. }
    version { @(#)ACSAddFPSim.pl	1.5 09/08/99}
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
		double sum = 0.0;
		while ((p = nexti.next()) != 0)
			sum += double((*p)%0);
		corona.output%0 << sum;
	}
}
