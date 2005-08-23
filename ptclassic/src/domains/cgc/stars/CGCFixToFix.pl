defstar {
	name { FixToFix }
	domain { CGC }
	derivedFrom { Fix }
	version { @(#)CGCFixToFix.pl	1.7 8/2/96 }
	author { J.Weiss }
	copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { CGC main library }
	desc {
Convert a fixed-point value into one with a different precision.
	}
	input {
		name { input }
		type { fix }
		desc { Input fix type }
	}
	output {
		name { output }
		type { fix }
		desc { Output fix type }
	}
        defstate {
                name { OutputPrecision }
                type { precision }
                default { "" }
                desc {
Precision of the output in bits or empty if unspecified.
If the input value cannot be represented by the number of bits specified in
the precision parameter, then the output is set to its maximum value (or
minimum for negative magnitudes).
		}
        }

        setup {
		CGCFix::setup();
		output.setPrecision(OutputPrecision);
        }

	// an initCode method is inherited from CGCFix
	// if you define your own, you should call CGCFix::initCode()

	begin {
		// if the precision for the output port is not defined
		// - neither by this nor the successor star -, the actual
		// precision is determined at runtime

		if (!output.precision().isValid())
			output.setAttributes(A_VARPREC);
	}

	go {
		// insert code to clear overflow flag
		CGCFix::clearOverflow();

		// do the assignment
@	FIX_Assign($ref(output),$ref(input));

		// insert code to test overflow flag
		CGCFix::checkOverflow();
	}

	// a wrap-up method is inherited from CGCFix
	// if you define your own, you should call CGCFix::wrapup()
}
