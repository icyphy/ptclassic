defcore {
	name { Source }
	domain { ACS }
	coreCategory { FPCGC }
	corona { Source }
	desc {
	    Discards input samples.
	}
	version {$Id$ }
	author { Eric Pauer }
	copyright {
Copyright (c) 1999-%Q% Sanders, a Lockheed Martin Company
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