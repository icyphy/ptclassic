defcore {
    name { Sqr }
    domain { ACS }
    coreCategory { FixSim }
    corona { Sqr } 
    desc { Output the product of the inputs, as a floating-point value. }
    version { @(#)ACSSqrFixSim.pl	1.0 10/30/00}
    author { J. Ramanathan }
    copyright {
Copyright (c) 1998 The Regents of the University of California.
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
	        type {int }
	        default { "YES" }
	        desc {
Indicates whether or not to keep the precision of the arriving particles
as they are:  YES keeps the same precision, and NO casts the inputs
to the precision specified by the parameter "InputPrecision".
		}
	}
	defstate {
	        name { InputPrecision }
	        type { precision }
	        default { 2.14 }
	        desc {
Sets the precision of the input in bits.
The input particles are only cast to this precision if the
parameter "ArrivingPrecision" is set to NO.
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
	protected {
		Fix fixIn, product;
	}
	setup {
		ACSFixSimCore::setup();

		if ( ! int(ArrivingPrecision) ) {
		  fixIn = Fix( ((const char *) InputPrecision) );
		  if ( fixIn.invalid() )
		    Error::abortRun( *this, "Invalid InputPrecision" );
		}
		fixIn.set_rounding( int(RoundFix) );

	        product = Fix( ((const char *) OutputPrecision) );
		if ( product.invalid() )
		  Error::abortRun( *this, "Invalid OutputPrecision" );
	        product.set_ovflow( ((const char *) OverflowHandler) );
		if ( product.invalid() )
		  Error::abortRun( *this, "Invalid OverflowHandler" );
		product.set_rounding( int(RoundFix) );
	}
	go {
	    // We use a temporary variable to avoid gcc2.7.2/2.8 problems
	    Fix tmpproduct = corona.input1%0;
	    fixIn = tmpproduct;
	    product = fixIn;
	    checkOverflow(product);
	    Fix tmpproduct2 = corona.input2%0;	
	    if ( int(ArrivingPrecision) )
		product *= tmpproduct2;
	    else {
		fixIn = tmpproduct2;
		product *= fixIn;
	    }
	    checkOverflow(product);
	    corona.output%0 << product;
	}
}	   
