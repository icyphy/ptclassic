defstar {
	name { FloatToFix }
	domain { CGC }
	derivedFrom { CGCFix }
	version { $Id$ }
	author { J.Weiss }
	copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { CGC conversion palette }
	desc { Convert a floating-point input to an fixed-point output. }
	explanation {
This star converts a float value to a fix value with the specified precision.
If the output precision is not specified, the precision is determined at
runtime according to the incoming float value.
	}
	input {
		name { input }
		type { float }
		desc { Input float type }
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
If the value of the double cannot be represented by the number of bits
specified in the precision parameter, then the output is set to its maximum
value (or minimum for negative magnitudes).
		}
        }

        setup {
		CGCFix::setup();
		output.setPrecision(OutputPrecision);
        }

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

		if (output.attributes() & AB_VARPREC)
@	FIX_SetPrecisionFromDouble($precision(output),$ref(input));

@	FIX_DoubleAssign($ref(output),$ref(input));

		// insert code to test overflow flag
		CGCFix::checkOverflow();
	}

	// a wrap-up method is inherited from CGCFix
	// if you defined your own, you should call CGCFix::wrapup()
}
