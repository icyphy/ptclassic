defstar {
	name { Quant }
	domain { C50 }
	desc {
The star quantizes the input to one of N+1 possible output levels
using N thresholds.
	}
	version { $Id$ }
	author { Luis Gutierrez, based on CG56 version }
	copyright {
Copyright (c) 1996-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { C50 main library }
	htmldoc {
The star quantizes the input to one of <i>N&#43</i>1 possible output levels
using <i>N</i> thresholds.
For an input less than or equal to the <i>N</i>th threshold,
but larger than all previous thresholds,
the output will be the <i>N</i>th level.
If the input is greater than all thresholds,
the output is the (<i>N&#43</i>1)th level.
The <i>levels</i> parameter must be one greater than the number
of <i>thresholds</i>.
	}
	input {
		name {input}
		type {fix}
	}
	output {
		name {output}
		type {fix}
	}
        state  {
                name { thresholds }
                type { fixarray }
                default { "0.1 0.2 0.3 0.4" }
                desc { threshold file }
                attributes { A_NONCONSTANT|A_SETTABLE}
        }
	state {
		name { augThresholds }
		type { fixarray }
		default { "0.1 0.2 0.3 0.4 1.0 " }
		desc { internal state }
		attributes { A_NONCONSTANT|A_NONSETTABLE|A_UMEM|A_NOINIT }
	}
        state  {
                name { levels }
                type { fixarray }
                default { "0.05 0.15 0.25 0.35 0.45" }
                desc { levels file }
                attributes { A_NONCONSTANT|A_UMEM }
        }
 
	constructor {
		noInternalState();
	}

	protected{
		StringList ThresholdValues;
	}

	codeblock(main) {
	setc	ovm			; addition/subs. saturates
	lar	ar0,#$addr(augThresholds)	; ar0 -> threshold
	lar	ar1,#65535		; initialize ar1
	lar	ar3,#$addr(input)	; ar3 -> input
	lar	ar4,#$addr(output)	; ar4 -> output value
	mar	*,ar3
	lacc	*,16,ar0
$starSymbol(lp):
	sub	*+,16,ar1
	bcndd	$starSymbol(lp),GT	; delayed brach to avoid
	mar	*+,ar3			; flushing the pipeline.
	lacc	*,16,ar0		; branch taken here.
	lamm	ar1			; acc = level
	add	#$addr(levels),0	; acc -> indx output value
	samm	ar0			; ar0 -> indx output value
	clrc	ovm
	nop
	lacc	*,0,ar4			; acc = output value
	sacl	*,0,ar2			; output value
	}

	codeblock(one){
	lar	ar0,#$addr(input)
	lar	ar1,#0000h
	lar	ar2,#$addr(augThresholds)
	mar	*,ar0
	lacc	*,16,ar2		; acc = input
	sub	*,16,ar0		; acc = input - threshold
	lar	ar0,#$addr(levels)	; ar0 -> levels
	xc	2,GT			; if input - threshold > 0 do next inst
	mar	*+,ar1			; ar0 -> levels + 1
	mar	*+,ar0
	lar	ar3,#$addr(output)	; ar3 -> output
	lacc	*,16,ar3		; acc = corresponding level
	sach	*			; output high acc.
	}


	initCode{
		addCode(ThresholdValues);
	}

        setup {
		ThresholdValues.initialize();
                int n = thresholds.size() + 1;
		augThresholds.resize(n);
		n--;
		ThresholdValues << "\t.ds\t$addr(augThresholds)\n";
		for ( int j = 0; j < n; j++) {
			ThresholdValues << "\t.q15\t"
					<<  double(thresholds[j])
					<< "\n";
		}
		ThresholdValues << "\t.word\t32767\n";
                if (levels.size() == 0) {       // set to default: 0, 1, 2...
                        levels.resize(n + 1);
                        for (int i = 0; i <= n; i++)
                                levels[i] = i;
                }
                else if (levels.size() != n+1) {
                        Error::abortRun (*this,
                              "must have one more level than thresholds");
                }
        }
	go {
		if (thresholds.size() == 1) addCode(one);
		else {
			addCode(main);
		}
	}
	exectime {
		if ( thresholds.size() == 1 ) return 13;
	        return 13 + 4*int(thresholds.size());
	}
}
