defstar {
	name { Log }
	domain { C50 }
	desc { Base two logarithm }
	version { @(#)C50Log.pl	1.3   06 Oct 1996 }
	author { Luis Gutierrez, ported from Gabriel }
	copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { C50 main library }
	htmldoc {
This star computes the base two logarithm of its input divided by 32,
using polynomial approximation with eight bits of accuracy.
The coefficients of the polynomial are given by <i>d</i>.
<a name="logarithm"></a>
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
                name { d }
                type { FIXARRAY }
                default { "-0.3372223 0.9981958 -0.6626105" }
                attributes { A_UMEM|A_NONSETTABLE }
        }
	constructor {
		noInternalState();
	}

	codeblock(std) {
	lar     ar1,#65535
	mar     *,ar2
	lar     ar2,#$addr(input)
	lacc    *,16,ar1
	rpt     #13
	norm    *-
	lar     ar2,#$addr(output)
	lar     ar3,#$addr(d)
	mar     *,ar2
	sach    *,0,ar3         ; output = x
	lt      *+,ar2          ; treg = d1
	mpy     *               ; p = 0.5*x*d1
	ltp     *,ar3           ; acc = 0.5*x*d1
	add     *+,15,ar2       ; acc = 0.5(x*d1 + d0)
	sach    *,1
	mpy     *,ar3           ; p = 0.5*x(x*d1+d0)
	pac                     ; acc = p
	add     *,15,ar2        ; acc = 0.5*(x(x*d1+d0) + d2)
	sach    *,1
	lacc    *,13
	and     #3ffh,16
	sach    *,0,ar3
	lar     ar3,#11h
	lacc    *,10,ar2
	or      *
	sacl    *
	} 

        go {
        addCode(std);
	}
	execTime { 
		return 39;
	}
 }


