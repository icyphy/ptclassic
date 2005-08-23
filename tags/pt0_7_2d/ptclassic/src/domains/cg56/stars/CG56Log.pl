defstar {
	name { Log }
	domain { CG56 }
	desc { Base two logarithm }
	version { @(#)CG56Log.pl	1.14 06 Oct 1996 }
	author { Chih-Tsung Huang, ported from Gabriel }
	copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { CG56 main library }
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
                default { "0.9981958 -0.3372223 -0.6626105" }
                attributes { A_ROM|A_YMEM|A_NONSETTABLE }
        }
	constructor {
		noInternalState();
	}

	codeblock(std) {
        move            m7,r7
        move            $ref(input),a
        move            #>$addr(d),r1
        rep   #23
        norm  r7,a
        move            a,x0
        mpyr  x0,x0,a           y:(r1)+,y0
        mpy   x0,y0,a   a,x1    y:(r1)+,y0
        mac   x1,y0,a           y:(r1)+,y0
        add   y0,a
        asl   a
        asl   a
        asl   a
        move            r7,a2
        rep   #6
        asr   a
        rnd   a
        move            a,$ref(output)
	} 

        go {
        addCode(std);
	}
	execTime { 
		return 49;
	}
 }


