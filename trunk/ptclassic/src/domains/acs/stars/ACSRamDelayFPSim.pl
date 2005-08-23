defcore {
    name { RamDelay }
    domain { ACS }
    coreCategory { FPSim }
    corona { RamDelay } 
    desc { Provides explicit pipeline delay (default 1). }
    version{ @(#)ACSRamDelayFPSim.pl	1.5 08/02/01 }
    author { Eric Pauer }
    copyright {
Copyright (c) 1999- Sanders, a Lockheed Martin Company
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
    }
    location { ACS main library }

        setup {
                corona.input.setSDFParams(1, int(corona.delays));
		for (int i = 0; i < int(corona.delays); i++)
		  corona.input%i << (double)0.0;
	}
	go {
		corona.output%0 << (double)(corona.input%(int(corona.delays)));
	}
}
