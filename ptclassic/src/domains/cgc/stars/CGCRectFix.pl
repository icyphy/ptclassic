defstar {
	name { RectFix }
	domain { CGC }
	derivedFrom { CGCFix }
	desc {
Generate a fixed-point rectangular pulse of height "height" 
(default 1.0) and width "width" (default 8).
If "period" is greater than zero, then the pulse is repeated with the
given period.
	}
        author { J.Weiss }
	copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	version { $Id$ }
	location { CGC main library }

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
		attributes { A_CONSTANT }
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
If the value cannot be represented by this precision,
the output is set to its maximum value (or minimum for negative magnitudes).
                }
        }
        setup {
		CGCFix::setup();
		// if the user specified an invalid precision string, the error
		// will be automatically reported in the initialize method of
		// class PrecisionState
		output.setPrecision(OutputPrecision);
        }
	go {
@	FIX_SetToZero($ref(output));

@	if ($ref(count) < $ref(width) {
		CGCFix::clearOverflow();
@		FIX_Assign($ref(output),$ref(height));
		CGCFix::checkOverflow();
@	}
@	$ref(count)++;
@	if ($ref(period) > 0 && $ref(count) > $ref(period))
@		$ref(count) = 0;
	}

        // a wrap-up method is inherited from CGCFix
        // if you defined your own, you should call CGCFix::wrapup()
}
