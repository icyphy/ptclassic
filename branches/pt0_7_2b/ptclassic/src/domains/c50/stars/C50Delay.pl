defstar {
	name { Delay }
	domain { C50 }
	desc { A delay star of parameter totalDelay unit delays. }
	version { $Id$ }
	author { A. Baensch, ported from Gabriel }
	copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { C50  control library }
        explanation {
A delay star of parameter totalDelay unit delays.
}

        input  {
                name { input }
	        type { fix }
	}
        output {
		name { output }
		type { fix }
	}
        state  {
                name { delayBuf }
		type { fixarray }
		desc { buffer }
		default { "0" }
                attributes {A_CIRC|A_NONCONSTANT|A_NONSETTABLE|A_UMEM|A_NOINIT}
        }
        state  {
                name { delayBufStart }
                type { int }
                default { 0 }
                desc { pointer to the buffer }
                attributes { A_NONCONSTANT|A_NONSETTABLE|A_UMEM|A_NOINIT }
        }
        state  {
                name { delayBufSpace }
                type { int }
                default { 0 }
                desc { memory space for buffer }
                attributes { A_NONCONSTANT|A_NONSETTABLE|A_UMEM|A_NOINIT }
        }
        state  {
                name { totalDelay }
	        type { int }
	        default { 10 }
	        desc { maximum delay }
         }
	codeblock(block) {
; initialize delay star
; pointer to internal buffer
	.ds	$addr(delayBuf)
	.space  $val(delayBufSpace)
	.ds	$addr(delayBufStart)
  	.word	$addr(delayBuf)
   	.text
	}        		      
        codeblock(one) {
	mar 	*,AR0
        lar	AR0,#$addr(delayBufStart)	;Adress Bufferstart =>AR0
        splk    #$addr(input),BMAR		;Adress Input => BMAR
	lar	AR7,#$addr(output)		;Adress Output => AR7
	lar	AR1,*				;Buffervalue  => AR1
	bldd	BMAR,*,AR7			;Input => Buffer
	sar	AR1,*				;Buffervalue => Output
        }
        codeblock(std) {
	mar	*,AR4
        lar	AR4,#$addr(delayBufStart)	;Adress Bufferstart Poi. => AR4
	lar	AR2,*,AR2			;Adress Buffer position  => AR2
	splk	#$addr(delayBuf),CBSR1		;circ. buffer start adress
	splk	#$addr(delayBuf)+$val(totalDelay)-1,CBER1
	splk	#0ah,CBCR			;enable circ buf with AR2
        splk    #$addr(input),BMAR		;Adress Input => BMAR
	bldd	*,#$addr(output)		;Output = oldest buffer value
	bldd	BMAR,*+,AR4			;Input=>Buffer incr. Bufferadr.
	sar	AR2,*				;new adress from oldest value
	apl	#0fff7h,CBCR			;disable circ. buffer 1
        }
	codeblock(zero) {
	splk	#$addr(input),BMAR		;just move Data from
	bldd	BMAR,#$addr(output)		; Input => Output
	}
        setup {
                delayBuf.resize(totalDelay);
		delayBufSpace=int(delayBuf.size())*16;
        }		
        initCode {
                addCode(block);
	}
        go {
		if(totalDelay==0)
		     addCode(zero);
                else if(totalDelay==1)
	             addCode(one);
		else     
                     addCode(std);
        }		

	execTime { 
		if (totalDelay==0) return 3;
		else if (totalDelay==1) return 8;
		else return 10;
	}
}
