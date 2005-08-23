defcore {
    name { Fork }
    domain { ACS }
    coreCategory { FPSim }
    corona { Fork } 
    desc { Copy input particles to each output. }
    version { @(#)ACSForkFPSim.pl	1.4 09/08/99}
    author { James Lundblad }
    copyright {
Copyright (c) 1998 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
    }
    location { ACS main library }

    go {
		MPHIter nextp(corona.output);
		PortHole* p;
		while ((p = nextp.next()) != 0)
			(*p)%0 = corona.input%0;
    }
}
