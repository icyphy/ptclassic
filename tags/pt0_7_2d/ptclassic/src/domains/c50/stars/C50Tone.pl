defstar {
	name { Tone }
	domain { C50 }
	desc { Sine or cosine function using second order oscillator } 
	version { @(#)C50Tone.pl	1.4	8/13/96 }
	author { A. Baensch, Luis Gutierrez, ported from Gabriel }
	acknowledge { Method from Motorola's "DSP News", Vol. 1., No. 3, 1988 }
	copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { C50 main library }
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
		attributes { A_NONCONSTANT|A_NONSETTABLE|A_UMEM }
	}
	state {
		name { state2 }
		type { FIX }
		desc { internal }
		default { 0 }
		attributes { A_NONCONSTANT|A_NONSETTABLE|A_UMEM }
	}

	protected{
		int	X;
	}

	setup {
		// maximum fixed point value
		double twoPiF = 2.0 * M_PI * frequency.asDouble();
		double temp  = cos(twoPiF);
		if (temp >= 0) {
			X = int(double(32768)*temp + 0.5);
		} else {
			X = int(double(32768)*(2.0+temp)+0.5);
		}
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
	
	codeblock (std,"") {
	lar	AR1,#$addr(state1)		;Address state1 =>AR1
	lar	AR2,#$addr(state2)		;Address state2 =>AR2
	lar	AR7,#$addr(output)		;Address output =>AR7
	mar	*,AR2
	lt	*,AR1				;val(state2) => TREG0
	mpy	#@(int(X))			;val(state2)*val(X)
	pac					;Accu=state2*X
	apac					;Accu=2*state2*X
	sub	*,15,AR7			;Accu=2*state2*X - state1
	bldd	#$addr(state2),*,AR2		;val(state2) => output
	bldd	*,#$addr(state1)		;val(state2) => addr(state1)
	sach	*,1				;Accu => addr(state2)
	}

	go {
		addCode(std());
	}

	execTime {
		return 14;
	}
}
