
defstar {
	name { Quantizer }
	domain { CG56 }
	desc { Linear fractional or integer quantizer with adjustable offset. }
	version { $Id$ }
	author { Chih-Tsung Huang, ported from Gabriel }
	copyright { 1992 The Regents of the University of California }
	location { CG56 demo library }
        explanation {
Normally, the output is just the two's complement number
given by the top no_bits of the input, but
an optional offset can be added to shift the output levels up or down.
        }
	input	{
		name { input }
		type { fix }
		}
        output {
		name { output }
		type { fix }
	}
	state {
		name { no_bits }
		type { int }
		desc {   }
		default { 4.0 }
	}
	state {
		name { offset }
		type { int }
		desc {    .  }
		default { 0 }
	}
    	state {
		name { integer_out }
		type {   }
		desc {    . }
		default { "no" }
	}
	state  {
		name { X }
		type { FIX }
		desc { internal }
		default { 0 }
		attributes { A_NONCONSTANT|A_NONSETTABLE }
	}
	state  {
		name { Y }
		type { FIX }
		desc { internal }
		default { 0 }
		attributes { A_NONCONSTANT|A_NONSETTABLE }
	}
        codeblock(std) {
	move	$ref(input),x0
	} 
	ccinclude { <math.h> }
        go { 
		const double X = pow(2,(1.0-double(no_bits)));
		const double Y; 
		const *p=integer_out;
                char buf[80];
                char buf1[80];
		if (p[0]=='y' || p[0]=='Y') {
                            sprintf (buf, "\tmove\t$val(X),a");
                            gencode(CodeBlock(buf));
			    if (offset !=0) {
                            sprintf (buf1, "\tmove\t$val(offset),x1");
                            gencode(CodeBlock(buf1));			     
			    }
			    sprintf (buf2, "\tand\tx0,a");
			    gencode(CodeBlock(buf2));
			    if (offset !=0) {
                            sprintf (buf3, "\tadd\tx1,a");
                            gencode(CodeBlock(buf3));
			    }
                            sprintf (buf4, "\tmove\ta1,$ref(output)");
                            gencode(CodeBlock(buf4));
                }
	        else {
	        if (no_bits!=24) {
	                    if (no_bits<9)
			         { Y=pow(2,(double(no_bits)-1));
			    
					
		gencode(std);
	   }
	execTime { 
		return 3;
	}
 }
