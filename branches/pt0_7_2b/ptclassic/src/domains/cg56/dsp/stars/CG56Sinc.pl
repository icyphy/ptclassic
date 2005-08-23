defstar {
	name { Sinc }
	domain { CG56 }
	desc { returns sinc(x) using polynomial approximation}
	version { @(#)CG56Sinc.pl        1.31 2/23/96  }
	author { Luis Gutierrez }
	copyright{
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, an ddisclaimer of warranty provisions.
	}
	location { CG56 signal sources library }
	explanation { 
Calculates sinc(x)=sin(PI*x)/(PI*x) using a fifth order polynomial.
Input x must be between -1 and 1.	
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
		default { 0.405871213 }
		desc{internal state}
		attributes{ A_YMEM|A_NONSETTABLE|A_NONCONSTANT}
	}
	codeblock(sinc){
;sinc(x)/x = a + x^2(b + cx^2)
		move $ref(input),x0
		move $ref(b),b                ; B=b
		mpyr x0,x0,a     $ref(c),x1   ; A=x^2  X1=c
		move a,y0                     ; Y0=x^2
		macr x1,y0,b     $ref(a),a    ; B=b+cx^2  A=a
		move b,x0                     ; X0=b+cx^2
		macr x0,y0,a                  ; A=a+x^2(b+cx^2)
		asl a
		move a,$ref(output)           
		
	}
	exectime { return 9; }
	go{
		addCode(sinc);
	}
}
