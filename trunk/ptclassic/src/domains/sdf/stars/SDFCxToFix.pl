defstar {
	name { CxToFix }
	domain { SDF }
	derivedFrom { SDFFix }
	version { $Id$ }
	author { Mike J. Chen }
	copyright {
Copyright (c) 1990-1994 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
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
		name { OutputPrecision }
		type { string }
		default { "2.14" }
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
	begin {
		SDFFix::begin();

		out = Fix( ((const char *) OutputPrecision) );
		if ( out.invalid() )
		  Error::abortRun( *this, "Invalid OutputPrecision" );
		out.set_ovflow( ((const char *) OverflowHandler) );
		if ( out.invalid() )
		  Error::abortRun( *this, "Invalid OverflowHandler" );

		const char* Masking = masking;
		if ( strcasecmp(Masking, "truncate") == 0 )
		  out.Set_MASK(Fix::mask_truncate);
		else if ( strcasecmp(Masking, "round") == 0 )
		  out.Set_MASK(Fix::mask_truncate_round);
		else
		  Error::abortRun(*this, ": not a valid function for masking");
	}
	go {
		out = (double) (input%0);
		checkOverflow(out);
		output%0 << out;
	}
        // a wrap-up method is inherited from SDFFix
        // if you defined your own, you should call SDFFix::wrapup()
}
