defstar {
	name { LookupTbl }
	domain { C50 }
	desc { Table lookup }
	version {@(#)C50LookupTbl.pl	1.7	05/26/98}
	author { A. Baensch, G. Arslan }
	copyright {
Copyright (c) 1990-1998 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { C50 main library }
	htmldoc {
<p>
The input accesses a lookup table.  More generally, this star defines
a function which maps input values between -1 and +1-2^-15 into
user-specified output values.
<a name="table lookup"></a>
<a name="lookup table"></a>
<p>
The basic function is given by a table of constants which specify
values of the function at certain fixed points; the constants are
taken from a file or given by the list <i>coef</i>.
For a table with n constants, the
first constant specifies the output for an input of -1.  The second
constant specifies the output for an input of -1 + 2/n, and so on,
the last constant handling an input of 1 - 2/n.  In essence, the
table is scaled to fit in the interval from -1 to 1, and the input
acts as an index into the table.
<p>
The <i>interpolation</i> parameter determines the output for input
values between table-entry points.  If <i>interpolation</i> is
"linear", the star will interpolate between table entries; if
<i>interpolation</i> is "none", it will use the next lowest entry.
With a two-element table, for instance, the first constant
specifies the output for an input of -1, while the second handles
an input of 0.  With no interpolation, all inputs less than 0 will
result in an output of the first constant.
<p>
Since the table specifies outputs only for inputs up to +1 - 2/n,
special provisions must be made for inputs between that point and +1.
With <i>interpolation</i> set to "none", inputs in that range result
in an output of the last table constant.  If  <i>interpolation</i>
is "linear", the behavior of the function is specified by
<i>tableType</i>, which determines the effective table entry for an
input of 1.0.  "periodic" sets the entry equal to the entry for -1,
allowing smoothly wrapping periodic functions.  For "limited", it
is equal to the last constant.  "linear" sets it to twice
the last constant minus the previous one, linearly extending the
table.  Any other value results in a value of 0 for an input of 1.
<p>
As an example, the table [1 2 3 5] with linear interpolation returns
the following input-output pairs: -1=&gt;1; -.75=&gt;1.5; -.5=&gt; 2; -.25=&gt;2.5;
0=&gt;3; .25=&gt;4; .5=&gt;5.  An input of .75 gives an output of 3 for periodic
table type, 5 for limited, 6 for linear, and 2.5 for any other type.
<p>
The <i>tableType</i> feature is in fact implemented by tacking an extra
value onto the end of the table.  Thus, the coefficient table will require
one more memory location if <i>interpolation</i> is "linear".
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
		attributes { A_UMEM|A_CONSEC }
	}
	state {
		name { addedVal }
		type { fix }
		desc { internal }
		default { 0 }
		attributes { A_NONCONSTANT|A_NONSETTABLE|A_BMEM }
	}
	state { 
		name { coefNum }
		type { int }
		desc { Number of coefficients. }
		default { 2 }
		attributes { A_NONCONSTANT|A_NONSETTABLE }
	}
	setup {
		coefNum = coef.size();
	}
	ccinclude { <string.h> }
	constructor {
		noInternalState();
	}
	initCode {
		const char* tabletype = tableType;
		const char* interptype = interpolation;
		if (strcasecmp(interptype, "linear") == 0) {
		    if (strcasecmp(tabletype, "periodic") == 0)
			addedVal = coef[0];
		    else if (strcasecmp(tabletype, "limited") == 0)
			addedVal=coef[int(coefNum)-1];
		    else if (strcasecmp(tabletype, "linear") == 0)
			addedVal = 2*coef[int(coefNum) - 1] -
				   coef[int(coefNum) - 2];

		}
		else if ( strcasecmp(interptype, "none") != 0 ) {
		    Error::abortRun(*this,
				    "Unrecognized interpolation type '",
				    interptype,
				    "': should be either linear or none");
		}
	}
	go {
		const char* q = interpolation;
		switch (q[0]) {
		  case 'n':
		  case 'N':    	// none
			addCode(none);
			break;
		  default:	// linear
			addCode(other);
			return;
		}
	}
	

	codeblock (none) {
	mar 	*,AR6				;
	lar	AR6,#$addr(input)		;Address input		=> AR6
	lar	AR7,#$addr(output)		;Address output		=> AR7
 	lacl   	#$val(coefNum)			;Accu = number of coef
	lt	*,AR7			 	;TREG0 = input
	mpy     #$val(coefNum)			;P-Reg = input*coefNum
	apac					;Accu = coefNum*(1+input)
	sfr			     		;1 shift right
	add   	#$addr(coef),0			;Accu = Accu + Address coef
	samm	BMAR				;store Accu in BMAR
	bldd	BMAR,*				;output = value at address BMAR
  	}

	codeblock(other) {
	mar 	*,AR6				;
	lar	AR6,#$addr(input)		;Address input		=> AR6
	lar	AR7,#$addr(output)		;Address output		=> AR7
 	lacl   	#$val(coefNum)			;Accu = number of coef
	lt	*,AR0			 	;TREG0 = input
	mpy     #$val(coefNum)			;P-Reg = input*coefNum
	apac					;Accu = coefNum*(1+input)
	sfr			     		;1 shift right
	add   	#$addr(coef),0			;Accu = Accu + Address coef
	samm	AR0				;store Accu in AR0
	rpt 	#15				; 
	 sfl					;16 shifts left(value in acch)
	lacl	#80h				;Accu = acch and 0080h in accl
	add	#40h				;Accu = Accu + 0040h
	neg					;Accu = - Accu
	samm	TREG0				;TREG0 = - Accu
	mpy	*+				;P-Reg =
	pac
	samm	TREG0
	mpy	*,AR7
	apac
	sach	*,1
	}

	exectime {
		const char* i = interpolation;  
		if (i[0]=='n' || i[0]=='N') return 9;
		else return 21;
	}
}
