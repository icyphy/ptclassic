defstar {
	name { FixTable }
	domain { CG56 }
	desc { Table Lookup }
	version { $Id$ }
	author { Chih-Tsung Huang }
	copyright { 1992 The Regents of the University of California }
	location { CG56 demo library }
        explanation {
.PP
The input accesses a lookup table.  More generally, 56lookup defines
a function which maps input values between -1 and +1-2^-23 into
user-specified output values.
.PP
The basic function is given by a table of constants which specify
values of the function at certain fixed points; the constants are
taken from a file or given by the list \fIcoef\fR.
For a table with n constants, the
first constant specifies the output for an input of -1.  The second
constant specifies the output for an input of -1 + 2/n, and so on,
the last constant handling an input of 1 - 2/n.  In essence, the
table is scaled to fit in the interval from -1 to 1, and the input
acts as an index into the table.
.PP
The \fIinterpolation\fR parameter determines the output for input
values between table-entry points.  If \fIinterpolation\fR is
"linear", the star will interpolate between table entries; if
\fIinterpolation\fR is "none", it will use the next lowest entry.
With a two-element table, for instance, the first constant
specifies the output for an input of -1, while the second handles
an input of 0.  With no interpolation, all inputs less than 0 will
result in an output of the first constant.
.PP
Since the table specifies outputs only for inputs up to +1 - 2/n,
special provisions must be made for inputs between that point and +1.
With \fIinterpolation\fR set to "none", inputs in that range result
in an output of the last table constant.  If  \fIinterpolation\fR
is "linear", the behavior of the function is specified by
\fItableType\fR, which determines the effective table entry for an
input of 1.0.  "periodic" sets the entry equal to the entry for -1,
allowing smoothly wrapping periodic functions.  For "limited", it
is equal to the last constant.  "linear" sets it to twice
the last constant minus the previous one, linearly extending the
table.  Any other value results in a value of 0 for an input of 1.
.PP
As an example, the table [1 2 3 5] with linear interpolation returns
the following input-output pairs: -1=>1; -.75=>1.5; -.5=> 2; -.25=>2.5;
0=>3; .25=>4; .5=>5.  An input of .75 gives an output of 3 for periodic
table type, 5 for limited, 6 for linear, and 2.5 for any other type.
.PP
The \fItableType\fR feature is in fact implemented by tacking an extra
value onto the end of the table.  Thus, the coefficient table will require
one more memory location if \fIinterpolation\fR is "linear".
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





