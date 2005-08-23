defcore {
    name { 16FFT }
    domain { ACS }
    coreCategory {FixSim}
    corona {16FFT}
    desc { Compute a 16point complex FFT }
    version { @(#)ACS16FFTFixSim.pl	1.0 03/03/00 }
    author { Ken Smith }
    copyright {
Copyright (c) 1998-1999 Sanders, a Lockheed Martin Company
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
}
    location { ACS main library }
    defstate {
	name { OutputPrecision }
	type { precision }
	default { 2.14 }
	desc {
Precision of the output in bits.
This is the precision that will hold the result of the arithmetic operation
on the inputs.
When the value of the product extends outside of the precision,
the OverflowHandler will be called.
	}
    }
    defstate {
	name { LockOutput }
	type {int}
	default {"NO"}
	desc { 
Flag that indicates that the specified output precision should be used 
rather than modified by wordlength analysis in the FPGA domain }
    }
    defstate {
	name { ArrivingPrecision }
	type {int}
	default {"YES"}
	desc { 
Flag that indicates whether or not to keep the precision of the arriving
particles as is:  YES keeps the same precision, and NO casts them to the
precision specified by the parameter "InputPrecision". } 
    }
    defstate {
	name { InputPrecision }
	type { precision }
	default { 2.14 }
	desc { 
Precision of the input in bits.
The input particles are only cast to this precision if the
parameter "ArrivingPrecision" is set to NO. }
    }
    defstate {
	name { LockInput }
	type {int}
	default {"NO"}
	desc { 
Flag that indicates that the specified input precision should be used 
rather than modified by wordlength analysis in the FPGA domain }
    }
    setup {
    }
    go {
    }
}
