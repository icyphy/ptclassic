defstar {
	name { Sinc }
	domain { CG56 }
	desc {
Sinc function using a polynomial approximation.  Input range of [-1,1)
is scaled by pi.  Output is sin(pi*input)/(pi*input).
	}
	version { @(#)CG56Sinc.pl	1.7	06 Oct 1996 }
	author { Luis Javier Gutierrez and Brian L. Evans }
	copyright{
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, an ddisclaimer of warranty provisions.
	}
	location { CG56 dsp library }
	htmldoc {
This star computes the sinc of the input, which must be in the range
[-1.0, 1.0).  The output equals
<pre>
sin(pi&#183input)
-------------
 pi&#183input
</pre>
<p>
so the input range is effectively (<i>-pi, pi</i>).  The output is in
the range (<i>-</i>0<i>.</i>22<i>, </i>1<i>.</i>0).
	}
	input{
		name { input }
		type { fix }
	}
	output{
		name { output }
		type { fix }
	}
	state {
		name { a }
		type { fix }
		default { 0.5 }
		desc{ internal state}
		attributes{ A_YMEM|A_NONSETTABLE|A_CONSTANT }
	}
	state {
		name { b }
		type { fix }
		default { "-0.822467034" }
		desc{ internal state }
		attributes{ A_YMEM|A_NONSETTABLE|A_NONCONSTANT }
	}
	state {
		name { c }
		type { fix }
		default { "0.405871213" }
		desc{internal state}
		attributes{ A_YMEM|A_NONSETTABLE|A_NONCONSTANT}
	}
	codeblock(sinc){
; sin(x)/x = a + x^2 (b + c x^2)
	move	$ref(input),x0
	move	$ref(b),b                ; B=b
	mpy	x0,x0,a     $ref(c),x1   ; A=x^2  X1=c
	move	a,y0                     ; Y0=x^2
	mac	x1,y0,b     $ref(a),a    ; B=b+cx^2  A=a
	move	b,x0                     ; X0=b+cx^2
	macr	y0,x0,a                  ; A=a+x^2(b+cx^2)
	asl	a
	move	a,$ref(output)           
	}
	go {
		addCode(sinc);
	}
	exectime {
		return 9;
	}
}
