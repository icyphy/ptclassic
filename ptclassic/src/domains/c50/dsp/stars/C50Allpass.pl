defstar {
	name { Allpass }
	domain { C50 }
	desc { Allpass filter }
	version {@(#)C50Allpass.pl	1.4	05/26/98}
	acknowledge { Gabriel version by E. A. Lee }
	author { A. Baensch, ported from Gabriel, G. Arslan }
	copyright {
Copyright (c) 1990-1998 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { C50 dsp library }
	htmldoc {
An allpass filter with one pole and one zero.
The location of these is given by the polezero input.
This is from fig. 1a of J.A. Moore, `About this 
reverberation business', Computer Music. Journal, Vol 3, No.2 
	}

        input  {
                name { input }
	        type { fix }
	}
	input  { 
		name { polezero }
          	type { fix }
	}
        output {
		name { output }
		type { fix }
	}
        state  {
                name { delay }
		type { int }
		desc { delay value }
		default { 10 }
        }
       state  {
                name { delayBuf }
                type { fixarray }
                desc { buffer }
                default { "0" }
                attributes {A_CIRC|A_NONCONSTANT|A_NONSETTABLE|A_BMEM}
        }
        state  {
                name { delayBufStart }
                type { int }
                default { 0 }
                desc { pointer to the buffer }
                attributes { A_NONCONSTANT|A_NONSETTABLE|A_BMEM }
        }

        codeblock(block) {
        mar *,ar0
        lar ar0,#$addr(delayBuf)
        sar ar0,$addr(delayBufStart)
        }

        codeblock(std) {
        mar 	*,AR1
	lar	AR0,#$addr(delayBufStart)	;Address delayBufStart 	=> AR0
	lar	AR1,#$addr(polezero)		;Address polezero	=> AR1
	lar	AR6,#$addr(input)		;Address input		=> AR6
        lar 	AR7,#$addr(output)		;Address output		=> AR7
	splk	#$addr(delayBuf),CBSR1		;startadress circ. buffer 1
	splk	#$addr(delayBuf)+($size(delayBuf)-1)      ;endadress circ. buffer 1
	splk	#08h,CBCR			;enable circ. buf. 1 with AR0
 	lt    	*,AR0				;polezero => TREG0
        mpy	#8000h				;polezero*(-1.0) in Q15
	sph	INDX				;result => INDX =-polezero
	lacc	*,15,AR6			;Accu = oldValue
	sacb					;AccuB=oldValue
	zap					;clear P-Reg. and Accu
	lt	INDX				;-polezero => TREG0
	lacc	*,15,AR0			;input => Accu
	mpy	*,AR1				;P-Reg=-polezero*oldestValue
	lta	*,AR0				;Accu=input-(P-Reg) pole=>TREG0
	sach 	*+,1				;Accu=>newestValue incr BufSt.
	sar	AR0,*,AR7			;store new delayBufStart addres
	sach	INDX,1				;Accu => INDX
	mpy	INDX				;P-Reg=polezero*(inp-pole*oldV)
	pac					;P-Reg => Accu
	addb					;Accu=oldVal+pol(inp-pol*OldVa)
	sach	*,1				;Accu => output
	apl	0fff7fh,CBCR			;disable circ. buffer 1
	}
        setup {
                delayBuf.resize(delay);
        }
        initCode {
                addCode(block);
	}
        go {
                addCode(std);
        }		
	execTime { 
                 return 22;
	}
}
