defstar {
	name { FixTable }
	domain { CG56 }
	desc { Table Lookup }
	version { $Id$ }
	author { Chih-Tsung Huang }
	copyright { 1992 The Regents of the University of California }
	location { CG56 demo library }
        explanation {
The input accesses a lookup table.  
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
		name { interpolation }
		type { string }
		desc { interpolation }
		default { "linear" }
	}
	state {
		name { tableType }
		type { string }
		desc { type of table used. }
		default { "periodic" }
	}
	state {
		name { coef }
		type { FIXARRAY }
		desc { values stored in file. }
                default { "-0.5 0.5" }
                attributes { A_YMEM|A_CONSEC }
	}
        state {
                name { addedVal }
                type { fix }
	        desc { internal }
	        default { 0 }
                attributes { A_NONCONSTANT|A_NONSETTABLE|A_YMEM|A_NOINIT }
	}
	state { 
		name { coefNum }
		type { int }
		desc { Number of coefficients. }
		default { 2 }
                attributes { A_NONCONSTANT|A_NONSETTABLE }
	}
        start {
                coefNum=coef.size();
        }		
        initCode {
        const char* p=tableType;
        const char* r = interpolation;
                if(r[0]=='l' || r[0]=='L') {
                          switch (p[4]) {
	                  case 'o': case 'O':      // periodic
                               addedVal=coef[0];
  		               break;
	     	          case 't': case 'T':      // limited
			       addedVal=coef[coefNum-1];
		               break;
                          case 'a': case 'A':      // linear
                               addedVal=2*coef[coefNum-1]-coef[coefNum-2];
                               break;
            	          }
                          gencode(addblockinit);
		}               
        }		
	go {
        const char* q = interpolation;
		switch (q[0]) {
		case 'n': case 'N':    	// none
			gencode(none);
			break;
		default:
			gencode(other);
			return;
	        }
	}

        codeblock(addblockinit) {
; initialize for added value
        org     $ref(addedVal)
        dc      $val(addedVal)
        org     p:
        }
	codeblock (none) {
        move            $ref(input),x1  ;Lookup
        move            #>$val(coefNum),y1
        mpy   x1,y1,a           ;w/o interp.
        add   y1,a
        asr   a         #>$addr(coef),n0
        move            a,r0
        nop
        move            y:(r0+n0),a
        move            a,$ref(output)
	}
        codeblock(other) {
        move            $ref(input),x1  ;Lookup
        move            #>$val(coefNum),y1
        mpy   x1,y1,a           ;w/ interp.
        add   y1,a      #<$$80,b         ;-1
        asr   a         #>$addr(coef),y1
        add   y1,a      a0,b1
        asr   b         a,r0
        move            #<$$40,a         ;.5
        addl  b,a       y:(r0)+,y1  b,x1
        mpy  -x1,y1,a   y:(r0),y1   a,x1
        macr +x1,y1,a
        move            a,$ref(output)
        }
        exectime {
		const char* i = interpolation;  
                if (i[0]=='n' || i[0]=='N')
	            return 14;
                else
	            return 16;
	}

}





