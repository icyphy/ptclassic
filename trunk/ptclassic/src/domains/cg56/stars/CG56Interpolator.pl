defstar {
	name { Interpolator }
	domain { CG56 }
	desc { An interpolator }
	version { $Id$ }
	author { Chih-Tsung Huang, ported from Gabriel }
	copyright { 1992 The Regents of the University of California }
	location { CG56 demo library }
	explanation {
This is an interpolator with 2 parameters:
   ratio -- interpolation ratio, default 2
   mode -- 0's are stuffed in mode 1 (default)
           input sample repeated in mode 2.
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
		name {ratio}
		type {int}
		default {2}
		desc {interpolation ratio.}
	}
	state {
		name {mode}
		type {int}
		default {1}
		desc { mode value.}
	}
	start {
		output.setSDFParams(int(ratio),int(ratio)-1);
	}
	codeblock(int_block) {
	move	#$addr(output),r1
	move	$ref(input),x0
	}
	codeblock(one) {
        clr     a      x0,x:(r1)+
        }
        codeblock(g_one) {
        move    a,x:(r1)+
        }
        codeblock(g_two) {
        rep     #$val(ratio)        
	}
	codeblock(cont) {
        rep    #$val(ratio)
        }
        codeblock(done) {
        move   x0,x:(r1)+
        }
	codeblock(done1) {
	move	a,x:(r1)+
	}
 	go {
   		gencode(int_block);
		if (mode==1) {
	             gencode(one);
			if (ratio>1) {
				if (ratio>2) {
					ratio=ratio-1;			 
					gencode(g_two);		   	 
				}
				gencode(done1);
			}
		}
	 	else {
	             if (ratio>1) gencode(cont);
		     gencode(done);
		}
 	}

	exectime {
                  return ratio+2;
 	}
}
 
 
