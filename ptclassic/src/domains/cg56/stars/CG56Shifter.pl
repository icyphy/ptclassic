defstar {
	name { Shifter }
	domain { CG56 }
	desc { Hard shifter. }
	version { $Id$ }
	author { Chih-Tsung Huang, ported from Gabriel }
	copyright { 1992 The Regents of the University of California }
	location { CG56 demo library }
	explanation {
Scale by shifting left N (default 1) bit.
Negative values of N implies right shifting.
	}
	input {
		name {input}
		type {FIX}
	}
	output {
		name {output}
		type {FIX}
	}
        state {
                name {N}
	        type {int}
	        default { 1 }
	        desc { N shifts }
	}
        state {
		name { saturation }
		type { string }
		default { "YES" }
		desc { If true, use saturation arithmetic }
	}
        codeblock(shiftblock) {
        move    $ref(input),a
        }
	codeblock (one) {
        asl     a
 	}
        codeblock (neg_one) {
        asr     a
        }
        codeblock (g_one) {
        rep     #$val(N)
        asl     a
        }
        codeblock (l_neg_one) {
        rep     -#$val(N)
        asr     a
        }
 	codeblock (sat) {
	move	a,$ref(output)
        }
	codeblock  (nosat) {
	move	a1,$ref(output)
	}
        go {
		const char* p = saturation;
                gencode(shiftblock);
		if (int(N)==1) 
			gencode(one);
                if (int(N)==-1)
                        gencode(neg_one);
                if (int(N)>1)
	                gencode(g_one);
		if (int(N)<-1)
	                gencode(l_neg_one);
		switch (p[0]) {
	        case 'Y': case 'y':
              	               gencode(sat);
			       break;
                case 'N': case 'n':
	                       gencode(nosat);
			       break;
                }
	    }
	exectime {
		if (int(N)>=0)
		   return int(N)+3;
		else
                   return -int(N)+3;  
	}
}


