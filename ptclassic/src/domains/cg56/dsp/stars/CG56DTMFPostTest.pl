defstar{
	name { DTMFPostTest }
	domain { CG56 }
	desc { 
Returns whether or not a valid dual-tone modulated-frequency has
been correctly detected based on the last three detection results.
	}
	version { @(#)CG56DTMFPostTest.pl	1.1	1/30/96 }
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
	author { Luis Javier Gutierrez }
	copyright { 
Copyright (c) 1990- The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
    }
	location { CG56 dsp library }
	input {
		name { input }
		type { INT }
	}
	output {
		name { output }
		type { INT }
	}
	input {
		name { valid }
		type { INT }
	}
	state {
		name { last_in }
		type { INT }
		desc { internal }
		attributes {A_RAM|A_XMEM|A_NONCONSTANT|A_NONSETTABLE}
	}
	state{
		name { sec_to_last }
		type { INT }
		desc { internal }
		attributes {A_RAM|A_XMEM|A_NONCONSTANT|A_NONSETTABLE}
	}
	state { 
		name { initial_last_input }
		type { INT }
		default { " -1" }
		desc{
The value to assign to last and secondToLast internal states, which hold the
previous two valid inputs.  For the purposes of DTMF detection, it is
set to an integer that does not represent a DTMF digit.

		}
		attributes{A_ROM|A_XMEM}
	}

	codeblock ( only ){
		clr a0 #$val(initial_last_input),x0 
		move $ref(valid),y0 
		move $ref(last_in),x1
		move $ref(sec_to_last),y1
		cmp y0,a
		jeq $label(_not_valid)
		move $ref(input), x0
		cmp x0,x1
		jne $label(_not_valid)
		cmp x1,y1
		jeq $label(_not_valid)
		move #-1,a
		;assumes a==TRUE iff a!=0
	  $label(_not_valid)
		move a0,$ref(output)
		move x0,$ref(last_in)
		move x1,$ref(sec_to_last)
	}

	exectime{ return 15; }

	setup{
		last_in = int(initial_last_input);
		sec_to_last = int(initial_last_input);
	}
	
 	go{
		addCode(only);
	}
}
