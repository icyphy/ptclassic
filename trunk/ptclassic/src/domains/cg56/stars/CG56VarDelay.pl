defstar {
	name { VarDelay }
	domain { CG56 }
	desc { A variable delay with linear interpolation. }
	version { @(#)CG56VarQuasar.pl	1.2 7/14/92 }
	author { Chih-Tsung Huang, ported from Gabriel }
	copyright { 1992 The Regents of the University of California }
	location { CG56 demo library }
        explanation {
Delay will vary between 0 and maxDelay as the control varies between -1.0 and 1.0.
	}

        input  {
                name { input }
	        type { fix }
	}
	input  { 
		name { control }
          	type { fix }
	}
        output {
		name { output }
		type { fix }
	}
        state  {
                name { buf }
		type { fixarray }
		desc { buffer }
		default { "0" }
                attributes {A_CIRC|A_NONCONSTANT|A_NONSETTABLE|A_YMEM|A_NOINIT}
        }
        state  {
                name { bufStart }
                type { int }
                default { 0 }
                desc { pointer to the buffer }
                attributes { A_NONCONSTANT|A_NONSETTABLE|A_YMEM|A_NOINIT }
        }
        state  {
                name { maxDelay }
	        type { int }
	        default { 10 }
	        desc { maximum delay }
         }
	codeblock(block) {
	org	$ref(bufStart)
  	dc	$addr(buf)
   	org 	p:
	}        		      
        codeblock(std) {
        movec   #$val(maxDelay),m0         ; MAX may be short immediate
        move    $ref(bufStart),r0
        clr     a       $ref(input),y0
        movec   m0,a1
        move    a,x1    y0,y:(r0)+
        move    $ref(control),y1
        mac     x1,y1,a r0,$ref(bufStart)
        asr     a       (r0)-
        clr     b       (r0)-
        move    a1,n0           ; offset
        move    a0,b1           ; remainder
        asr     b       #<0.5,a
        rnd     b       (r0)-n0 ; linear interpolation
        subl    b,a     y:(r0)+,y1      b,x1    ; a=1-b
        mpy     x1,y1,a y:(r0),y1       a,x1
        macr    x1,y1,a
        move    a,$ref(output)
        movec   m1,m0           ; restore m0
        }
        start {
                buf.resize(maxDelay+1);
        }		
        initCode {
                gencode(block);
	}
        go {
                gencode(std);
        }		

	execTime { 
                 return 19;
	}
}
