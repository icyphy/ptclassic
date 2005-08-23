defstar {
	name { DivByInt }
	domain { CG56 }
	desc {
This is an amplifier.  The integer output is the integer input
divided by the integer "divisor" (default 2).  Truncated integer
division is used.
	}
	version { @(#)CG56DivByInt.pl	1.8	06 Oct 1996 }
	author { Brian L. Evans }
	copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	htmldoc {
This star computes the inverse of the <i>divisor</i> state as a fixed-point
number and then generates code that uses integer-fix multiplication
to compute the dividend.  For values of <i>divisor</i> greater than 10,
additional code is generated to ensure that the integer division is correct.
<p>
There is no easy way to implement integer division on the Motorola 56000.
If we used fixed-point division, then the execution time would explode.
Even integer division by two cannot be implemented as an arithmetic
shift right (asr) because in that case, -1 divided by 2 would still be -1.
	}
	location { CG56 main library }
	input {
		name { input }
		type { int }
	}
	output {
		name { output }
		type { int }
	}
	defstate {
		name { divisor }
		type { int }
		default { "2" }
		desc { Inverse of the gain of the amplifier. }
	}
	defstate {
		name { gain }
		type { fix }
		default { "0.5" }
		desc { Inverse of the gain of the amplifier. }
		attributes { A_NONCONSTANT|A_NONSETTABLE|A_YMEM }
	}
	code {
#define	CG56DIVBYINT_POSTTEST() \
	((int(divisor) < -10) || (int(divisor) > 10))
	}
	setup {
		// Avoid dividing by zero
		if ( int(divisor) == 0 ) {
		     Error::abortRun(*this, "divisor cannot be zero");
		     return;
		}

		// Find the absolute value and sign of divisor
		int thedivisor = int(divisor);
		int negdivisor = ( thedivisor < 0 );
		if ( negdivisor ) thedivisor = -thedivisor;

		// Integers are 24 bits (1 sign bit + 23 bits)
		int largest56int = 16777215;		// 2^24 - 1
		if ( thedivisor > largest56int ) {
		     Error::abortRun(*this, "divisor is larger than the ",
				     "24-bit representation allows");
		     return;
		}

		// Compute gain as 1/divisor and truncate to 23 bits
		double gain56 = 1.0/double(thedivisor);
		double adjgain56 = double(int(largest56int * gain56)) /
				   double(largest56int);

		// Check if gain56 is too low or too high by one bit
		// We always want divisor/divisor to equal 1 and
		// (divisor - 1)/divisor = 0
		double absdivisord = double(thedivisor);
		double smallest56fix = 1.1920928955078125e-7;	// 2^(-23)
		double threshold = 1.0 + smallest56fix;
		if (adjgain56 * absdivisord > threshold) {
		    adjgain56 -= smallest56fix;
		}
		if (adjgain56 * absdivisord < 1.0) {
		    adjgain56 += smallest56fix;
		}

		if ( negdivisor ) adjgain56 = -adjgain56;
		gain = adjgain56;
	}
	codeblock(readInputAndMultiplier) {
	move	$ref(input),x1			; x1 = input
	move	$ref(gain),x0			; x0 = 1/divisor = gain
	}
	codeblock(copyInput) {
	move	$ref(input),x1			; copy input to output
	move	x1,$ref(output)
	}
	codeblock(multiplyFixAndInt) {
	mpy 	x0,x1,a			; multiply/truncate
	}
	codeblock(testResult) {
; Register usage: x0 = divisor, x1 = input, y0 = dividend, y1 = -1
	clr	b	#$val(divisor),x0	a,y0
	mpy	x0,y0,a			; integer multiplication
	asr	a
	cmp	x1,a	#$$FFFF,y1
	tgt	y1,b
	move	y0,a
	add	b,a
	move	a1,$ref(output)
	}
	codeblock(writeOutput) {
	move	a,$ref(output)
	}
	go {
		StringList header = "; Divide input by ";
		header << int(divisor) << "\n";
		addCode(header);

		if ( int(divisor) == 1 ) {
		    addCode(copyInput);
		}
		else {
		    addCode(readInputAndMultiplier);
		    addCode(multiplyFixAndInt);
		    if ( CG56DIVBYINT_POSTTEST() ) {
			addCode(testResult);
		    }
		    else {
		    	addCode(writeOutput);
		    }
		}
	}
	exectime {
		if ( CG56DIVBYINT_POSTTEST() ) return 11;
		return 4;
	}
}
