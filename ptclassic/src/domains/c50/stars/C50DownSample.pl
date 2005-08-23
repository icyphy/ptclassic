defstar {
	name { DownSample }
	domain { C50 }
	desc { 
A decimator by a given "factor" (default 2).
The "phase" tells which sample to output.
If phase = 0, the most recent sample is the output,
while if phase = factor-1 the oldest sample is the output.
Phase = 0 is the default.  Note that "phase" has the opposite
sense of the phase parameter in the UpSample star, but the
same sense as the phase parameter in the FIR star.
	}
	htmldoc {
<a name="decimation"></a>
	}
	version {@(#)C50DownSample.pl	1.8	05/26/98}
	author { A. Baensch, Luis Gutierrez, ported from Gabriel, G. Arslan }
	copyright {
Copyright (c) 1990-1998 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { C50 main library }
	input {
		name {input}
		type {ANYTYPE}
	}
	output {
		name {output}
		type {=input}
	}
	state {
		name {factor}
		type {int}
		default {2}
		desc { Downsample factor }
		attributes { A_SETTABLE }
	}
	state {
		name {phase}
		type {int}
		default {0}
		desc { Downsample phase }
		attributes { A_SETTABLE }
	}
	protected{
		// effective offset from the beginning of input buffer
		int effOffset;
		// effective size of input
		int effSize;
	}

	setup {
		input.setSDFParams(int(factor),int(factor)-1);
		if (int(phase) >= int(factor))
			Error::abortRun(*this, "phase must be < than factor");

		if (output.resolvedType() == COMPLEX){
			 effOffset = 2*int(phase);
			 effSize = 2*int(factor);
		} else	{
			effOffset = int(phase);
			effSize = int(factor);
		}

	}
	codeblock (sendCxSample,""){
	lar	ar1,#$addr(output)
	mar	*,ar1
	rpt	#1
	bldd	#($addr(input)+@(effSize - effOffset - 2)),*+
	}

	codeblock (sendSample,"") {
	lar	ar1,#$addr(output)
	mar	*,ar1
	bldd	#($addr(input)+@(effSize - effOffset - 1)),*
	}

	go {
		if (output.resolvedType() == COMPLEX) addCode(sendCxSample());
		else addCode(sendSample());
	}

	execTime {
		return 3;
	}
}

