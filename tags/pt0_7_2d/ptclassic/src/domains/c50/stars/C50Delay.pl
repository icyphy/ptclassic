defstar {
	name { Delay }
	domain { C50 }
	desc { Delay input by totalDelay unit delays. }
	version {@(#)C50Delay.pl	1.7	05/26/98}
	author { Luis Gutierrez, A. Baensch, ported from Gabriel, G. Arslan }
	copyright {
Copyright (c) 1990-1998 The Regents of the University of California.
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
                attributes {A_NONCONSTANT|A_NONSETTABLE|A_BMEM}
        }
        state  {
                name { delayBufStart }
                type { int }
                default { 0 }
                desc { pointer to the buffer }
                attributes { A_NONCONSTANT|A_NONSETTABLE|A_BMEM }
        }
      
        state  {
                name { totalDelay }
	        type { int }
	        default { 10 }
	        desc { maximum delay }
         }

	protected{
		int bufSize;
	}

	codeblock(block) {
; initialize delay star
; pointer to internal buffer
        mar *,ar0
        lar ar0,#$addr(delayBuf)
        sar ar0,$addr(delayBufStart)
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
	lmmr	ar1,#$addr(delayBufStart)
	splk	#$addr(delayBuf),cbsr1
	splk	#($addr(delayBuf)+@(int(bufSize)-1)),cber1
	splk	#9h,cbcr
	mar	*,ar1
	bldd	*,#$addr(output)
	bldd	#$addr(input),*+
	smmr	ar1,#$addr(delayBufStart)
	splk	#0,cbcr
        }

	codeblock(zero) {
	lmmr	ar1,#$addr(input)
	nop
	nop
	smmr	ar1,#$addr(output)
	}

        setup {
		bufSize = int(totalDelay);
                delayBuf.resize(bufSize);
		
        }		
        initCode {
                addCode(block);
	}
        go {
		if (int(totalDelay) ==0 ) addCode(zero);
                else if ( int(totalDelay) == 1) addCode(one);
		else addCode(std());
        }		

	execTime { 
		if (int(totalDelay) == 0) return 3;
		else if (int(totalDelay) == 1 ) return 8;
		else return 10;
	}
}





