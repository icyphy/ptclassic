defstar {
	name { AddFix }	
	domain {CGC}
	derivedFrom{ CGCFix }
	desc {
Output the sum of the fixed-point inputs as a fixed-point value.
	}
	version { $Id$ }
        author { J.Weiss }
	copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
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
parameter "ArrivingPrecision" is set to NO.
		}
        }
        defstate {
                name { OutputPrecision }
                type { precision }
                default { 2.14 }
                desc {
Precision of the output in bits and precision of the accumulation.
When the value of the accumulation extends outside of the precision,
the output is set to its maximum value (or minimum for negative
magnitudes).
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
@	FIX_Add($ref(output), $ref(input#1),$ref(input#2));

		else {
			// initialize sum
@	FIX_Assign($ref(output),$ref(input#1));

			for (int i=2; i <= input.numberPorts(); i++) {
			    index = i;
@	FIX_Add($ref(output), $ref(output),$ref(input#index));
			}
		}

		// insert code to test overflow flag
		CGCFix::checkOverflow();
	}

	// a wrap-up method is inherited from CGCFix
	// if you defined your own, you should call CGCFix::wrapup()
}
