defstar {
	name { ConstFix }
	domain { CGC }
	derivedFrom { Fix }
	desc {
Output a fixed-point constant output with value "level" (default 0.0).
	}
        author { Y. K. Lim }
	copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	version { @(#)CGCConstFix.pl	1.2 01 Oct 1996 }
	location { CGC main library }
	htmldoc {
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
                type { precision }
                default { 2.14 }
                desc { Precision of the output in bits. }
        }
        setup {
		CGCFix::setup();

		output.setPrecision(OutputPrecision);
	}

        // an initCode method is inherited from CGCFix
	// if you define your own, you should call CGCFix::initCode()

	go {
	        CGCFix::clearOverflow();
@               FIX_Assign($ref(output), $ref(level));
                CGCFix::checkOverflow();
	}
        // a wrapup method is inherited from CGCFix
        // if you define your own, you should call CGCFix::wrapup()
}

