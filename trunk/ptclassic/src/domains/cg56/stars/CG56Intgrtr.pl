defstar {
	name { Integrator }
	domain { CG56 }
	desc { 
An integrator with leakage and limits.  With the default parameters,
input samples are simply accumulated, and the running sum is the
output.  If there is an overflow, the integrator will wrap around.
	}
	version { $Id$ }
	author { J. Pino, ported from Gabriel }
	copyright { 1992 The Regents of the University of California }
	location { CG56 demo library }
	explanation {
.Id "filter, integrator"
Limits are controlled by the \fItop\fP and \fIbottom\fP parameters.
If top <= bottom, no limiting is performed (default).
Otherwise, the output is kept between \fIbottom\fP and \fItop\fP.
If \fIsaturate\fP = YES, saturation is performed.
If \fIsaturate\fP = NO, wrap-around is performed (default).
Limiting is performed before output.
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
 	start {
		sum = initialValue;
	}
	codeblock (setLeakage) {
	move	#$val(feedbackGain),x0
	}
	codeblock (setUp) {
	move	$ref(input),a
	move	$ref(sum),y0
	}
	codeblock (noLeakageNotReset) {
	add	y0,a
	}
	codeblock (leakageNotReset) {
	macr	x0,y0,a
	}
	codeblock (noLeakageWithReset) {
	add	y0,a	#$val(initialValue),y0
	}
	codeblock (leakageWithReset) {
	macr	x0,y0,a	#$val(initialValue),y0
	}
	codeblock (resetWrapUp) {
; If the Extension bit is set, replace with initial value
	tes	y0,a
 	move	a,$ref(output)
	move	a,$ref(sum)
	}
	codeblock (saturateWrapUp) {
	move	a,$ref(output)
	move	a,$ref(sum)
	}
	codeblock (wrapAroundWrapUp) {
	move	a1,$ref(output)
	move	a1,$ref(sum)
	}
	go {
		if (feedbackGain != 1) gencode(setLeakage);
		gencode(setUp);
		const char *p = onOverflow;
		switch (p[0]) {
		case('r'):		//Reset on overflow
 			if (feedbackGain == 1) gencode(noLeakageWithReset);
			else gencode(leakageWithReset);
			gencode(resetWrapUp);
			break;
		case('s'):		//Saturate on overflow
			if (feedbackGain == 1) gencode(noLeakageNotReset);
			else gencode(leakageNotReset);
			gencode(saturateWrapUp);
			break;
		case('w'):		//Wrap around on overflow
			if (feedbackGain == 1) gencode(noLeakageNotReset);
			else gencode(leakageNotReset);
			gencode(wrapAroundWrapUp);
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
