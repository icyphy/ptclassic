defstar {
	name { CxToFix }
	domain { CGC }
	derivedFrom { CGCFix }
	version { $Id$ }
	author { J.Weiss }
	copyright {
Copyright (c) 1990-1994 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { CGC conversion palette }
	desc { Convert a complex input to a fixed-point output. }
	explanation {
This star converts a complex value to a fix value with the specified precision
by computing the absolute value.
If the output precision is not specified, the precision is determined at
runtime according to the incoming float value.
	}
	input {
		name { input }
		type { complex }
		desc { Input complex type }
	}
	output {
		name { output }
		type { fix }
		desc { Output fix type }
	}
	state {
		name { numSample }
		type { int }
		default { "1" }
		desc { number of samples to be copied }
	}
	defstate {
		name { OutputPrecision }
		type { precision }
		default { "" }
		desc {
Precision of the output in bits or empty if unspecified.
The absolute value of the complex number is computed and then converted to
this precision.
If the converted value cannot be represented by the number of bits specified
in the precision parameter, then the output is set to its maximum value (or
minimum for negative magnitudes).
		}
	}

	setup {
		CGCFix::setup();

		if (int(numSample) > 1) {
			input.setSDFParams(int(numSample));
			output.setSDFParams(int(numSample));
		}
		output.setPrecision(OutputPrecision);
	}

	begin {
		// if the precision for the output port is not defined
		// - neither by this nor the successor star -, the actual
		// precision is determined at runtime

		if (!output.precision().isValid())
			output.setAttributes(A_VARPREC);
	}

	initCode {
		numSample = output.numXfer();
		addInclude("<math.h>");
	}

	go {
		// insert code to clear overflow flag
		CGCFix::clearOverflow();

@     {	int i;
@	double p,q;
@	for (i = 0; i < $val(numSample); i++) {
@		p = $ref(input, i).real;
@		q = $ref(input, i).imag;
@		p = sqrt(p*p+q*q);

	    if (output.attributes() & AB_VARPREC)
@		FIX_SetPrecisionFromDouble($precision(output),p);

@		FIX_DoubleAssign($ref(output,i),p);
@     } }

		// insert code to test overflow flag
		CGCFix::checkOverflow();
	}

        // a wrap-up method is inherited from CGCFix
        // if you defined your own, you should call CGCFix::wrapup()
}
