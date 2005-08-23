defcore {
    name { Register }
    domain { ACS }
    coreCategory { FPSim }
    corona { Register } 
    desc { Provides explicit pipeline delay (default 1). }
    version{ @(#)ACSRegisterFPSim.pl	1.2 08/02/01 }
    author { Eric Pauer }
    copyright {
Copyright (c) 1999- Sanders, a Lockheed Martin Company
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
    }
    location { ACS main library }

        setup {
                corona.input.setSDFParams(1, 1);
		corona.input%1 << (double)0.0;
	}
	go {
		corona.output%0 << (double)(corona.input%1);
	}
}
