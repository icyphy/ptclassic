defstar {
	name { UpSample }
	domain { C50 }
	desc { 
Upsample by a factor (default 2), filling with fill (default 0.0).  The
"phase" tells where to put the sample in an output block.  The default
is to output it first (phase = 0). The maximum phase is "factor" - 1.
	}
	version {@(#)C50UpSample.pl	1.8	05/26/98}
	author { Luis Gutierrez, A. Baensch, ported from Gabriel, G. Arslan }
	copyright {
Copyright (c) 1990-1998 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { C50 main library }
	input {
		name {input}
		type {anytype}
	}
	output {
		name {output}
		type {=input}
	}
	state {
		name {factor}
		type {int}
		default {2}
		desc { Number of samples produced }
		attributes { A_SETTABLE }
	}
	state {
		name {phase}
		type {int}
		default {0}
		desc { Where to put the input in the output block }
		attributes { A_SETTABLE }
	}
	state {
		name {fill}
		type {FIX}
		default {0.0}
		desc { Value to fill the output block }
		attributes { A_SETTABLE }
	}

	protected {
		// holds effective offset from beginning of output buffer
		int effOffset;
		// holds effective size of output buffer
		int effSize;
		// holds the value of fill as an integer(dsk5a doesn't handle 
		// real numbers when used in instructions like add #<number>
		int fillAsInt;
	}	

	setup {
		output.setSDFParams(int(factor), int(factor)-1);
		if (int(phase) >= int(factor))
			Error::abortRun(*this, "phase must be < factor");
		if (int(phase) == -1 ) phase = int(factor) - 1;
		if (input.resolvedType() == COMPLEX ){ 
			effOffset = 2*int(phase);
			effSize = 2*int(factor);
		} else {
			effOffset = int(phase);
			effSize = int(factor);
		}
		double temp = fill.asDouble();
		if (temp >= 0) {
			fillAsInt = int(32768*temp + 0.5);
		} else {
			fillAsInt = int(32768*(2+temp) + 0.5);
		}
	}

	initCode {
		addCode(initfill());
		if (int(factor) > 1) addCode(repeatcode());
		addCode(fillcode);
	}

	codeblock (initfill,"") {
	lacc	#@fillAsInt,0
	mar	*,AR1			;
	lar	AR1,#$addr(output)	;ar1->output
	}

	codeblock (repeatcode,"") {
	rpt	#@(effSize-1)		;for number of output size
	}

	codeblock (fillcode) {
	sacl	*+,0			;outputsample(i) = fill 
	}

	codeblock (sendsampleStd,"") {
	mar	*,ar1
	lar	AR1,#$addr(input)		;Address input	=> AR1
	bldd	*,#($addr(output)+@effOffset)	;output = input
	}

	codeblock (sendsampleCplx,""){
	mar	*,ar1
	lar	ar1,#$addr(input)
	rpt	#1
	bldd	*+,#($addr(output)+@effOffset)
	}

	go {
		if (input.resolvedType() == COMPLEX) 
			addCode(sendsampleCplx());
		else
			addCode(sendsampleStd());
	}

	execTime {
		if (input.resolvedType() == COMPLEX)
			return 5;
		else	return 3;
	}
}









