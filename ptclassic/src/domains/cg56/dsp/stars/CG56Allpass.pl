defstar {
	name { Allpass }
	domain { CG56 }
	desc { Allpass filter }
	version { $Id$ }
	acknowledge { Gabriel version by E. A. Lee }
	author { Chih-Tsung Huang, ported from Gabriel }
	copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { CG56 dsp library }
        explanation {
An all-pass filter with one pole and one zero.
The location of these is given by the pole-zero input [1, fig. 1a].
.UH REFERENCES
.ip [1]
J.A. Moore, "About this reverberation business",
\fIComputer Music Journal\fR,
vol. 3, no. 2, pp. 13-28, June, 1979.
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
                attributes {A_CIRC|A_NONCONSTANT|A_NONSETTABLE|A_YMEM|A_NOINIT}
        }
        state  {
                name { delayBufStart }
                type { int }
                default { 0 }
                desc { pointer to the buffer }
                attributes { A_NONCONSTANT|A_NONSETTABLE|A_YMEM|A_NOINIT }
        }

        codeblock(block) {
; initialize allpass
; pointer to internal buffer
        org     $ref(delayBufStart)
        dc      $addr(delayBuf)
        org     p:
        }
        codeblock(std) {
        move    $ref(input),b
        move    $ref(delayBufStart),r0
        move    #>$val(delay)-1,m0
        move    $ref(polezero),x0
        move    y:(r0),y0
        mac     -x0,y0,b        y0,a
        move    b,x1    b,y:(r0)+
        mac     x0,x1,a
        move    a,$ref(output)
        move    r0,$ref(delayBufStart)
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
                 return 16;
	}
}
