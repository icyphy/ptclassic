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
		name { X1 }
		type { FIX }
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
	state  {
		name { Y1 }
		type { FIX }
		desc { internal }
		default { 0 }
		attributes { A_NONCONSTANT|A_NONSETTABLE }
	}
        codeblock(std) {
	move	$ref(input),x0
	} 
        codeblock(contInt) {
	move	#-$val(X),a
	}
        codeblock(contFix) {
	move	#-$val(X1),a
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
	codeblock(cont3Int) {
	move	#$val(Y),a1
	move	a1,y0
	}
	codeblock(cont3Fix) {
	move	#$val(Y1),a1
	move	a1,y0
	}	    
	codeblock(cont4Int) {
	move	#$val(Y),y0
	}
	codeblock(cont4Fix) {
	move	#$val(Y1),y0
	}    
	codeblock(cont5) {
	mpy	x0,y0,a
	move	a1,$ref(output)
	}
	codeblock(cont6) {
	move	x0,$ref(output)
	}
	ccinclude {<math.h>}

        go { 
		
                double a = pow(2,1-double(noBits));
        	double b = pow(2,double(noBits)-1);
		const char *p=intOut;
		X=a;
		X1=a;
		Y=b;
		Y1=b;
		gencode(std);
		if (p[0]=='n' || p[0]=='N') {
	                    if(a>1 || a<-1)
	                    gencode(contInt);
			    else
		            gencode(contFix);

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
			         if (b>1 || b <-1)
				 gencode(cont3Int);
				 else
			         gencode(cont3Fix);
			    else
			         if (b>1 || b <-1)
			         gencode(cont4Int);
				 else
			         gencode(cont4Fix);
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
