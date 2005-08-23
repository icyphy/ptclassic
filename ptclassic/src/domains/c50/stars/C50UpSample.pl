defstar {
	name { UpSample }
	domain { C50 }
	desc { 
Upsample by a factor (default 2), filling with fill (default 0.0).  The
"phase" tells where to put the sample in an output block.  The default
is to output it first (phase = 0). The maximum phase is "factor" - 1.
	}
	version { $Id$ }
	author { A. Baensch, ported from Gabriel }
	copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { C50 control library }
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
		desc { Number of samples produced. }
		attributes { A_SETTABLE }
	}
	state {
		name {phase}
		type {int}
		default {0}
		desc { Where to put the input in the output block. }
		attributes { A_SETTABLE }
	}
	state {
		name {fill}
		type {FIX}
		default {0.0}
		desc { Value to fill the output block. }
		attributes { A_SETTABLE|A_UMEM }
	}
	setup {
		output.setSDFParams(int(factor),int(factor)-1);
		if (int(phase) >= int(factor))
			Error::abortRun(*this, ": phase must be < factor");
	}
	codeblock (initfill) {
	mar	*,AR0				;
	lar	AR1,#$addr(output)		;Address output		=> AR1
	lar	AR0,#$addr(fill)		;Address fill		=> AR0
	lacc	*,15,AR1			;Accu = fill
	}
	codeblock (repeatcode) {
	rpt	#$size(output)-1		;for number of output size
	}
	codeblock (fillcode) {
	sach	*,1				;outputsample(i) = fill 
	}
	initCode {
		addCode (initfill);
		if (factor > 1) addCode(repeatcode);
		addCode(fillcode);
	}
	codeblock (sendsample) {
	mar	*,AR6				;
	lar	AR6,#$addr(input)		;Address input		=> AR6
	bldd	*,#$addr(output,phase)		;output = input
	}
	go {
		addCode(sendsample);
	}
	execTime {
		return 4;
	}
}









