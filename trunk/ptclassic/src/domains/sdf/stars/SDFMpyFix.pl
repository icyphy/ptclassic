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
		int in_IntBits, in_len;
	}
	setup {
	        const char* IP = InputPrecision;
	        in_IntBits = Fix::get_intBits(IP);
	        in_len = Fix::get_length(IP);

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

		p = nexti++;
		product = Fix((*p)%0);
	        while ((p = nexti++) != 0) {
		  if ( int(ArrivingPrecision) )
	            fixIn = Fix((*p)%0);
		  else
	            fixIn = Fix(in_len, in_IntBits, Fix((*p)%0));
		  product *= fixIn;
		}
	        output%0 << product;
	}
}
