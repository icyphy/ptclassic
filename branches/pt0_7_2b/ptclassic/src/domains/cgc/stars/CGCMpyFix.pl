defstar {
	name { MpyFix }
	domain { CGC }
	derivedFrom { CGCFix }
	desc { Output the product of the inputs, as a fixed-point value. }
	author { J.Weiss }
	copyright {
Copyright (c) 1990-1994 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	version { $Id$ }
	location { CGC main library }
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
the output is set to its maximum value (or minimum for negative magnitudes).
		}
	}

	state {
		name { index }
		type { int }
		default { 1 }
		desc { index for multiple input trace }
		attributes { A_NONSETTABLE|A_NONCONSTANT }
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

		// avoid FIX_Assign if possible
		if (input.numberPorts() == 2)
@	FIX_Mul($ref(output), $ref(input#1),$ref(input#2));

		else {
			// initialize the product
@	FIX_Assign($ref(output),$ref(input#1));

			for (int i=2; i <= input.numberPorts(); i++) {
			    index = i;
@	FIX_Mul($ref(output), $ref(output),$ref(input#index));
			}
		}

		// insert code to test overflow flag
		CGCFix::checkOverflow();
	}

        // a wrap-up method is inherited from CGCFix
        // if you defined your own, you should call CGCFix::wrapup()
}
