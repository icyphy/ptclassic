defstar {
        name { SubFix }
        domain { CGC }
	derivedFrom { CGCFix }
        desc {
Output as a fixed-point number the "pos" input minus all "neg" inputs.
	}
        author { J.Weiss }
	copyright {
Copyright (c) 1990-1994 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	version { $Id$ }
        location { CGC main library }
        input {
                name { pos }
                type { fix }
        }
        inmulti {
                name { neg }
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
Flag indicating whether or not to use the precision of the arriving particles
(the "pos" input and the "neg" inputs) as they are:  YES processes them
unaltered, and NO casts them to the precision specified by the
parameter "InputPrecision".
		}
        }
        defstate {
                name { InputPrecision }
                type { precision }
                default { 2.14 }
                desc {
Precision of the "neg" input in bits.
The input particles are only cast to this precision if the
parameter "ArrivingPrecision" is set to NO.
		}
        }
        defstate {
                name { OutputPrecision }
                type { precision }
                default { 2.14 }
                desc {
Precision of the output in bits.
This is the precision that will hold the result of the difference
of the inputs.
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
                if (!int(ArrivingPrecision)) {
		    pos.setPrecision(InputPrecision);
		    neg.setPrecision(InputPrecision);
		}
		output.setPrecision(OutputPrecision);
        }

	go {
		// insert code to clear overflow flag
		CGCFix::clearOverflow();

		// avoid FIX_Assign if possible
		if (neg.numberPorts() == 1)
@	FIX_Sub($ref(output), $ref(pos),$ref(neg#1));

		else {
			// initialize sum
@	FIX_Assign($ref(output),$ref(pos));

			for (int i=1; i <= neg.numberPorts(); i++) {
			    index = i;
@	FIX_Sub($ref(output), $ref(output),$ref(neg#index));
			}
		}

		// insert code to test overflow flag
		CGCFix::checkOverflow();
	}

        // a wrap-up method is inherited from CGCFix
        // if you defined your own, you should call CGCFix::wrapup()
}
