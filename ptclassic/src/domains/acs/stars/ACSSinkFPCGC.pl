defcore {
	name { Sink }
	domain { ACS }
	coreCategory { FPCGC }
	corona { Sink }
	desc {
	    Discards input samples.
	}
	version {@(#)ACSSinkFPCGC.pl	1.2 09/08/99}
	author { Eric Pauer }
	copyright {
Copyright (c) 1999 Sanders, a Lockheed Martin Company
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { ACS main library }
	constructor {
		noInternalState();
	}
	go {
		addCode("/* This star generates no code */");
	}
	exectime {
		return 0;
	}
}
