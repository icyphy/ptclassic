defstar {
	name { MpyFix }
	domain { SDF }
	derivedFrom { SDFFix }
	desc { Output the product of the inputs, as a fixed-point value. }
	author { A. Khazeni }
	copyright {
Copyright (c) 1990-1994 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
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
	        type { precision }
	        default { 2.14 }
	        desc {
Sets the precision of the input in bits.
The input particles are only cast to this precision if the
parameter "ArrivingPrecision" is set to NO.
		}
	}
	defstate {
	        name { OutputPrecision }
	        type { precision }
	        default { 2.14 }
	        desc {
Sets the precision of the output in bits.
This is the precision that will hold the result of the product of the inputs.
When the value of the product extends outside of the precision,
the OverflowHandler will be called.
		}
	}
	protected {
		Fix fixIn, product;
	}
	setup {
		SDFFix::setup();

		if ( ! int(ArrivingPrecision) ) {
		  fixIn = Fix( ((const char *) InputPrecision) );
		  if ( fixIn.invalid() )
		    Error::abortRun( *this, "Invalid InputPrecision" );
		}
		fixIn.set_rounding( ((int) RoundFix) );

	        product = Fix( ((const char *) OutputPrecision) );
		if ( product.invalid() )
		  Error::abortRun( *this, "Invalid OutputPrecision" );
	        product.set_ovflow( ((const char *) OverflowHandler) );
		if ( product.invalid() )
		  Error::abortRun( *this, "Invalid OverflowHandler" );
		product.set_rounding( ((int) RoundFix) );
	}
	go {
	        MPHIter nexti(input);
	        PortHole *p;

		// initialize product as the value on the first input port
		// (if it exists) and then compute the product
		p = nexti++;
		if ( p != 0 ) {
	          fixIn = (Fix)((*p)%0);
	          product = fixIn;
		  checkOverflow(product);
	          while ((p = nexti++) != 0) {
		    if ( int(ArrivingPrecision) )
		      product *= (Fix)((*p)%0);
		    else {
	              fixIn = (Fix)((*p)%0);
		      product *= fixIn;
		    }
		    checkOverflow(product);
		  }
		}
		else {
		  product = 1.0;	   // 1.0 will overflow if the fixed-
		  checkOverflow(product);  // representation has 0 integer bits
		}
	        output%0 << product;
	}
        // a wrap-up method is inherited from SDFFix
        // if you defined your own, you should call SDFFix::wrapup()
}
