 defstar {
	name { MaxMin }
	domain { C50 }
	desc { Finds maximum or minimum value }
	version {@(#)C50MaxMin.pl	1.11	05/26/98}
	author { Luis Gutierrez, G. Arslan }
	copyright {
Copyright (c) 1990-1998 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { C50 main library }
	htmldoc {
If <i>cmpareMagnitude</i> is "no", the star finds from among
the <i>input</i> inputs the one with the maximum or minimum value;
otherwise, it finds from among the <i>input</i> inputs the one with
the maximum or minimum magnitude.
if <i>outputMagnitude</i> is "yes", the magnitude of the result is
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
	setc	ovm
	lar	ar0,#($addr(input+@(int(N)-1))
	mar	*,ar0
	ldp	#00h
	lar	ar1,#@(int(N)-1)
	splk	#@(int(N)-2),brcr
	lacc	*-,16
	sacb	
	lacc	*-,16
	rptb	$starSymbol(lp)
	.if	@(int(MAX))
	crgt
	.else
	crlt
	.endif
	lacc	*-,16
	xc	1,C
	lar	ar1,brcr
$starSymbol(lp)
	nop
	}

	codeblock(outIndex){
	smmr	ar1,#$addr(index)
	}
	
	codeblock(out){
	lamm	ar1
	add	#$addr(input),0
	samm	ar0
	clrc	ovm
	bldd	*,#$addr(output)
	}

	codeblock(compareMag,""){
	setc	ovm
	lar	ar0,#($addr(input)+@(int(N)-1))
	mar	*,ar0
	ldp	#00h
	lar	ar1,#@(int(N)-1)
	splk	#@(int(N)-2),brcr
	lacc	*-,16
	abs
	sacb	
	lacc	*-,16
	abs
	rptb	$starSymbol(lp)
	.if	@(int(MAX))
	crgt
	.else
	crlt
	.endif
	lacc	*-,16
	abs
	xc	1,C
	lar	ar1,brcr
$starSymbol(lp)
	nop
	}

	codeblock(outMag){
	exar
	abs
	lar	ar0,#$addr(output)
	sach	*,0
	clrc	ovm
	}

	codeblock(one){
	lmmr	ar0,#$addr(input)
	smmr	ar0,#$addr(output)
	lar	ar0,#0000h
	smmr	ar0,#$addr(index)
	}

	codeblock(oneMag){
	setc	ovm
	zap	
	lar	ar0,#$addr(input)
	lar	ar1,#$addr(output)
	lar	ar2,#$addr(index)
	mar	*,ar2
	sach	*,0,ar0
	lacc	*,16,ar1
	abs
	sach	*
	clrc	ovm
	}

 	go {
		if (int(N) > 1) {
			if (int(compareMagnitude)){
				addCode(compareMag());
			} else {
				addCode(compare());
			}
			addCode(outIndex);
			if (int(outputMagnitude)){
				addCode(outMag);
			} else {
				addCode(out);
			}				
		}else{
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
		if (int(compareMagnitude)) time += 11 + 5*(int(N)-1);
		else time += 9 + 4*(int(N)-1);
		if (int(outputMagnitude)) time += 4;
		else  time += 3;
		time += 5;
		return time;
 	}
    }









