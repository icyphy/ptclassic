defstar {
	name { FIR }
	domain { C50 }
	desc {
Finite Impulse Response (FIR) filter.
Coefficients are in the "taps" state variable.  Default coefficients
give an 8th order, linear phase lowpass filter.  To read coefficients
from a file, replace the default coefficients with "<fileName".
Decimation parameter > 1 reduces sample rate.  Interpolation parameter
> 1 increases sample rate.
	}
	version { $Id$ }
	author { A. Baensch, ported from Gabriel }
	copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { C50 dsp library }
	explanation {
The output of the FIR filter is given by:
.EQ
        y(n) ~ = ~ sum from { i = 0 } to { N - 1 } { c[i] x[N-i] }
.EN
where N is the order of the filter given by the number of coefficients in
\fItaps\fP.
c[i], i = 0..N-1 are the tap coefficients; and x[T-i] is the input i samples
before the current input.
.PP
The default filter is a linear-phase equiripple lowpass filter with its 3dB
cutoff frequency at about 1/3 of the Nyquist frequency.
	}
	input {
		name {input}
		type {FIX}
	}
	output {
		name {output}
		type {FIX}
	}
	state {
		name {taps}
		type {FIXARRAY}
		default {
	"-.040609 -.001628 .17853 .37665 .37665 .17853 -.001628 -.040609"
		}
		desc { Filter tap values. }
		attributes { A_NONCONSTANT|A_UMEM|A_NOINIT }
	}
	state {
		name {oldsample}
		type {fixarray}
		default {0}
		desc {internal}
                attributes {
               A_CIRC|A_NONCONSTANT|A_NONSETTABLE|A_UMEM|A_NOINIT
	        }
	}
        state {
                name {oldsampleStart}
	        type {int}
	        default {0}
                desc { pointer to oldsample }
                attributes {A_NONCONSTANT|A_NONSETTABLE|A_UMEM|A_NOINIT}
        }	    
        state {
                name {oldsampleSize}
	        type {int}
	        default {0}
                desc { size of oldsample }
                attributes {A_NONCONSTANT|A_NONSETTABLE}
        }	
	state {
                name {oldsampleSpace}
	        type {int}
	        default {0}
                desc { space for oldsample values in TMS320C5x }
                attributes {A_NONCONSTANT|A_NONSETTABLE}
        }	        
        state {
                name {tapsNum}
	        type {int}
	        default {8}
                desc { internal }
                attributes {A_NONCONSTANT|A_NONSETTABLE}
        }	    
       
	setup {
	      tapsNum=taps.size();
	      int oldSampleNum = tapsNum;
	      oldsample.resize(oldSampleNum);
        }
        initCode {
		int i = 0;
		StringList tapInit;
		tapInit = "\t.ds\t$addr(taps)\n";
		for (i = 0; i < taps.size() ; i++)
			tapInit << "\t.q15\t" << double(taps[i]) << '\n';
		tapInit << "\t.text\n";
		addCode(tapInit);
		oldsampleSize=oldsample.size();
		oldsampleSpace=int(oldsampleSize)*16;
		if (oldsampleSize>0) 
                     addCode(block);
        }
        
	go {
                addCode(must);
	        if(tapsNum>2) addCode(greaterTwo);
		if(tapsNum==2) addCode(equalTwo);
		if(tapsNum<2) addCode(lessTwo);
	}

	codeblock(block) {
        .ds     $addr(oldsample)		;initialize FIR variables
        .space  $val(oldsampleSpace)
        .ds     $addr(oldsampleStart)		;pointer to buffer
        .word	$addr(oldsample)
        .text
        }

        codeblock(must) {
	mar	*,AR4				;
        lar     AR0,#$addr(taps) 		;Adress Taps     	=> AR0
        lar     AR1,#$addr(input)		;Adress Input    	=> AR1
	lar	AR4,#$addr(oldsampleStart)	;Adress pointer	 	=> AR2
	lar	AR2,*,AR2 			;Address oldsample	=> AR2
	lar	AR7,#$addr(output)		;Adress output   	=> AR7
        }    
        codeblock(greaterTwo) {
	splk	#$addr(oldsample),CBSR1		;Startadress circular buffer 1
	splk	#$addr(oldsample)+$val(oldsampleSize)-1,CBER1
	splk	#0ah,CBCR			;enable circ. buf. 1 with AR2
	splk	#$addr(taps),BMAR
	rptz    #$val(tapsNum)-1		;
	 mads	*+				;Accu = SUM[u(N-k)*c(k)]
	nop					;
	mar	*,AR7				;
	apac					;
	sach    *,1,AR2 			;Accu => Output
	bldd	#$addr(input),*+,AR4		;Input => newSample in Buffer
	sar	AR2,*   			;store new oldsampleStart addr
	apl	#0fff7h,CBCR			;disable circ. buffer 1
	 }    
        codeblock(equalTwo) {
	zap					;clear P-Reg. and Accu
	lmmr	BMAR,#$addr(oldsampleStart)	;Adress oldsampleStart => BMAR
	madd	*+,AR1				;Accu = u(2)*c(1)
	lt	*,AR0				
	mpya	*,AR7				;Accu = Accu + Input*c(2)
	apac
	sach	*,1,AR4				;Accu => output
	lacc	*+,15				;u(k-1) in accu
	sach	*-,1				;u(k-2) = u(k-1)
	bldd	#$addr(input),*			;u(k-1) = input
        }
        codeblock(lessTwo) {
	zap					;clear P_reg. and Accu
	mar 	*,AR1
	lt 	*,AR7				;Input => TREG0
        mpy     #$val(taps)			;Input*c
	pac					;Accu =Input*c
	sach	*,1				;Accu => output
        }
  
execTime  {
	if (tapsNum>1)  return 17+2*(tapsNum-2);
	if (tapsNum==2) return 14;
	if (tapsNum<2)  return 10;
	return 0;
	}
}











































































































































































































































































