defstar {
	name { DivByInt }
	domain { C50 }
	desc {
This is an amplifier.  The integer output is the integer input
divided by the integer "divisor" (default 2).  Truncated integer
division is used.
	}
	version { @(#)C50DivByInt.pl	1.4	06 Oct 1996 }
	author { Luis Gutierrez, based on the CG56 version }
	acknowledge { Brian L. Evans, author of the CG56 version }
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
	location { C50 main library }
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
		attributes { A_NONCONSTANT|A_NONSETTABLE|A_UMEM }
	}
	code {
#define	C50DIVBYINT_POSTTEST() \
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

		// Integers are 16 bits (1 sign bit + 15 bits)
		int largestC50int = 65535;		// 2^16 - 1
		if ( thedivisor > largestC50int ) {
		     Error::abortRun(*this, "divisor is larger than the ",
				     "16-bit representation allows");
		     return;
		}

		// Compute gain as 1/divisor and truncate to 16 bits
		double gainC50 = 1.0/double(thedivisor);
		double adjgainC50 = double(int(largestC50int * gainC50)) /
				   double(largestC50int);

		// Check if gainC50 is too low or too high by one bit
		// We always want divisor/divisor to equal 1 and
		// (divisor - 1)/divisor = 0
		double absdivisord = double(thedivisor);
		double smallestC50fix = 3.051757e-5;	// 2^(-15)
		double threshold = 1.0 + smallestC50fix;
		if (adjgainC50 * absdivisord > threshold) {
		    adjgainC50 -= smallestC50fix;
		}
		if (adjgainC50 * absdivisord < 1.0) {
		    adjgainC50 += smallestC50fix;
		}

		if ( negdivisor ) adjgainC50 = -adjgainC50;
		gain = adjgainC50;
	}

	codeblock(readInputAndMultiplier) {
	lmmr	treg0,#$addr(gain)		; treg0 = gain
	lar	ar0,#$addr(input)		; ar0->input
	mar	*,ar0				; arp= 0
	}

	codeblock(copyInput) {
	lmmr	ar1,#$addr(input)
	smmr	ar1,#$addr(output)
	}

	codeblock(multiplyFixAndInt) {
	mpy	*
	pac
	}


	codeblock(testResult) {
	lar	ar0,#treg0
	sach	*,1
	mpy	#$val(divisor)
	lar	ar0,#$addr(input)
	lacc	*,15
	sacb	
	pac	
	crgt
	lmmr	ar0,#treg0
	xc	1,TC
	sbrk	#1
	smmr	ar0,#$addr(output)
	}

	codeblock(writeOutput) {
	lar	ar0,#$addr(output)
	sach	*,1
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
		    if ( C50DIVBYINT_POSTTEST() ) {
			addCode(testResult);
		    }
		    else {
		    	addCode(writeOutput);
		    }
		}
	}
	exectime {
		if ( C50DIVBYINT_POSTTEST() ) return 16;
		return 7;
	}
}


