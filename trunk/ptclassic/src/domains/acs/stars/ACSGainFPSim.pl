defcore {
    name { Gain }
    domain { ACS }
    coreCategory { FPSim }
    corona { Gain } 
    desc { This is an amplifier; the output is the input multiplied by the "gain" (default 1.0). }
    version { @(#)ACSGainFPSim.pl	1.4 09/08/99}
    author { James Lundblad }
    copyright {
Copyright (c) 1998 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
    }
    location { ACS main library }

	go {
		corona.output%0 << double(corona.gain) * double(corona.input%0);
	}
}
