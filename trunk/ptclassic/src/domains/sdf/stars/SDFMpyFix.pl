defstar {
	name { MpyFix }
	domain { SDF }
	desc { Output the product of the inputs, as a fixed-point value. }
	author { A. Khazeni }
	copyright {
Copyright (c) 1993, 1994 The Regents of the University of California.
All rights reserved.
See the file ~ptolemy/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	version { $Id$ }
	location { SDF main library }
	inmulti {
		name { input }
		type { fix }
	}
	output {
		name { output }
		type { fix }
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
	        type { string }
	        default { "4.14" }
	        desc {
Sets the precision of the input in bits.
The input particles are only cast to this precision if the
parameter "ArrivingPrecision" is set to NO.
		}
	}
	defstate {
	        name { OutputPrecision }
	        type { string }
	        default { "4.14" }
	        desc {
Sets the precision of the output in bits.
This is the precision that will hold the result of the product of the inputs.
When the value of the product extends outside of the precision,
the OverflowHandler will be called.
		}
	}
	defstate {
	        name { OverflowHandler }
	        type { string }
	        default { "saturate" }
	        desc {
Overflow characteristic for the output.
If the result of the sum cannot be fit into the precision of the output,
then overflow occurs and the overflow is taken care of by the method
specified by this parameter.
The keywords for overflow handling methods are:
"saturate" (the default), "zero_saturate", "wrapped", and "warning".
The "warning" option will generate a warning message whenever overflow occurs.
		}
	}
	protected {
		Fix fixIn, product;
	}
	setup {
		if ( ! int(ArrivingPrecision) )
		  fixIn = Fix( ((const char *) InputPrecision) );

	        product = Fix( ((const char *) OutputPrecision) );
	        product.set_ovflow( ((const char *) OverflowHandler) );
	}
	go {
	        MPHIter nexti(input);
	        PortHole *p;

		// initialize product as the value on the first input port
		// (if it exists) and then compute the product
		p = nexti++;
		if ( p != 0 ) {
	          fixIn = Fix((*p)%0);
	          product = fixIn;
	          while ((p = nexti++) != 0) {
		    if ( int(ArrivingPrecision) )
		      product *= Fix((*p)%0);
		    else {
	              fixIn = Fix((*p)%0);
		      product *= fixIn;
		    }
		  }
		}
		else {			// 1.0 will overflow if the fixed-
		  product = 1.0;	// representation has 0 integer bits
		}
	        output%0 << product;
	}
}
