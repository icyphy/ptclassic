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
	output {
		name { output }
		type { int }
	}
	input {
		name { valid }
		type { int }
	}
	state {
		name { last }
		type { int }
		default { "-1" }
		desc { internal }
		attributes {A_YMEM|A_NONCONSTANT|A_NONSETTABLE}
	}
	state{
		name { secondToLast }
		type { int }
		default { "-1" }
		desc { internal }
		attributes {A_YMEM|A_NONCONSTANT|A_NONSETTABLE}
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

	codeblock(block){
; Register usage:
; a = input (from x memory)     y0 = last input (from y memory)
; b = valid (from x memory)     y1 = second-to-last input (from y memory)
; x0 = output (either TRUE or FALSE); initially assume a value of zero (FALSE)
	clr	a	$ref(valid),b		$ref(last),y0
	tst	b	$ref(input),a		a,x0
	jeq	$label(not_valid)
	cmp	y0,a	$ref(secondToLast),y1
	jeq	$label(not_valid)
	cmp	y1,a
	jne	$label(not_valid)
	move	#$$FF,x0		; output is TRUE
$label(not_valid)
	move	x0,$ref(output)		y0,$ref(secondToLast)
	move	a0,$ref(last)
	}
	setup {
		secondToLast = int(initialLastInput);
		last = int(initialLastInput);
	}

 	go{
		addCode(block);
	}

	// Return an execution estimate in pairs of cycles
	exectime{ return 13; }
}
