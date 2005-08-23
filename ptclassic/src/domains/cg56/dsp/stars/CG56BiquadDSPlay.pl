defstar {
	name { BiquadDSPlay }
	domain { CG56 }
	desc { Second order IIR filter (Biquad) }
	version { @(#)CG56BiquadDSPlay.pl	1.15 06 Oct 1996 }
	author { Chih-Tsung Huang, ported from Gabriel }
	copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { CG56 dsp library }
	htmldoc {
This second-order biquad IIR filter is tailored to use the coefficients
from the DSPlay filter design tool.
If DSPlay gives the coefficients A B C D E, then define the parameters
as follows:
<p>
<ul>
<li> a = A
<li> b = B
<li> c = C
<li> d = -(D+1)
<li> e = -E
</ul>
<p>
This only works if a, b, c, d, and e are in the range (<i>-</i>1<i>, +</i>1).
The transfer function realized by the filter is:
<pre>
H(z) = a + bz<sup>-1</sup> + cz<sup>-2</sup>
       ---------------------
       1 - (d + 1)z<sup>-1</sup> - ez<sup>-2</sup>
</pre>
The default coefficients implement a low pass filter.
        }
        input {
                name { input }
                type { fix }
        }	    
        output {
		name { output }
		type { fix }
	}
	state {
		name { a }
		type { fix }
		desc { coef }
		default { "4.5535887e-06" }
	}
	state {
		name { b }
		type { fix }
		desc { coef }
		default { "9.1071774e-06" }
	}
	state {
		name { c }
		type { fix }
		desc { coef }
		default { "4.5535887e-06" }
	}
	state {
		name { d }
		type { fix }
		desc { coef }
		default { 0.9939553 }
	}
	state {
		name { e }
		type { fix }
		desc { coef }
		default { "-0.993973494" }
	}
	state {
		name { state }
		type { fixarray }
		desc { internal }
		default { "0[4]" }
                attributes { A_NONCONSTANT|A_NONSETTABLE|A_XMEM }
	}
	state {
		name { coef }
		type { fixarray }
		desc { internal }
		default { "0[6]" }
                attributes { A_NONCONSTANT|A_NONSETTABLE|A_YMEM|A_NOINIT }
	}

        codeblock(coefinit) {
; put coef into appropriate memory location

        org     $ref(coef)
        dc      $val(a)
        dc      $val(b)
        dc      $val(c)
        dc      $val(d)
        dc      $$7fffff
        dc      $val(e)
        org     p:

        }
        codeblock(std) {
        move    #$addr(state),r0
        move    #$addr(coef),r4
        move    $ref(input),x0
        move    y:(r4)+,y0
        mpy     x0,y0,a x:(r0),x1       y:(r4)+,y1
        move    x0,x:(r0)+
        mac     x1,y1,a x:(r0),x0       y:(r4)+,y0
        move    x1,x:(r0)+
        mac     x0,y0,a x:(r0),x1       y:(r4)+,y1
        mac     x1,y1,a x:(r0)+,x0      y:(r4)+,y0
        mac     x0,y0,a x:(r0),x1       y:(r4),y1
        move    x0,x:(r0)-
        macr    x1,y1,a
        move    a1,x:(r0)
        move    a1,$ref(output)
	} 
        initCode {
                addCode(coefinit);
  	}
        go {
                addCode(std);
	}
	execTime { 
		return 15;
	}
 }
