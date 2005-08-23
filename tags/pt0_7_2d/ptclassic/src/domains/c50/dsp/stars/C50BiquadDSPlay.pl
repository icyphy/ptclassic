defstar {
	name { BiquadDSPlay }
	domain { C50 }
	desc { Second order IIR filter (Biquad) }
	version {@(#)C50BiquadDSPlay.pl	1.5	05/26/98}
	author { A.Baensch, ported from Gabriel, G. Arslan }
	copyright {
Copyright (c) 1990-1998 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { C50 dsp library }
	htmldoc {
Second order IIR filter (Biquad).  This biquad is tailored to use the
coefficients from the DSPlay filter design tool.
If DSPlay gives the coefficients: A, B, C, D and E then define the parameters
as follows:
<p>
<ul>
<li> a = A
<li> b = B
<li> c = C
<li> d = -(D &#43 1)
<li> e = -E
</ul>
<p>
<p>
This only works if a, b, c, d, and e are in the range (-1,&#43<sub></sub>1).
The transfer function realized by the filter is:
<pre>
a &#43 b&#183z<sup>-1</sup> &#43 c&#183z<sup>-2</sup>
-----------------------
1 - (d &#43 1)&#183z<sup>- 1</sup> - e&#183z<sup>-2</sup>
</pre>
<p>
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
		name { state }
		type { fixarray }
		desc { internal }
		default { "0[4]" }
                attributes { A_NONCONSTANT|A_NONSETTABLE|A_BMEM }
	}
	state {
		name { coef }
		type { fixarray }
		desc { internal }
		default { "4.5535887e-06 9.1071774e-06  4.5535887e-06 0.9939553 1 -0.993973494" }
                attributes { A_NONCONSTANT|A_NONSETTABLE|A_BMEM}
	}

        codeblock(std) {
	zap					;clear P-Reg and Accu
        lar     AR0,#$addr(state)		;Address state		=> AR0
        lar     AR4,#$addr(coef)		;Address coef		=> AR4
        lar     AR6,#$addr(input)		;Address input		=> AR6
	lar	AR7,#$addr(output)		;Address output		=> AR7
	mar	*,AR6
	lacc	*,15,AR0			;Accu = input
	splk	#3,BRCR				;Number of loops => BRCR
	rptb	$label(lpMup)			;loop to label(lpMup)
	 lt	*+,AR4				;TREG0=state(n)       incr AR0
	 mpya	*+,AR0				;P-Reg=st(n)*coef(n)  incr AR0
$label(lpMup)					;Accu=Accu+(P-Reg)
	mar	*,AR4
	mpya	*+,AR0				;P-Reg=stat(4)*coef(5) incr AR4
	lt	*-,AR4				;TREG0=stat(4)         decr AR0
	mpya	*,AR7				;P-Reg=stat(4)*coef(6) accumul.
	apac					;Accu = Accu+(P-Reg)
	sach	*,1,AR0				;Accu => output
	dmov	*				;state(3) => state(4)
	sach	*-,1				;Accu => state(3)     decr AR0 
	sbrk	#1				;AR0=AR0-1
	dmov	*				;state(1) => state(2)
	bldd	#$addr(input),*			;input => state(1)
	} 

        go {
                addCode(std);
	}
	execTime { 
		return 27;
	}
 }
