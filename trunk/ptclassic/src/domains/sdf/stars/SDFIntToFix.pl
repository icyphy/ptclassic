defstar {
	name { IntToFix }
	domain { SDF }
	derivedFrom { SDFFix }
	version { $Id$ }
	author { Mike J. Chen }
	copyright {
Copyright (c) 1990-1993 The Regents of the University of California.
All rights reserved.
See the file ~ptolemy/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { SDF conversion palette }
	desc { Convert an integer input to a fixed-point output. }
	input {
		name { input }
		type { int }
		desc { Input integer type }
	}
	output {
		name { output }
		type { fix }
		desc { Output fix type }
	}
        defstate {
                name { OutputPrecision }
                type { string }
                default { "16.0" }
                desc {
Precision of the output in bits.
The integer number is cast to a double and then converted to this precision.
If the value of the double cannot be represented by the number of bits
specified in the precision parameter, then the OverflowHandler will be called.
		}
        }
        protected {
		Fix out;
        }
        setup {
                out = Fix( ((const char *) OutputPrecision) );
		out.set_ovflow( ((const char *) OverflowHandler) );
		if ( out.invalid() )
		  Error::abortRun( *this, "Invalid overflow handler" );
        }
	go {
		out = (double) ((int)(input%0));
		checkOverflow(out);
                output%0 << out;
	}
        // a wrap-up method is inherited from SDFFix
        // if you defined your own, you should call SDFFix::wrapup()
}
