defstar {
	name { Comb }
	domain { CG56 }
	desc { Comb Filter }
	version { $Id$ }
	author { Chih-Tsung Huang, ported from Gabriel }
	copyright { 1992 The Regents of the University of California }
	location { CG56 demo library }
        explanation {
DSP56000 - A comb filter with a one-pole lowpass filter
           in the delay loop.  The pole location is given
           by the pole parameter.  The reverberation time
           is given by the time input.  This filter is
           from fig. 5 of J. A. Moore, `About this
           reverberation business', Computer Music
           Journal, Vol.  3, No. 2.
	}

        input  {
                name { input }
	        type { fix }
	}
	input  { 
		name { time }
          	type { fix }
	}
        output {
		name { output }
		type { fix }
	}
        state  {
                name { state }
		type { fix }
		desc { internal }
		default { 0.0 }
                attributes { A_NONCONSTANT|A_NONSETTABLE|A_YMEM }
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
                name { delay }
	        type { int }
	        default { 10 }
	        desc { maximum delay }
         }
        state  {
                name { pole }
	        type { fix }
	        default { 0.5 }
	        desc { value of pole }
         }
        state  {
                name { X }
	        type { fix }
	        default { 1.0 }
	        desc { value of pole }
                attributes { A_NONCONSTANT|A_NONSETTABLE }
         }

	codeblock(block) {
; initialize comb
; pointer to internal buffer
	org	$ref(delayBufStart)
  	dc	$addr(delayBuf)
   	org 	p:
	}        		      
        codeblock(std) {
        move    $ref(input),b
        move    $ref(delayBufStart),r0
        move    #>$val(delay)-1,m0
        move    y:(r0),a
; go ahead and output the result
        move    a,$ref(output)
; oldest data sample is now in a. filter it with the LPF
        move    $ref(state),y0
        move    #>$val(pole),x0
        mac     x0,y0,a
; store the state
        move    a,$ref(state)  a,x0
; multiply it by the feedback constant, which is time*(1-pole)
        move    $ref(time),y0
        mpy     x0,y0,a #>$val(X),y0
        move    a,x0
        mac     x0,y0,b
        move    b,y:(r0)+
        move    r0,$ref(delayBufStart)
        move    m1,m0

        }
        start {
                delayBuf.resize(delay);
        }		
        initCode {
                gencode(block);
	}
        go {
                X=1.0-pole;
                gencode(std);
        }		

	execTime { 
                 return 26;
	}
}