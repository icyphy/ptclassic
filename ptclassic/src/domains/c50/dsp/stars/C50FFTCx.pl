defstar {
name { FFTCx }
domain { C50 }
desc {
}
version { $Id$ }
author { Luis Gutierrez }
copyright {
Copyright (c) 1996-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
}
location { C50 dsp library }
	htmldoc {
}
input {
	name {input}
	type {COMPLEX}
	attributes {A_CIRC}
}
output {
	name {output}
	type {COMPLEX}
}
state { 
	name{order}
	type{int}
	desc { FFT order, must be a power of 2}
	default {128}
}
state {
	name {direction}
	type {Float}
	default {"1.0"}
	desc { Forward (1.0) or Reverse (-1.0)FFT. }
}

state {
	name{tempArray}
	type {fixarray}
	attributes {A_RAM|A_CONSTANT|A_NONSETTABLE|A_NOINIT|A_UMEM}
	
}

state {
	name {twdfct}
	type {FIXARRAY}
	attributes {A_RAM|A_CONSTANT|A_NONSETTABLE|A_NOINIT|A_UMEM}
}

setup {
	twiceOrder = 2*int(order);
	twdfct.resize(order);
	tempArray.resize(twiceOrder);
	input.setSDFParams(order,order-1);
	output.setSDFParams(order,order-1);
}

protected{
	int log2OfOrder;
	int twiceOrder;
	int halfOrder;
	}

initCode {
	
	int temp = 1;
	for (log2OfOrder = 0; log2OfOrder < 16; log2OfOrder++){
		if (temp == order) break;
		temp = temp*2;
	}	
	if (log2OfOrder > 15) {
		if (int(order) < temp) 
			Error::abortRun(*this,
					"Size must be a power ",
					"of two!");
		else	Error::abortRun(*this,
					"Size of FFT must be less ",
					"than 32768.");
	}

	halfOrder = int(order)/2;	
	StringList twiddle;
	char	buf[32];
	double	dtwcos, dtwsin;

	twiddle.initialize();
	twiddle <<"*Twiddle factors\n\t.ds\t$addr(twdfct)\n";

	for(int i = 0; i < halfOrder; i++) {
		dtwcos = cos (i*2.0*M_PI/double( (int (order))));
		dtwsin = double(direction)*(-1.0)* 
		  sin(i*2.0*M_PI/double(int(order)));
		if (dtwcos > C50_ONE) dtwcos = C50_ONE;
		if (dtwsin > C50_ONE) dtwsin = C50_ONE;		
	// twiddle factors are stored in memory with the real part followed
	// by the imaginary part
		sprintf(buf,"%.15f",dtwcos);
		twiddle <<"\t.q15\t"<<buf<<"\n";
		sprintf(buf,"%.15f",dtwsin);
		twiddle <<"\t.q15\t"<<buf<<"\n";

	}
	twiddle<<"\t.text\n";
	addCode(twiddle);
}

codeblock(butterfly){
*butterfly computation for FFT
*based on code given in "TMS320C5x User's Guide"
*@input: ar1->addr of real 1,ar2->addr of real 2,
*	 ar3->addr real twiddle,dp=spm=0,arp=2
*uses ar4 for temporary storage
	rptb	$label(btrfly)
	lt	*+,ar3	 ; treg=re(2),ar2->im(2)
	mpy	*+,ar2	 ; p = 0.5*re(2)*re(t),ar3->im(t)
	ltp	*-,ar3	 ; trg=im(2), ar2->re(2),acc= 0.5re(2)*re(t)
	mpy	*-	 ; p = 0.5*im(2)*im(t),ar3->re(t)
	mpys	*+,ar1	 
; p = 0.5*im(2)*re(t), acc=0.5(re(2)*re(t)-im(2)*im(t)),ar3->im(t)
	sach	1Ah,1	 ; ar4 = 2*acc
	add	*,15	 ; acc = 0.5(re(1)+re(2)re(t)-im(2)im(t))
	sach	*+,1,ar2 ; re(1+) = 2*acc, ar1->im(1) 
	sub	1Ah,16   ; acc =0.5(re(1)+im(t)im(2)-re(t)re(2))
	lt	*	 ; treg0 = re(2)
	sach	*+,1,ar1 ; re(2+) = 2*acc, ar2->im(2)
	lacc	*,15,ar3 ;  acc = im(1)
	mpys	*-	 
; p = im(t)re(2), acc=0.5(im(1)-im(2)re(t)), ar3->re(t)
	lts	*0+,ar2  
; acc = 0.5(im(1)-im(2)re(t)-im(t)re(2)), ar3->re(t)+indx
	sach	*+,1,ar1 ; im(2+) = 2acc, ar2->re(2)+2
	sub	*,16	 ; acc = 0.5(-im(1)-im(2)re(t)-im(t)re(2))
	neg		 ; acc = -acc
$label(btrfly)
	sach	*+,1,ar2 ; im(1+) = 2acc, ar1->re(1)+2, arp = ar2
}

codeblock(firstStage,""){
*this code implements bit reversal of complex inputs
	ldp	#0h
	splk	#2,indx
	lar	ar2,#$addr(input)
	mar	*,ar2
	rpt	#@(int(order)-1)
	bldd	*0+,#$addr(tempArray)
	mar	*-
	rpt	#@(int(order)-1)
	bldd	*0-,#$addr(tempArray,@(order))
	splk	#@(int(order)),indx
	lar	ar2,#$addr(tempArray)
	rpt	#@(twiceOrder - 1)
	bldd	*BR0+,#$addr(output)
*this codeblock implements the butterfly computations
*for the first stage of the FFT
	splk	#@(halfOrder-1),brcr
	splk	#3,indx
	lar	ar1,#$addr(output)
	lar	ar2,#$addr(output,2)
	rptb	$label(stage1)
	lacc    *,15,ar1        ; acc = re(2)
	add     *,15,ar1        ; acc = re(2)+re(1)
	sach    *+,1,ar2       ; re(1+) = re(2)+re(1), ar1->im(1)
	sub     *,16            ; acc = re(1)-re(2)
	sach    *+,1,ar1        ; re(2+) = re(1)-re(2), ar2->im(2)
	lacc    *,15,ar2        ; acc = im(1)
	add     *,15,ar1        ; acc = im(1)+im(2)
	sach    *0+,1,ar2      ; im(1+) = im(1)+im(2), ar1->re(1)+4
	sub     *,16            ; acc = im(1)-im(2)
$label(stage1)
	sach    *0+,1          ; im(2+) = im(1) - im(2), ar2->re(2)+4
}	

codeblock(loopStart,""){
*ar5= num of times to do butterfly
*ar4 = counter for inner loop
*indx = delta between one coeff and the next
*ar6 = num of times to repeat butterfly - 1
*ar7 = counter for outer loop
	splk	#@(int(order)),indx
	splk	#1,ar5
	splk	#@(int(log2OfOrder)-2),ar7
	lar	ar2,#$addr(output)
$starSymbol(ffta)
	lacc	indx,14
	sach	indx,1	;update indx
	sub	#1
	sach	ar4,0	; update ar4
	lacc	ar5,1
	samm	ar5
	sub	#1
	samm	ar6
	samm	brcr
$starSymbol(fftb)
	lacc	ar2
	samm	ar1
	add	ar5,1
	samm	ar2
	lar	ar3,#$addr(twdfct)
}

codeblock(loopEnd,""){
	mar	*,ar4
	banzd	$starSymbol(fftb),*-,ar2
	lmmr	brcr,#16h	; update block counter
	mar	*,ar7
	banzd	$starSymbol(ffta),*-,ar2
	lar	ar2,#$addr(output)
}


	

go {
	addCode(firstStage());
	addCode(loopStart());
	addCode(butterfly);
	addCode(loopEnd());
	}

exectime{
    int time;
    // time for general butterfly computation
    time = halfOrder*18*(log2OfOrder-1);
    // xtra time to execute inner loop
    time += (halfOrder-1)*9;
    // xtra time to execute outer loop
    time += (log2OfOrder -1)*12;
    // time to execute first stage
    time += 5 + 10*(halfOrder);
    // time to do the bit reversal of complex inputs
    time += 10 + 2*twiceOrder;
    return time;
}

}
          


