defcore {
    name { Sub }
    domain { ACS }
    coreCategory { FPSim }
    corona { Sub } 
    desc { Output the product of the inputs, as a floating-point value. }
    version { $Id$ }
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
		MPHIter nexti(corona.neg);
		for(int i=corona.neg.numberPorts(); i>0; i--)
			diff -= double((*nexti++)%0);
		corona.output%0 << diff;
    }
}
