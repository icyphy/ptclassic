defstar {
	name { VarQuasar }
	domain { C50 }
	desc {
A sequence of values is repeated at the output with period N, zero-padding
or truncating to N if necessary.
A value of 0 for N outputs an aperiodic sequence.
        }
	version { $Id$ }
	author { Luis Gutierrez, based on the CG56 version }
	copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { C50 nonlinear functions library }

        input  {
                name { N }
	        type { int }
	}
        output {
		name { output }
		type { fix }
	}
        state  {
                name { data }
		type { fixarray }
		desc { list of values. }
		default { "0.1 0.2 0.3 0.4" }
                attributes { A_NONCONSTANT|A_UMEM }
        }
        state  {
                name { dataPtr }
                type { int }
                default { 0 }
                desc { keep track number of data }
                attributes { A_NONCONSTANT|A_NONSETTABLE|A_UMEM }
        }
        state  {
                name { dataLen }
	        type { int }
	        default { 0 }
	        desc { length of data (internal) }
                attributes { A_NONCONSTANT|A_NONSETTABLE }
        }
      

	codeblock(init){
	.ds	$addr(dataPtr)
	.word	$addr(data)
	.text
	}

        codeblock(std,"") {
	lmmr	ar1,#$addr(dataPtr)	
	mar	*,ar1
	lacl	#$addr(data,@(dataLen)),0	
	samm	arcr		
	cmpr	1
	lar	ar2,#$addr(output)
	xc	1,TC
	lacc	*,16
	mar	*+,ar2
	sach	*,0,ar3		
	lamm	ar1
	sub	#$addr(data),0
	samm	ar3
	lacl	#$addr(data)
	lmmr	arcr,#$addr(N)
	cmpr	1
	lar	ar3,#0000h
	xc	1,TC
	lamm	ar1
	cmpr	0
	mar	*,ar2
	xc	1,TC
	lamm	ar1
	sacl	*,0
	}

        setup {
                dataLen=data.size();
        }		

	initCode{
		addCode(init);
	}

        go {
                addCode(std());
        }		

	execTime { 
                 return 24;
	}
}



