defstar {
	name { IntToFix }
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
                type { precision }
                default { 16.0 }
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
		SDFFix::setup();

                out = Fix( ((const char *) OutputPrecision) );
		if ( out.invalid() )
		  Error::abortRun( *this, "Invalid OutputPrecision" );
		out.set_ovflow( ((const char *) OverflowHandler) );
		if ( out.invalid() )
		  Error::abortRun( *this, "Invalid OverflowHandler" );
		out.set_rounding( ((int) RoundFix) );
        }
	go {
		out = (double) ((int)(input%0));
		checkOverflow(out);
                output%0 << out;
	}
        // a wrap-up method is inherited from SDFFix
        // if you defined your own, you should call SDFFix::wrapup()
}
