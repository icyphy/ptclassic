defstar {
	name { IntToFix }
	domain { CGC }
	derivedFrom { Fix }
	version { $Id$ }
	author { J.Weiss }
	copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { CGC main library }
	desc { Convert a floating-point input to an fixed-point output. }
	htmldoc {
This star converts a int value to a fix value with the specified precision.
If the output precision is not specified, the precision is determined at
runtime according to the incoming int value.
	}
	input {
		name { input }
		type { int }
		desc { Input int type }
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

	// an initCode method is inherited from CGCFix
	// if you define your own, you should call CGCFix::initCode()

	begin {
		// if the precision for the output port is not defined
		// - neither by this nor the successor star -, the actual
		// precision is determined at runtime

		if (!output.precision().isValid())
			output.setAttributes(A_VARPREC);
	}

	codeblock(setprec) {
	        FIX_SetPrecisionFromDouble($precision(output),(double)((int)$ref(input)));
	}
	codeblock(assign) {
	        FIX_DoubleAssign($ref(output),(double)((int)$ref(input)));
	}
	go {
		// insert code to clear overflow flag
		CGCFix::clearOverflow();

		if (output.attributes() & AB_VARPREC)
		     addCode(setprec);

		addCode(assign);

		// insert code to test overflow flag
		CGCFix::checkOverflow();
	}

	// a wrapup method is inherited from CGCFix
	// if you define your own, you should call CGCFix::wrapup()
}
