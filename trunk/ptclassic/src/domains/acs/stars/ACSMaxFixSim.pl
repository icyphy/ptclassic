defcore {
	name { Max }
	domain { ACS }
	coreCategory { FixSim }
	corona { Max }
	desc { Produced the maximum of two inputs	}
	version { @(#)ACSMaxFixSim.pl	1.0 12/13/00 }
	author { J. Ramanathan }
	copyright {
Copyright (c) 1998 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
    	}
    	location { ACS main library }
        defstate {
                name { InputPrecision }
                type { precision }
                default { 4.14 }
                desc {
Precision of the input in bits.
The input particles are only cast to this precision if the parameter
"ArrivingPrecision" is set to NO.
		}
        }
	defstate {
	    name { LockInput }
	    type {int}
	    default {"NO"}
	    desc { 
Flag that indicates that the specified input precision should be used 
rather than modified by wordlength analysis in the FPGA domain }
	}
        defstate {
                name { OutputPrecision }
                type { precision }
                default { 4.14 }
                desc { Precision of the output in bits. } 
        }
	defstate {
	    name { LockOutput }
	    type {int}
	    default {"NO"}
	    desc { 
Flag that indicates that the specified output precision should be used 
rather than modified by wordlength analysis in the FPGA domain }
	}
        protected {
	    Fix fixIn1, fixIn2, fixOut;
	}
	setup {
	    fixIn1 = Fix(((const char *) InputPrecision));
	    if (fixIn1.invalid())
	      Error::abortRun( *this, "Invalid InputPrecision" );
	    fixIn2 = Fix(((const char *) InputPrecision));
	    if (fixIn2.invalid())
	      Error::abortRun( *this, "Invalid InputPrecision" );
	    fixOut = Fix(((const char *) OutputPrecision));
	    if (fixOut.invalid())
	      Error::abortRun( *this, "Invalid OutputPrecision" );
	}
	go {
	    Fix temp = corona.input1%0;
	    fixIn1 = temp;
	    Fix temp2 = corona.input2%0;
	    fixIn2 = temp2;
	    if (fixIn1 > fixIn2)
		fixOut = fixIn1;
	    else
		fixOut = fixIn2;
	    corona.output%0 << fixOut;
	}
}
