
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
		name { noBits }
		type { int }
		desc {   }
		default { 4 }
	}
	state {
		name { offset }
		type { int }
		desc {    .  }
		default { 0 }
	}
    	state {
		name { intOut }
		type { string  }
		desc {  yes ignores offset. }
		default { "no" }
	}
	state  {
		name { X }
		type { int }
		desc { internal }
		default { 0 }
		attributes { A_NONCONSTANT|A_NONSETTABLE }
	}
	state  {
		name { Y }
		type { int }
		desc { internal }
		default { 0 }
		attributes { A_NONCONSTANT|A_NONSETTABLE }
	}
        codeblock(std) {
	move	$ref(input),x0
	} 
        codeblock(cont) {
	move	-#$val(X),a
	}
	codeblock(adjust1) {
 	move	#$val(offset),x1
	}
	codeblock(cont1) {
	and	x0,a
	}
	codeblock(adjust2) {
	add	x1,a
	}
	codeblock(cont2) {
	move	a1,$ref(output)
	}
	codeblock(cont3) {
	move	#$val(Y),a1
	move	a1,y0
	}	
	codeblock(cont4) {
	move	#$val(Y),y0
	}
	codeblock(cont5) {
	mpy	x0,y0,a
	move	a1,$ref(output)
	}
	codeblock(cont6) {
	move	x0,$ref(output)
	}
	ccinclude { <math.h> }
        go { 
		
		const int X = 1-int(noBits);
        	const int Y = int(noBits)-1;
		const char *p=intOut;
		if (X<0)
	          X=1/pow(2,X);
		else
	          X=pow(2,X);
		if (Y<0)
	          Y=1/pow(2,Y);
		else
	          Y=pow(2,Y);

	
		gencode(std);
		if (p[0]=='n' || p[0]=='N') {
                            gencode(cont);
			    if (offset !=0) {
                            gencode(adjust1);			     
			    }
  			    gencode(cont1);
			    if (offset !=0) {
                            gencode(adjust2);
			    }
                            gencode(cont2);
                }
	        else {
	        if (noBits !=24) {
	                    if (noBits<9) 
				 gencode(cont3);
			    else
			         gencode(cont4);
			    gencode(cont5);
		}
		else
			gencode(cont6);
	
   	        }
	}

	execTime { 
		return 5;
	}
 }
