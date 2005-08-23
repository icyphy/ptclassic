defstar {
	name { QuantRange }
	domain { C50 }
	desc {
The star quantizes the input to one of N+1 possible output levels
using N thresholds.
	}
	version { @(#)C50QuantRange.pl	1.4	01 Oct 1996 }
	author { A. Baensch }
	copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { C50 main library }
	htmldoc {
<p>
The star quantizes the input to one of N+1 possible output levels
using N thresholds.
For an input less than or equal to the Nth threshold,
but larger than all previous thresholds,
the output will be the Nth level.
If the input is greater than all thresholds,
the output is the (N+1)th level.
There must be one more level than thresholds.
	}
	input {
		name {input}
		type {fix}
	}
	input {
		name {range}
		type {fix}
	}
	output {
		name {output}
		type {fix}
	}
        state  {
                name { thresholds }
                type { fixarray }
                default { "0" }
                desc { threshold values }
                attributes { A_NONCONSTANT|A_UMEM }
        }
        state  {
                name { levels }
                type { fixarray }
                default { "-0.5 0.5" }
                desc { levels values }
                attributes { A_NONCONSTANT|A_UMEM }
        }
        state  {
                name { X }
                type { int }
                default { 4 }
                desc { internal }
                attributes { A_NONCONSTANT|A_NONSETTABLE }
        }

	codeblock (main) {
	mar	*,AR1				;
        lar	AR0,#$addr(thresholds)		;Address thresholds	=>AR0
	lar	AR1,#$addr(range)		;Address range		=>AR1
        lar	AR4,#$addr(levels)		;Address levels		=>AR4
        lar     AR6,#$addr(input)		;Address input		=>AR6
	lar	AR7,#$addr(output)		;Address output		=>AR7
	lt	*,AR6				;TREG0 = range
	lacc	*,15,AR0			;Accu = input
	sacb					;store Accu in Accub
	splk	#$val(X)-1,BRCR			;store number of loops in BRCR
	rptb	$label(term)			;loop to label(term)
	 mpy	*+,AR4				;P-Reg = range*thresholds(i)
	 pac					;Accu = P-Reg
	 crlt					;range*thresholds(i) < input
	 bcnd 	$label(again),C			;if yes jump to label(again)
	 splk	#0,BRCR				;reset number of loops
	 b	$label(term)			;jump to label(term)
$label(again)					;
 	 mar	*+,AR0				;
$label(term)					;
	zap					;clear P-Reg and Accu
	mpy	*,AR7				;P-Reg = range*levels(i)
	pac					;Accu = P-Reg
	sach	*,1				;output = range*levels(i)
	}

        codeblock(other) {
	mar	*,AR1
        lar	AR0,#$addr(thresholds)
	lar	AR1,#$addr(range)
        lar	AR4,#$addr(levels)
        lar     AR6,#$addr(input)
	lar	AR7,#$addr(output)
	lt	*,AR6
	lacc	*,15,AR0
	sacb
	mpy	*,AR4
	pac
	crgt
	bcnd	$label(term)
	mar	*+
$label(term)
	zap
	mpy	*,AR7
	pac
	sach	*,1
        }
	constructor {
		noInternalState();
	}
        setup {
		if (levels.size() != thresholds.size()+1) {
		    Error::abortRun(*this,
			"Must have one more level than number of thresholds");
		}
        }
	go {
                 X = thresholds.size();

                 if (thresholds.size()>1) addCode(main);
		 else addCode(other);
	}
	exectime {
	        return 15+7*int(thresholds.size());
	}
}
