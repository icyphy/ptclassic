defstar {
	name { Sinc }
	domain { C50 }
	desc {
Sinc function using a polynomial approximation.  Input range of [-1,1)
is scaled by pi.  Output is sin(pi*input)/(pi*input).
	}
	version { $Id$ }
	author { Luis Gutierrez, based on the CG56 version }
	copyright{
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, an ddisclaimer of warranty provisions.
	}
	location { C50 dsp library }
	htmldoc {
<a name="sinc"></a>
This star computes the sinc of the input, which must be in the range
[-1.0, 1.0).  The output equals
<pre>
sin(<i>pi</i>&#183input)
-------------
<i>pi</i>&#183input
</pre>
so the input range is effectively [<i>-pi, pi</i>).  The output is in
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
		name { coeffs }
		type { fixarray }
		default { "0.405871213 -0.822467034 0.5"}
		desc{ internal state}
		attributes{ A_UMEM|A_NONSETTABLE|A_CONSTANT }
	}

	codeblock(sinc){
; sin(x)/x = 2(a + x^2 (b + c x^2))
	lar	ar0,#$addr(input)
	lar	ar1,#$addr(coeffs)	; coeffs -> c,b,a
	lar	ar2,#$addr(output)
	mar	*,ar0
	sqra	*,ar1			; p = 0.5*in^2
	ltp	*+,ar2			; acc = p, treg0 = c
	sach	*,1			; output = in^2
	mpy	*,ar1			; p = 0.5c*in^2
	pac				; acc = p
	add	*+,15,ar2		; acc = 0.5(b+cin^2)
	sach	*,1,ar0			; output = (b+c*in^2)
	sqra	*,ar2			; p = 0.5*in^2
	ltp	*			; treg0 = (b+c*in^2), acc = p
	sach	*,1			; output = in^2
	mpy	*,ar1			; p = 0.5*in^2(b+c*in^2)
	pac				; acc = p
	add	*,15,ar2		; acc = 0.5(a+in^2(b+c*in^2))
	setc	ovm			; set overflow mode
	sfl				; these inst are used
	sacb				; to handle the case
	addb				; where input = 0
	clrc	ovm			; so that output = 0.999..
	sach	*			; output = 4*acc
	}
	go {
		addCode(sinc);
	}
	exectime {
		return 18;
	}
}
