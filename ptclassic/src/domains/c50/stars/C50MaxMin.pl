 defstar {
	name { MaxMin }
	domain { C50 }
	desc { Finds maximum or minimum value }
	version { @(#)C50MaxMin.pl	1.12	3/27/96 }
	author { Luis Gutierrez }
	copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { C50 main library }
	explanation {
If \fIcmpareMagnitude\fR is "no", the star finds from among
the \fIinput\fR inputs the one with the maximum or minimum value;
otherwise, it finds from among the \fIinput\fR inputs the one with
the maximum or minimum magnitude.
if \fIoutputMagnitude\fR is "yes", the magnitude of the result is
written to the output, else the result itself is written to the output.
Returns maximum value among N (default 10) samples.
Also, the index of the output is provided (count starts at 0).
	}
	input {
		name {input}
		type {fix}
	}
	output {
		name {output}
		type {fix}
	}
	output {
		name {index}
		type {int}
	}
	state {
		name {N}
		type {int}
		default {10}
		desc {default samples}
	}
	state {
		name {MAX}
		type {int}
		default {"YES"}
		desc {output maximum value else minimum is the output}
	}
	state {
		name {compareMagnitude}
		type {int}
		default {"NO"}
		desc {default is not to compare magnitude}
	}
	state {
		name {outputMagnitude}
		type {int}
		default {"NO"}
		desc {default is not to output magnitude}
	}

	setup {
		noInternalState();
		input.setSDFParams(int(N),int(N)-1);
	}		


	codeblock(compare,"int numSamplesMinus1, int max" ){
	ldp	#00h		; data page pointer == 0
	splk	#@numSamplesMinus1,brcr
	lar	ar0,#$addr(input)
	lar	ar1,#$addr(input)
	mar	*,ar0
	rptb	$starSymbol(lp)
	lacc	*,0,ar1		; acc = current sample
	sub	*,0,ar0		; compare with max/min sample
	lph	ar0
	.if	@max		;if max = 1 seach for max
	xc	1,GT
	.else
	xc	1,LT		;else search for min
	.endif
	sph	ar1		; if necessary
$starSymbol(lp):
	mar	*+		; point to next sample
	}

	codeblock(outIndex){
	lamm	ar1		; ar1 = address of max/min input
	sub	#$addr(input),0 ; acc = index
	samm	ar2		; ar2 = index
	bldd	ar2,#$addr(index)	; output index
	}
	
	codeblock(out){
	bldd	ar1,#$addr(output)	; output value
	}

	codeblock(compareMag,"int numSamplesMinus1,int max"){
	ldp	#0000h
	splk	#@numSamplesMinus1,brcr
	lar	ar0,#$addr(input)
	lar	ar1,#$addr(input)
	mar	*,ar0
	rptb	$starSymbol(lp)
	lacc	*,0,ar1		; load acc with sample
	abs			; take abs of sample
	samm	ar2		; store abs(sample) in ar2
	lacc	*,0		; load acc with max/min sampl
	abs			; take abs of max/min smpl
	sub	ar2		; compare the magnitudes
	lph	ar0
	.if	@max
	xc	1,LT
	.else
	xc	1,GT
	.endif
	sph	ar1
$starSymbol(lp):
	mar	*+		; point to next sample
	}

	codeblock(calcMag){
	lamm	ar1
	abs
	samm	ar1
	}

	codeblock(one){
	lmmr	ar0,#$addr(input)
	smmr	ar0,#$addr(output)
	lar	ar0,#0000h
	smmr	ar0,#$addr(output)
	}

	codeblock(oneMag){
	zap	
	lar	ar0,#$addr(input)
	lar	ar1,#$addr(output)
	lar	ar2,#$addr(index)
	mar	*,ar2
	sach	*,0,ar0
	lacc	*,0,ar1
	abs
	sach	*
	}

 	go {
		if (int(N) > 1) {
			if (int(compareMagnitude)){
				addCode(compareMag((int(N) - 1), int(MAX)));
			} else {
				addCode(compare((int(N) - 1), int(MAX)));
			}
			addCode(outIndex);
			if (int(outputMagnitude))
				addCode(calcMag);
			addCode(out);				
		}
		else{
			if (int(outputMagnitude)) 
				addCode(oneMag);
			else
				addCode(one);
		}
			
	}

	exectime {
		int time=0;
		if ((int(N) == 1) & int(outputMagnitude)) return 9;
		else return 4;
		if (int(compareMagnitude)) time += 6 + 10*int(N);
		else time += 6 + 6*int(N);
		if (int(outputMagnitude)) time += 3;
		time += 5;
		return time;
 	}
}
