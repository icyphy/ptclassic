defstar {
	name { GainFix }
	domain { CGC }
	derivedFrom { CGCFix }
	desc { 
This is an amplifier; the fixed-point output is the fixed-point input
multiplied by the "gain" (default 1.0).
The precision of "gain", the input, and the output can be specified in bits.
	}
	author { J.Weiss }
	copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	version { $Id$ }
	location { CGC main library }
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
                type { precision }
                default { 2.14 }
                desc {
Precision of the input in bits.  The input particles are only cast
to this precision if the parameter "ArrivingPrecision" is set to NO.}
        }
        defstate {
                name { OutputPrecision }
                type { precision }
                default { 2.14 }
                desc {
Precision of the output in bits.
This is the precision that will hold the result of the arithmetic operation
on the inputs.
When the value of the product extends outside of the precision,
the output is set to its maximum value (or minimum for negative magnitudes).
		}
        }
	constructor {
		noInternalState();
	}

        setup {
		CGCFix::setup();

                if (!int(ArrivingPrecision))
		    input.setPrecision(InputPrecision);
		output.setPrecision(OutputPrecision);
        }

	go {
		// insert code to clear overflow flag
		CGCFix::clearOverflow();

@	FIX_Mul($ref(output),$ref(input),$ref(gain));

		// insert code to test overflow flag
		CGCFix::checkOverflow();
	}

        // a wrap-up method is inherited from CGCFix
        // if you defined your own, you should call CGCFix::wrapup()
}
