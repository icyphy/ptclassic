defcore {
    name { Gain }
    domain { ACS }
    coreCategory { FPSim }
    corona { Gain } 
    desc { This is an amplifier; the output is the input multiplied by the "gain" (default 1.0). }
    version { $Id$ }
    author { James Lundblad }
    copyright {
Copyright (c) 1998 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
    }
    location { ACS main library }

	defstate {
		name { gain }
		type { float }
		default { "1.0" }
		desc { Gain of the star. }
	}
	go {
		corona.output%0 << double(gain) * double(corona.input%0);
	}
}
