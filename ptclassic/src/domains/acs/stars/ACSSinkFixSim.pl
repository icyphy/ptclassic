defcore {
	name { Sink }
	domain { ACS }
	coreCategory { FixSim }
	corona { Sink }
	desc {
	    Discards input samples.
	}
	version{ $Id$ }
	author { Eric Pauer }
	copyright {
Copyright (c) 1999-%Q% Sanders, a Lockheed Martin Company
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { ACS main library }
	defstate {
	    name { LockInput }
	    type {int}
	    default {"NO"}
	    desc { 
Flag that indicates that the specified input precision should be used 
rather than modified by wordlength analysis in the FPGA domain }
	}
}

