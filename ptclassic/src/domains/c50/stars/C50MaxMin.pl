 defstar {
	name { MaxMin }
	domain { C50 }
	desc { Finds maximum or minimum value }
	version { $Id$ }
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


	codeblock(compare,"" ){
	lar	ar0,#$addr(input)
	mar	*,ar0
	ldp	#00h
	splk	#@(int(N)-1),brcr
	lacc	*+,16
	sacb	
	rptb	$starSymbol(lp)
	lacc	*+,16
	.if	@(int(MAX))
	crgt
	.else
	crlt
	.endif
	nop
	xc	1,TC
$starSymbol(lp)
	lph	ar0
	}

	codeblock(outIndex){
	pac
	sub	#0001,0
	sub	#$addr(input),16
	lar	ar0,#$addr(output)
	sach	*,0
	}
	
	codeblock(out){
	lar	ar0,#$addr(output)
	exar	
	sach	*,0
	}

	codeblock(compareMag,""){
	lar	ar0,#$addr(input)
	mar	*,ar0
	ldp	#00h
	splk	#@(int(N)-1),brcr
	lacc	*+,16
	abs
	sacb	
	rptb	$starSymbol(lp)
	lacc	*+,16
	abs
	.if	@(int(MAX))
	crgt
	.else
	crlt
	.endif
	nop
	xc	1,TC
$starSymbol(lp)
	lph	ar0
	}

	codeblock(outMag){
	exar
	abs
	lar	ar0,#$addr(output0
	sach	*,0
	}

	codeblock(one){
	lmmr	ar0,#$addr(input)
	smmr	ar0,#$addr(output)
	lar	ar0,#0000h
	smmr	ar0,#$addr(index)
	}

	codeblock(oneMag){
	zap	
	lar	ar0,#$addr(input)
	lar	ar1,#$addr(output)
	lar	ar2,#$addr(index)
	mar	*,ar2
	sach	*,0,ar0
	lacc	*,16,ar1
	abs
	sach	*
	}

 	go {
		if (int(N) > 1) {
			if (int(compareMagnitude)){
				addCode(compareMag());
			} else {
				addCode(compare());
			}
			addCode(outIndex);
			if (int(outputMagnitude))
				addCode(outMag);
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
		if (int(N) == 1) {
			time += 4;
			if (int(outputMagnitude)) time += 5;
			return time;
		}
		if (int(compareMagnitude)) time += 8 + 6*int(N);
		else time += 7 + 5*int(N);
		if (int(outputMagnitude)) time += 4;
		else  time += 3;
		time += 5;
		return time;
 	}
}
