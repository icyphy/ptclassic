defstar {
	name { Tone }
	domain { CG56 }
	desc { Sine or cosine function using second order oscillator }
	version { $Id$ }
	author { ported from Gabriel by J. Buck }
	copyright { 1992 The Regents of the University of California }
	location { CG56 demo library }
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
	start {
		// maximum fixed point value
		double twoPiF = 2.0 * M_PI * double(frequency);
		X = cos(twoPiF);
		if (double(X) > double(ONE)) X = double(ONE);
		const char* p = calcType;
		switch (*p) {
		case 's':	// sine
			state1 = 0.0;
			state2 = double(amplitude) * sin(twoPiF);
			break;
		case 'c':	// cosine
			state1 = double(amplitude);
			state2 = double(amplitude) * cos(twoPiF);
			break;
		default:
			Error::abortRun(*this, "calcType must be sin or cos");
			return;
		}
	}
	go { gencode(std); }
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
