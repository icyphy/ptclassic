defstar {
	name { Integrator }
	domain { CG56 }
	desc { 
An integrator with leakage and limits.  With the default parameters,
input samples are simply accumulated, and the running sum is the
output.  If there is an overflow, the integrator will wrap around.
	}
	version { $Id$ }
	author { Kennard, J. Pino (ported from Gabriel) }
	copyright {
Copyright (c) 1990, 1991, 1992 The Regents of the University of California.
All rights reserved.
See the file ~ptolemy/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { CG56 nonlinear functions library }
	explanation {
.Id "filter, integrator"
.pp
Leakage is controlled by the \fPfeedbackGain\fP state (default 1.0).
The output is the data input plus feedbackGain*state, where state is the
previous output.
.pp
Overflow is controlled by the \fIonOverflow\fP state (default wrap around.
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
		desc { On overflow, either wrap around, saturate or reset.}
	}
	state {
		name {initialValue}
		type {FIX}
		default {0}
		desc { Value at time zero and also reset value if used. }
	}
	state {
		name {sum}
		type {FIX}
		default {0}
		desc { An internal state.}
		attributes { A_NONSETTABLE|A_YMEM|A_NONCONSTANT }
	}



    codeblock (cbSetup) {
	move	$ref(input),a
	move	$ref(sum),y0
    }
    codeblock(cbCoreReset) {
	IF	$val(unityGainB)
	  add	y0,a	#$val(initialValue),y0
	ELSE
	  move	#$val(feedbackGain),x0
	  macr	x0,y0,a	#$val(initialValue),y0
	ENDIF
	tes	y0,a	 ; If the Extension bit is set, replace with init value
 	move	a,$ref(output)
	move	a,$ref(sum)
    }

    codeblock(cbCoreSat) {
	IF	$val(unityGainB)
	  add	y0,a
	ELSE
	  move	#$val(feedbackGain),x0
	  macr	x0,y0,a
	ENDIF
	move	a,$ref(output)
	move	a,$ref(sum)
    }

    codeblock(cbCoreWrap) {
	IF	$val(unityGainB)
	  add	y0,a
	ELSE
	  move	#$val(feedbackGain),x0
	  macr	x0,y0,a
	ENDIF
	move	a1,$ref(output)
	move	a1,$ref(sum)
    }

    setup {
	sum = initialValue;
	unityGainB = (feedbackGain.asDouble() >= CG56_ONE);
    }
    go {
	addCode(cbSetup);
	const char *p = onOverflow;
	switch (p[0]) {
	case('r'):		//Reset on overflow
		addCode(cbCoreReset);
		break;
	case('s'):		//Saturate on overflow
		addCode(cbCoreSat);
		break;
	case('w'):		//Wrap around on overflow
		addCode(cbCoreWrap);
		break;
	default:
		Error::abortRun(*this,"Unrecognized onOverflow option in CG56Integrator.");
		break;
	}
    }
    execTime {
	const char *p = onOverflow;
	if (p[0]=='r') return 11;
	else return 9;
    }
}
