defstar {
	name { Delay }
	domain { CG56 }
	desc { A delay star of parameter totalDelay unit delays. }
	version { $Id$ }
	author { Chih-Tsung Huang, ported from Gabriel }
	copyright {
Copyright (c) 1990, 1991, 1992 The Regents of the University of California.
All rights reserved.
See the file ~ptolemy/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { CG56 control library }
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
                attributes {A_CIRC|A_NONCONSTANT|A_NONSETTABLE|A_YMEM|A_NOINIT}
        }
        state  {
                name { delayBufStart }
                type { int }
                default { 0 }
                desc { pointer to the buffer }
                attributes { A_NONCONSTANT|A_NONSETTABLE|A_YMEM|A_NOINIT }
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
	org	$ref(delayBufStart)
  	dc	$addr(delayBuf)
   	org 	p:
	}        		      
        codeblock(one) {
        move    $ref(delayBufStart),r0
        move    $ref(input),x1
        move    y:(r0),y0
        move    x1,y:(r0)
        move    y0,$ref(output)
        }
        codeblock(std) {
        move    $ref(input),x1
        move    $ref(delayBufStart),r0
        move    #$val(totalDelay)-1,m0
        move    y:(r0),y0
        move    x1,y:(r0)+
        move    r0,$ref(delayBufStart)
        move    y0,$ref(output)
        move    #-1,m0
        }
	codeblock(zero) {
	move	$ref(input),a
	move	a,$ref(output)
	}
        setup {
                delayBuf.resize(totalDelay);
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
		if (totalDelay==0) return 2;
		else if (totalDelay==1) return 5;
		else return 8;
	}
}
