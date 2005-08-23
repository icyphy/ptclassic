defstar {
	name { Logic }
	domain { C50 }
	version { @(#)C50Logic.pl	1.6  01 Oct 1996 }
	author { Luis Gutierrez }
	acknowledge { Brian L. Evans, Edward A. Lee }
	copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { C50 main library }
	desc {
This star applies a logical operation to any number of inputs.
The inputs are integers interpreted as Boolean values,
where zero means FALSE and a nonzero value means TRUE.
The logical operations supported are {NOT AND NAND OR NOR XOR XNOR}.
	}
	htmldoc {
The NOT operation requires that there be only one input.
The XOR operation with multiple inputs tests for an odd number
of TRUE values among the inputs.
The other operations are self-explanatory.
	}
	inmulti {
		name { input }
		type { int }
		desc { Input logic values. }
	}
	output {
		name { output }
		type { int }
		desc {
Result of the logic test, with FALSE equal to zero and TRUE equal to a
non-zero integer (not necessarily 1).
		}
	}
	defstate {
		name { logic }
		type { string }
		default { "AND" }
		desc { The test logic: one of NOT AND NAND OR NOR XOR or XNOR. }
	}
	code {
#define NOTID 0
#define ANDID 1
#define NANDID 2
#define ORID 3
#define NORID 4
#define XORID 5
#define XNORID 6
	}
	private {
		int test;
	}
        constructor {
		noInternalState();
		test = NOTID;
	}
	setup {
		const char* cn = logic;
		if ( strcasecmp(cn, "NOT") == 0) {
		    test = NOTID;
		    if (input.numberPorts() > 1)
			Error::abortRun(*this,
			    "NOT operation can only have one input");
		}
		else if ( strcasecmp(cn, "AND") == 0 ) test = ANDID;
		else if ( strcasecmp(cn, "NAND") == 0 ) test = NANDID;
		else if ( strcasecmp(cn, "OR") == 0 ) test = ORID;
		else if ( strcasecmp(cn, "NOR") == 0 ) test = NORID;
		else if ( strcasecmp(cn, "XOR") == 0 ) test = XORID;
		else if ( strcasecmp(cn, "XNOR") == 0 ) test = XNORID;
		else Error::abortRun(*this, "Unrecognized test.");
	}

	// Single-input cases

	codeblock(inverter) {
	lar	ar0,#$addr(input#1)
	mar	*,ar0
	lacc	*,16,ar1		; accH = input; accL = 0
	lar	ar1,#$addr(output)
	xc	1,eq			; if accH = 0 output 1
	add	#01h			; else output 0
	sacl	*
	}

	codeblock(copyInput) {
	lmmr	ar0,#$addr(input#1)
	smmr	ar0,#$addr(output)
	}

	// Two-input cases

	codeblock(twoInputAnd) {
	lar	ar0,#$addr(input#1)
	lar	ar1,#$addr(input#2)
	mar	*,ar0
	lt	*,ar1			; treg0 = input#1
	mpy	*,ar2			; p = input#1*input#2
	pac				; acc = p
	lar	ar2,#$addr(output)	
	xc	1,neq			; 
	lacl	#01h			; if (input#1&input#2) ? acc =1 : acc =0
	}
	

	// General cases

	codeblock(prepareAnd) {
	lar	ar0,#$addr(input#1)
	mar	*,ar0
	lacl	*			; accLow = input#1, accH = 0
	sacb				; accb = acc
	}

//repeat this n-1 times

	codeblock(doAnd,"int i"){
	lar	ar0,#$addr(input#@i)	
	lacl	*
	crlt
	}

	codeblock(endAnd){
	lar	ar1,#$addr(output)
	mar	*,ar1
	xc	1,neq			; if one of the inputs is 0 then acc=0
	lacl	#01h			; else acc=1
	}

	codeblock(prepareOr) {
	lar	ar0,#$addr(input#1)
	mar	*,ar0
	lacl	*
	}

//repeat this n-1 times

	codeblock(doOr,"int i") {
	lar	ar0,#$addr(input#@i)
	or	*
	}

	codeblock(endOr){
	lar	ar1,#$addr(output)
	mar	*,ar1
	xc	1,neq			; if all inputs are zero acc = 0
	lacl	#01h			; else acc = 1
	}

	codeblock(prepareXor) {
	lmmr	dbmr,#$addr(input#1)
	lar	ar1,#$addr(output)
	mar	*,ar1			
	lamm	dbmr
	lmmr	dbmr,#$addr(input#2)
	sacb
	sacl	*			
	}

//repeat n-2 times

	codeblock(doXor,"int i") {
	lamm	dbmr			; acc = n-1 input
	opl	*			; output = output || n-1 input
	lmmr	dbmr,#$addr(input#@i)	; dbmr = n intput(after 2 cycles)
	crlt				; if input is 0 then accB = 0
	nop				; pipeline delay
	}


	codeblock(endXor) {
	lamm	dbmr			; get last input
	crlt				; if last input = 0 accB = 0
	opl	*			; oputput = output || last in
	xc	1,eq			; if acc = 0 then accH = 1; else
	cmpl
	bsar	1
	bsar	15
	and	*			 
	nop
	xc	1,neq
	lacl	#1
	}


	codeblock(invertAcc){
	xor	#0001h,0
	}

        codeblock(saveResult) {
	sacl	*
	}

	go {
		int numinputs = input.numberPorts();
		const char* cn = logic;
		StringList header = "; Boolean ";
		header << cn << " for " << input.numberPorts() << " inputs";

		// The inverter (not) star is the simplest case
		if ( numinputs == 1 ) {
			addCode(header);
			if ( test == NOTID || test == NANDID ||
			     test == NORID || test == XNORID ) {
				addCode(inverter);
			}
			else {
				addCode(copyInput);
			}
			return;
		}

		// For the other stars, we loop through the inputs
		int i = 1;
		header << " using ";

		switch( test ) {
		    case ANDID:
		    case NANDID:
			header << "integer multiplication";
			addCode(header);
			if ( numinputs == 2 ) {
				addCode(twoInputAnd);
			}
			else {
				addCode(prepareAnd);
				for(i=2;  i<=numinputs; i++) 
					addCode(doAnd(i));
				addCode(endAnd);
			}
			break;
		    case ORID:
		    case NORID:
			header << "positive logic";
			addCode(header);
			addCode(prepareOr);
			for (i = 2; i<=numinputs; i++)
				addCode(doOr(i));
			addCode(endOr);
			break;
		    case XORID:
		    case XNORID:
			header << "positive logic";
			addCode(header);
			addCode(prepareXor);
			for( i=3; i<=numinputs; i++)
				addCode(doXor(i));
			addCode(endXor);
			break;
		}

		// Compute final result, adjusting for positive/negative logic
		if (test == NANDID || test == NORID || test == XNORID ) {
			addCode(invertAcc);
		}
		addCode(saveResult);
	}
	exectime {
		int numinputs = input.numberPorts();
		int pairsOfCycles = 0;
		switch( test ) {
		  case NOTID:
		    pairsOfCycles = 7;
		    break;

		  case NANDID:
		    pairsOfCycles += 1;
		    // fall through
		  case ANDID:
		    if (numinputs == 2) pairsOfCycles += 10;
		    else if (numinputs > 2) pairsOfCycles += 3*(numinputs-1)+9;
		    break;

		  case NORID:
		    pairsOfCycles += 1;
		    // fall through
		  case ORID:
		    pairsOfCycles += 8 + 2*(numinputs);
		    break;

		  case XNORID:
		    // fall through
		  case XORID:
		    pairsOfCycles += 4*numinputs + 14;
		    break;
		}

		return pairsOfCycles;
	}
}
