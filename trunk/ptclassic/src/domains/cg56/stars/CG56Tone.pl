defstar {
	name { Tone }
	domain { CG56 }
	acknowledge { Method from Motorola's "DSP News", Vol. 1., No. 3, 1988 }
	desc { Sine or cosine function using second order oscillator }
	version { $Id$ }
	author { J. Buck, ported from Gabriel }
	copyright {
Copyright (c) 1990, 1991, 1992 The Regents of the University of California.
All rights reserved.
See the file ~ptolemy/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { CG56 signal sources library }
	output {
		name { output }
		type { fix }
	}
	state {
		name { amplitude }
		type { fix }
		desc { amplitude of sine/cosine wave }
		default { .5 }
	}
	state {
		name { frequency }
		type { fix }
		desc { frequency, relative to the sampling frequency }
		default { .02 }
	}
	state {
		name { calcType }
		type { string }
		desc { calculation type (sin or cos) }
		default { "sin" }
	}
	state {
		name { state1 }
		type { FIX }
		desc { internal }
		default { 0 }
		attributes { A_NONCONSTANT|A_NONSETTABLE|A_YMEM }
	}
	state {
		name { state2 }
		type { FIX }
		desc { internal }
		default { 0 }
		attributes { A_NONCONSTANT|A_NONSETTABLE|A_XMEM }
	}
	state {
		name { X }
		type { FIX }
		desc { internal }
		default { 0 }
		attributes { A_NONCONSTANT|A_NONSETTABLE}
	}
	setup {
		// maximum fixed point value
		double twoPiF = 2.0 * M_PI * frequency.asDouble();
		X = cos(twoPiF);
		const char* p = calcType;
		switch (*p) {
		case 's':	// sine
			state1 = 0.0;
			state2 = amplitude.asDouble() * sin(twoPiF);
			break;
		case 'c':	// cosine
			state1 = amplitude;
			state2 = amplitude.asDouble() * cos(twoPiF);
			break;
		default:
			Error::abortRun(*this, "calcType must be sin or cos");
			return;
		}
	}
	go { addCode(std); }
	execTime { return 7;}
	codeblock (std) {
	move	$ref(state2),x1
	move	$ref(state1),a
	move	#$val(X),x0
	mac	-x1,x0,a	x1,$ref(output)
	neg	a
	mac	x1,x0,a		x1,$ref(state1)
	move	a,$ref(state2)
	}
}
