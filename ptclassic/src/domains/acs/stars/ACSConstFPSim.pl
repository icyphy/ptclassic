defcore {
    name { Const }
    domain { ACS }
    coreCategory { FPSim }
    corona { Const } 
    desc { Output the constant level as a floating-point value. }
    version { @(#)ACSConstFPSim.pl	1.1 05/07/98 }
    author { Eric Pauer }
    copyright {
Copyright (c) 1998 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
    }
    location { ACS main library }
    defstate {
	name { level }
	type{ float }
	default {"0.0"}
	desc {The constant value. }
    }

    go {
	corona.output%0 << level;
    }
}



