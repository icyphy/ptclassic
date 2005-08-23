defstar{
	name { DTMFPostTest }
	domain { CG56 }
	version { @(#)CG56DTMFPostTest.pl	1.9	06 Oct 1996 }
	desc { 
Returns whether or not a valid dual-tone modulated-frequency has
been correctly detected based on the last three detection results.
        }
	author { Luis J. Gutierrez and Brian L. Evans }
	htmldoc {
The assumption is that the 100 msec DTMF interval has been split into
roughly four parts.  This star looks at the last three detection results,
which are represented as integers.  A new digit has been detected
if two consecutive detected digits are the same followed by a third
detected digit that is different.
<p>
This test is useful for two reasons.
First, it filters redundant hits so that only one is reported.
Second, it improves robustness against noisy DTMF signals and speech input.
<h3>References</h3>
<p>[1]  
Pat Mock, "Add DTMF Generation and Decoding to DSP-uP Designs,"
Electronic Data News, March 21, 1985.  Reprinted in
<i>Digital Signal Processing Applications with the TMS320 Family</i>,
Texas Instruments, 1986.
	}
	copyright { 
Copyright (c) 1990-1996 The Regents of the University of California.
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
	clr	a	$ref(valid),b		; b = valid
	move	b,x0	$ref(secondToLast),y0		; x0 = valid, y0 = secondToLast
	move	$ref(input),a	a,y1		; y1 = 0, a = input
	cmp	y0,a	$ref(last),x1		; x1 = last
	teq	y1,b				; input must != secondToLast
	cmp	x1,a	#$val(initialLastInput),y0	; y0 = initialLastInput
	tne	y1,b				; input must == last
	mpy	x0,x0,b		b,$ref(output)		; test valid, output = b
	teq	y0,a				; if !valid, a = bad digit
	move	a,$ref(last)			; last = a
	move	x1,$ref(secondToLast)		; secondToLast = x1 = last
	}
	setup {
		secondToLast = int(initialLastInput);
		last = int(initialLastInput);
	}

 	go{
		addCode(postTest);
	}

	// Return an execution estimate in pairs of cycles
	exectime{ return 11; }
}


