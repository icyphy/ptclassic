defstar {
	name { GainFix }
	domain { SDF }
	desc { 
This is an amplifier; the fixed-point output is the fixed-point input
multiplied by the "gain" (default 1.0).
The precision of "gain", the input, and the output can be specified in bits.
	}
	author { A. Khazeni }
	version { $Id$ }
	location { SDF main library }
        explanation { 
The value of the "gain" parameter and its precision in bits can currently 
be specified using two different notations.
Specifying only a value by itself in the dialog box would create a
fixed-point number with the default precision which has a total length
of 24 bits with the number of range bits as required by the value.
For example, the default value 1.0 creates a fixed-point object with
precision 2.22, and a value like 0.5 would create one with precision
1.23.  An alternative way of specifying the value and the
precision of this parameter is to use the parenthesis notation
of (value, precision).  For example, filling the dialog
box for the gain parameter with (2.546, 3.5) would create a fixed-point
object formed by casting the double-precision floating-point number
2.546 to a fixed-point number with a precision of 3.5.
	}
	input {
		name { input }
		type { fix }
	}
	output {
		name { output }
		type { fix }
	}
	defstate {
		name { gain }
		type { fix }
		default { 1.0 }
		desc { Gain of the star. }
	}
        defstate {
                name { ArrivingPrecision }
                type {int}
                default {"YES"}
                desc {
Flag indicated whether or not to use the arriving particles as they are:
YES keeps the same precision, and NO casts them to the
precision specified by the parameter "InputPrecision". }
        }
        defstate {
                name { InputPrecision }
                type { string }
                default { "4.14" }
                desc {
Precision of the input in bits.  The input particles are only cast
to this precision if the parameter "ArrivingPrecision" is set to NO.}
        }
        defstate {
                name { OutputPrecision }
                type { string }
                default { "4.14" }
                desc {
Precision of the output in bits.
This is the precision that will hold the result of the arithmetic operation
on the inputs.
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
		Fix fixIn, out;
        }
        setup {
                if ( ! int(ArrivingPrecision) )
                  fixIn = Fix( ((const char *) InputPrecision) );

                out = Fix( ((const char *) OutputPrecision) );
                out.set_ovflow( ((const char *) OverflowHandler) );
        }
	go {
		// all computations should be performed with out since that
		// is the Fix variable with the desired overflow handler
		out = Fix(gain);
		if ( int(ArrivingPrecision) ) {
                  out *= Fix(input%0);
		}
		else {
                  fixIn = Fix(input%0);
                  out *= fixIn;
		}
		output%0 << out;
	}
}
