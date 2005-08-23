defcore {
	name { Source }
	domain { ACS }
	coreCategory { FixSim }
	corona { Source }
	desc {
	    Discards input samples.
	}
	version {@(#)ACSSourceFixSim.pl	1.2 09/08/99}
	author { Eric Pauer }
	copyright {
Copyright (c) 1999 Sanders, a Lockheed Martin Company
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { ACS main library }
	defstate {
	    name { LockOutput }
	    type {int}
	    default {"NO"}
	    desc { 
Flag that indicates that the specified output precision should be used 
rather than modified by wordlength analysis in the FPGA domain }
	}
}

