defstar {
	name { Log }
	domain { CG56 }
	desc { Logorithm }
	version { $Id$ }
	author { Chih-Tsung Huang, ported from Gabriel }
	copyright { 1992s The Regents of the University of California }
	location { CG56 demo library }
        explanation {
This star computes the base two logarithm of its input divided by 32,
using polynomial approximation with eight bits of accuracy.
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
                name { d1 }
                type { fix }
                default { 0.9981958 }
                attributes { A_ROM|A_YMEM|A_CONSEC }
        }
        state {
                name { d2 }
                type { fix }
                default { "-0.3372223" }
                attributes { A_ROM|A_YMEM|A_CONSEC }
        }
        state {
                name { d3 }
                type { fix }
                default { "-0.6626105" }
                attributes { A_ROM|A_YMEM }
        }
	codeblock(std) {
        move            m7,r7
        move            $ref(input),a
        move            #$addr(d1),r1
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
        gencode(std);
	}
	execTime { 
		return 49;
	}
 }


