defstar {
	name { RectFix }
	domain { SDF }
	desc {
Generate a fixed-point rectangular pulse of height "height" 
(default 1.0) and width "width" (default 8).
If "period" is greater than zero, then the pulse is repeated with the
given period.
The precision of "height" is specified by the precision of "outputPrecision".
	}
        author { A. Khazeni }
	version { $Id$ }
	location { SDF main library }
        explanation {
The "outputPrecision" is specified using an l.r format, where
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
		name { height }
		type { float }
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
		out = 0.0;
		if ( int(count) < int(width) ) out = height;
		output%0 << out;
		count = int(count) + 1;
		if ( int(period) > 0 && int(count) >= int(period) ) count = 0;
	}
}
