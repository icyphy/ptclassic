defstar {
	name { ConstFix }
	domain { SDF }
	derivedFrom { SDFFix }
	desc {
Output a fixed-point constant output with value "level" (default 0.0).
	}
        author { E. A. Lee }
	copyright {
Copyright (c) 1990-1995 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	version { $Id$ }
	location { SDF main library }
        explanation {
The "OutputPrecision" is specified using an "l.r" format, where
"l" is the number of bits to the left of the decimal place
(including the sign bit) and "r" is the number of bits to the
right of the sign bit.
For example, the precision "2.22" would represent a 24-bit fixed-point
number with 1 sign bit, 1 integer bit, and 22 fractional bits.
	}
	output {
		name { output }
		type { fix }
	}
	defstate {
		name { level }
		type { fix }
		default { "0.0" }
                desc { The constant value. }
	}
        defstate {
                name { OutputPrecision }
                type { string }
                default { "2.14" }
                desc { Precision of the output in bits. }
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
		out = 0.0;
		out = Fix(level);
		output%0 << out;
	}
        // a wrap-up method is inherited from SDFFix
        // if you defined your own, you should call SDFFix::wrapup()
}
