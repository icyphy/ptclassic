 defstar{
	name { DTMFPostTest }
	domain { CG56 }
	desc { 
Returns whether or not a valid dual-tone modulated-frequency has
been correctly detected based on the last three detection results.
        }
	version { @(#)CG56DTMFPostTest.pl 1.17 1/29/96 }
	acknowledge { }
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
	author { Luis Gutierrez }
	copyright { 
Copyright (c) 1990- The Regents of the University of California.
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
		clr b                  #$val(initialLastInput),a1
		move $ref(valid),y0
		cmp y0,b               $ref(last),x0
		jeq $label(not_valid)
		move $ref(secondToLast),b
		cmp x0,b               $ref(input),a
		jeq $label(not_valid)
		cmp x0,a
		jne $label(not_valid)
		move y0,b0
$label(not_valid)
		move b0,$ref(output)
		move a1,$ref(last)
		move x0,$ref(secondToLast)
	}
	setup {
		secondToLast = int(initialLastInput);
		last = int(initialLastInput);
	}

 	go{
		addCode(block);
	}

	// Return an execution estimate in pairs of cycles
	exectime{ return 16; }
}
