defstar {
	name { Comb }
	domain { C50 }
	desc { Comb Filter }
	version { $Id$ }
	acknowledge { Gabriel version by A.Baensch }
	author { A. Baensch, ported from Gabriel }
	copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { C50 dsp library }
	htmldoc {
<p>
A comb filter with a one-pole lowpass filter
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
                attributes { A_NONCONSTANT|A_NONSETTABLE|A_UMEM }
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
	        default { ONE }
	        desc { internal state }
                attributes { A_NONCONSTANT|A_NONSETTABLE }
         }

	codeblock(block) {
* initialize comb     pointer to internal buffer
	.ds	$val(delayBufStart)
  	.word	$val(delayBuf)
   	.text
	}        		      
        codeblock(std) {
	zap
	mar	*,AR0
	lar	AR0,#$val(delayBufStart)
	lar	AR1,#$addr(time)
	lar	AR6,#$addr(input)
	lar	AR7,#$addr(output)
	lacc	*,AR7
	sacl	*,AR1
	lt	*
	mpya	#>$val(pole)
	apac
	sacl	*
	lt	*,AR1
	mpy	*,AR6
	spl	TREG0
	lacc	*,AR0
	mpy	#$val(X)
	apac
	sacl	*+
	sar	*,INDX
	smmr	INDX,#$addr(delayBufStart)
	}
        
	setup {
                delayBuf.resize(delay);
        }		
        initCode {
                addCode(block);
	}
        go {
                X=1.0-pole.asDouble();
                addCode(std);
        }		

	execTime { 
                 return 21;
	}
}
