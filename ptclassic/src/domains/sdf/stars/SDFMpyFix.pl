defstar {
	name { MpyFix }
	domain { SDF }
	desc { Output the product of the inputs, as a fixed-point value. }
	author { A. Khazeni }
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
		}
	}
	protected {
		Fix fixIn, product;
	}
	setup {
		if ( ! int(ArrivingPrecision) ) {
	          const char* IP = InputPrecision;
	          int in_IntBits = Fix::get_intBits(IP);
	          int in_len = Fix::get_length(IP);
		  fixIn = Fix(in_len, in_IntBits);
		}

	        const char* OP = OutputPrecision;
	        int out_IntBits = Fix::get_intBits(OP);
	        int out_len = Fix::get_length(OP);
	        product = Fix(out_len, out_IntBits);

	        const char* OV = OverflowHandler;
	        product.set_ovflow(OV);
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
