defstar {
	name { RectFix }
	domain { SDF }
	desc {
Generate a fixed-point rectangular pulse of height "height" 
(default 1.0) and width "width" (default 8).
If "period" is greater than zero, then the pulse is repeated with the
given period.
	}
        author { A. Khazeni }
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
                type { string }
                default { "4.14" }
                desc {
Precision of the output in bits.
The value of the "height" parameter is cast to this precision and then output.
If the value cannot be represented by this precision, then
the OverflowHandler will be called.
                }
        }
        defstate {
                name { OverflowHandler }
                type { string }
                default { "saturate" }
                desc {
Overflow characteristic for the output.
If the result of the sum cannot be fit into the precision of the output,
then overflow occurs and the overflow is taken care of by the method
specified by this parameter.
The keywords for overflow handling methods are:
"saturate" (the default), "zero_saturate", "wrapped", and "warning".
The "warning" option will generate a warning message whenever overflow occurs.
		}
        }
	protected {
		Fix out;
	}
        setup {
                out = Fix( ((const char *) OutputPrecision) );
		out.set_ovflow( ((const char *) OverflowHandler) );
        }
	go {
		out = 0.0;
		if ( int(count) < int(width) ) out = Fix(height);
		output%0 << out;
		count = int(count) + 1;
		if ( int(period) > 0 && int(count) >= int(period) ) count = 0;
	}
}
