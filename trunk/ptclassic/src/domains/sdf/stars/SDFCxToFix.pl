defstar {
	name { CxToFix }
	domain { SDF }
	version { $Id$ }
	author { Mike J. Chen }
	copyright {
Copyright (c) 1990-1993 The Regents of the University of California.
All rights reserved.
See the file ~ptolemy/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { SDF conversion palette }
	desc { Convert a complex input to a fixed-point output. }
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
	defstate {
		name { outputPrecision }
		type { string }
		default { "4.14" }
		desc {
Precision of the output, in bits.
The complex number is cast to a double and then converted to this precision.
If the value of the double cannot be represented by the number of bits
specified in the precision parameter, then a error message is given.
		}
	}
	defstate {
		name { masking }
		type { string }
		default { "truncate" }
		desc {
Masking method.
This parameter is used to specify the way the complex number converted to
a double is masked for casting to the fixed-point notation.
The keywords are: "truncate" (the default) and "round".
		}
	}
	protected {
		Fix out;
	}
	setup {
		const char* Masking = masking;
		const char* OutputPrecision = outputPrecision;
		int outIntBits = Fix::get_intBits(OutputPrecision);
		int outLen = Fix::get_length(OutputPrecision);
		out = Fix(outLen, outIntBits);
		if ( strcasecmp(Masking, "truncate") == 0)
		  out.Set_MASK(Fix::mask_truncate);
		else if ( strcasecmp(Masking, "round") == 0)
		  out.Set_MASK(Fix::mask_truncate_round);
		else
		  Error::abortRun(*this, ": not a valid function for masking");
	}
	go {
		out = (double) (input%0);
		output%0 << out;
	}
}
