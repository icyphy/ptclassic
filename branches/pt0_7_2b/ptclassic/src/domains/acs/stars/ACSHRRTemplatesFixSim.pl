defcore {
    name { HRRTemplates }
    domain { ACS }
    coreCategory { FixSim }
    corona { HRRTemplates }
    desc { Produces a subset of the template data based on the incoming angle }
    version { @(#)ACSHRRTemplatesFixSim.pl	1.0 12/18/00 }
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
	    Fix fixIn, fixOut;
	}
	setup {
	    fixIn = Fix(((const char *) InputPrecision));
	    if (fixIn.invalid())
	      Error::abortRun( *this, "Invalid InputPrecision" );
	    fixOut = Fix(((const char *) OutputPrecision));
	    if (fixOut.invalid())
	      Error::abortRun( *this, "Invalid OutputPrecision" );
	}
	go {
	    // Meaningless
	    Fix temp = corona.input%0;
	    fixIn = temp;
	    fixOut = fixIn;
	    corona.output%0 << fixOut;
	}
}
