defstar {
	name { Intgrtr }
	domain { C50 }
	desc { 
An integrator with leakage and limits.  With the default parameters,
input samples are simply accumulated, and the running sum is the
output.  If there is an overflow, the integrator will wrap around.
	}
	version { @(#)C50Intgrtr.pl	1.4	03 Oct 1996 }
	author { A. Baensch, ported from Gabriel }
	copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { C50 main library }
	htmldoc {
<a name="filter, integrator"></a>
<p>
Leakage is controlled by the </i>feedbackGain</i> state (default 1.0).
The output is the data input plus feedbackGain*state, where state is the
previous output.
<p>
Overflow is controlled by the <i>onOverflow</i> state (default wrap around.
The integrator can either be set to wrap around (default), saturate or
reset.
	}
	input {
		name { input }
		type { FIX }
 	}
	output {
		name { output }
		type { FIX }
	}
	state {
		name {feedbackGain}
		type {FIX}
		default {ONE}
		desc { The gain on the feedback path.}
		attributes { A_UMEM }
	}
	state {
		name {unityGainB}
		type {INT}
		default {0}
		desc { "Boolean: is the feedbackGain unity?" }
		attributes { A_NONSETTABLE|A_NONCONSTANT }
	}
 	state {
		name {onOverflow}
		type {string}
		default {"wrap around"}
		desc { On overflow, either wrap around or reset.}
	}
	state {
		name {initialValue}
		type {FIX}
		default {0}
		desc { Value at time zero and also reset value if used. }
		attributes { A_UMEM }
	}
	state {
		name {sum}
		type {FIX}
		default {0}
		desc { An internal state.}
		attributes { A_NONSETTABLE|A_UMEM|A_NONCONSTANT }
	}

 
    codeblock (cbSetup) {
	mar	*,AR5
	lar	AR0,#$addr(feedbackGain) ;Address feedbackGain	=> AR0
	lar	AR1,#$addr(initialValue) ;Address initialValue	=> AR1
	lar 	AR5,#$addr(input)	; Address input		=> AR5
	lar	AR7,#$addr(output)	; Address output	=> AR7
	lar	AR6,#$addr(sum)		; Address sum		=> AR6
	zap				; clear P-Reg and Accu
	lacc	*,15,AR0		; Accu = input
    }
    codeblock(cbCoreReset) {
	.if	$val(unityGainB)
	  mar	*,AR6			;
	  add	*,15,AR1		; Accu = sum + input
	.else
	  lt	*,AR6			; TREG0 = feedbackGain
	  mpy	*,AR1			; feedbackGain * sum
	  apac				; Accu = feedbackGain * sum
	.endif
	nop				;
	xc 	2,LT			; if Accu <= 0 then
 	 lacc	*,15			; Accu = initialValue
	 nop				;
	mar	*,AR6			;
	sach	*,1,AR7			; Accu => sum
	sach	*,1			; Accu => output
    }

    codeblock(cbCoreWrap) {
	.if	$val(unityGainB)
	  mar	*,AR6			;
	  add	*,15			; Accu = sum + input	
	.else
	  lt	*,AR6			; TREG0 = feedbackGain
	  mpy	*			; feedbackGain * sum
	  apac				; Accu = feedbackGain * sum
	.endif
	sach	*,1,AR7			; Accu => sum
	sach	*,1			; Accu => output
    }

    setup {
	sum = initialValue;
	unityGainB = (feedbackGain.asDouble() >= C50_ONE);
    }
    go {
	addCode(cbSetup);
	const char *p = onOverflow;
	switch (p[0]) {
	  case('r'):		//Reset on overflow
		addCode(cbCoreReset);
		break;
	  case('w'):		//Wrap around on overflow
		addCode(cbCoreWrap);
		break;
	  default:
		Error::abortRun(*this, "Unrecognized onOverflow option.");
		break;
	}
    }
    execTime {
	const char *p = onOverflow;
	if (p[0]=='r') return 17;
	else return 13;
    }
}
