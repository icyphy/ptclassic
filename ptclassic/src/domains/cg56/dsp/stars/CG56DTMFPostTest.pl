defstar{
	name { DTMFPostTest }
	domain { CG56 }
	version { $Id$ }
	desc { 
Returns whether or not a valid dual-tone modulated-frequency has
been correctly detected based on the last three detection results.
        }
	author { Luis J. Gutierrez and Brian L. Evans }
	explanation{
The assumption is that the 100 msec DTMF interval has been split into
roughly four parts.  This star looks at the last three detection results,
which are represented as integers.  A new digit has been detected
if two consecutive detected digits are the same followed by a third
detected digit that is different.
.PP
This test is useful for two reasons.
First, it filters redundant hits so that only one is reported.
Second, it improves robustness against noisy DTMF signals and speech input.
.UH REFERENCES
.IP [1]
Pat Mock, "Add DTMF Generation and Decoding to DSP-uP Designs,"
Electronic Data News, March 21, 1985.  Reprinted in
\fIDigital Signal Processing Applications with the TMS320 Family\fR,
Texas Instruments, 1986.
	}
	copyright { 
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
    }
	location { CG56 dsp library }
	input {
		name { input }
		type { int }
	}
	input {
		name { valid }
		type { int }
	}
	output {
		name { output }
		type { int }
	}
	state {
		name { last }
		type { int }
		default { "-1" }
		desc { internal }
		attributes {A_NONCONSTANT|A_NONSETTABLE|A_YMEM}
	}
	state{
		name { secondToLast }
		type { int }
		default { "-1" }
		desc { internal }
		attributes {A_NONCONSTANT|A_NONSETTABLE|A_YMEM}
	}
	state { 
		name { initialLastInput }
		type { int }
		default { "-1" }
		desc{
The value to assign to last and secondToLast internal states, which hold
the previous two valid inputs.  For the purposes of DTMF detection, it
is set to an integer that does not represent a DTMF digit.
        	}
	}

	codeblock(postTest){
; Register usage:
; a = input (from x memory)     x1 = last input value (from y memory)
; b = valid (from x memory)	y1 = second-to-last input value (from y memory)
; y0 = FALSE
	clr	a	$ref(valid),b		; b = valid
	move	$ref(input),a	a,y0		; a = input, y0 = FALSE
	move	$ref(last),x1		; x1 = last input value
	cmp	x1,a	$ref(secondToLast),y1		; y1 = 2nd-to-last input 
	tne	y0,b		; if last != input, then b = y0 = FALSE
	cmp	y1,a	x1,$ref(secondToLast)		; update second-to-last value
	teq	y0,b		; if secondTolast = input, then b = y0 = FALSE
	move	a,$ref(last)		; update last value
	move	b,$ref(output)		; write output
	}
	setup {
		secondToLast = int(initialLastInput);
		last = int(initialLastInput);
	}

 	go{
		addCode(postTest);
	}

	// Return an execution estimate in pairs of cycles
	exectime{ return 9; }
}
