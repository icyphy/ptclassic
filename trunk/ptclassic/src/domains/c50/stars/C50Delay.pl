defstar {
	name { Delay }
	domain { C50 }
	desc { Delay input by totalDelay unit delays. }
	version { $Id$ }
	author { Luis Gutierrez, A. Baensch, ported from Gabriel }
	copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { C50 main library }

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
                attributes { A_NONCONSTANT|A_NONSETTABLE }
        }
        state  {
                name { totalDelay }
	        type { int }
	        default { 10 }
	        desc { maximum delay }
         }

	protected {
		int delayMask;
		int tempDelay;
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
        lar	AR0,#$addr(delayBuf)		;Address Bufferstart =>AR0
	lar	AR7,#$addr(output)		;Address Output => AR7
	lar	AR1,*				;Buffervalue  => AR1
	bldd	#$addr(input),*,AR7			;Input => Buffer
	sar	AR1,*				;Buffervalue => Output
        }

        codeblock(std,"") {
	ldp	#00h
	lmmr	ar2,#$addr(delayBufStart)
	mar	*,ar2
	lacc	#@delayMask
	samm	dbmr
	bldd	#$addr(input),*+
	apl	ar2
	opl	#$addr(delayBuf),ar2
	smmr	ar2,#$addr(delayBufStart)
@( (int(totalDelay) < 256) ? "\tsbrk\t#$val(totalDelay)\n" :\
			     "\tlamm\tar2\n\tsub\t#$val(totalDelay)\n")\
	apl	ar2
	opl	#$addr(delayBuf),ar2
	bldd	*,#$addr(output)		
        }

	codeblock(zero) {
	lmmr	ar1,#$addr(input)
	smmr	ar1,#$addr(output)
	}
        setup {
		int tempSize = 1;

	// the C50 does not support arbitrary modulo addressing; the easiest
	// way to support this in the code is to have the size of the buffer
	// equal to a power of 2

		for (int i = 0; i < 16; i++,  tempSize = 2*tempSize){
			if (tempSize >= int(totalDelay)) break;
		};

	// if the delay buffer is really huge the error will get flagged 
	// later on but leave the error message anyway.  For now just
	// make sure that tempSize <= 2^15

		if (tempSize > 9000) Error::abortRun(*this, "the DSK does not have enough memory",
							    "to handle this delay");
                delayBuf.resize(tempSize);
		delayBufSpace = tempSize * 16;
		delayMask = tempSize - 1;
		tempDelay = int(totalDelay);
		totalDelay = int(totalDelay) + 1;
        }		
        initCode {
                addCode(block);
	}
        go {
		if (tempDelay ==0 ) addCode(zero);
                else if (tempDelay == 1) addCode(one);
		else addCode(std());
        }		

	execTime { 
		if (int(totalDelay) == 0) return 3;
		else if (int(totalDelay) == 1 ) return 8;
		else return 12;
	}
}





