defstar {
	name { RectFix }
	domain { SDF }
	derivedFrom { SDFFix }
	desc {
Generate a fixed-point rectangular pulse of height "height" 
(default 1.0) and width "width" (default 8).
If "period" is greater than zero, then the pulse is repeated with the
given period.
	}
        author { A. Khazeni }
	copyright {
Copyright (c) 1990-1994 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	version { $Id$ }
	location { SDF main library }
        explanation {
The "OutputPrecision" is specified using an l.r format, where
l is the number of bits to the left of the decimal place
(including the sign bit) and r is the number of bits to the
right of the sign bit.
For example, the precision "2.22" would represent a 24-bit fixed-point
number with 1 sign bit, 1 integer bit, and 22 fractional bits.
	}
	output {
		name { output }
		type { fix }
	}
	defstate {
		name { height }
		type { fix }
		default { "1.0" }
                desc { Height of the rectangular pulse. }
	}
	defstate {
		name { width }
		type { int }
		default { 8 }
		desc { Width of the rectangular pulse. }
	}
	defstate {
		name { period }
		type { int }
		default { 0 }
		desc { If greater than zero, the period of the pulse stream. }
	}
	defstate {
		name { count }
		type { int }
		default { 0 }
		desc { Internal counting state. }
		attributes { A_NONSETTABLE|A_NONCONSTANT }
	}
        defstate {
                name { OutputPrecision }
                type { precision }
                default { 2.14 }
                desc {
Precision of the output in bits.
The value of the "height" parameter is cast to this precision and then output.
If the value cannot be represented by this precision, then
the OverflowHandler will be called.
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
		out = 0.0;
		if ( int(count) < int(width) ) {
		  out = Fix(height);
		  checkOverflow(out);
		}
		output%0 << out;
		count = int(count) + 1;
		if ( int(period) > 0 && int(count) >= int(period) ) count = 0;
	}
        // a wrap-up method is inherited from SDFFix
        // if you defined your own, you should call SDFFix::wrapup()
}
