defstar {
	name { ASin  }
	domain { C50 }
	desc { Arc Sine }
	version {@(#)C50ASin.pl	1.4	02/03/99}
	author { A. Baensch , ported from Gabriel, G. Arslan }
	copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { C50 main library }
	htmldoc {
<a name="arcsine"></a>
<a name="sine, inverse"></a>
<a name="inverse sine"></a>
The output, in principal range -pi/2 to pi/2, is scaled down by pi.
}
	seealso { ACos, Cos, Sin }
	input {
		name {input}
		type {FIX}
	}
	output {
		name {output}
		type {FIX}
	}
	state {
	  name {ASinTable}
	  type {FIXARRAY}
	  attributes{A_UMEM | A_NONSETTABLE | A_CONSTANT}
	  default{ "0[73]"}
	}

	setup {
	  StringList init;
	  init << "-0.5 -0.420214 -0.386866 -0.361068 -0.339139 -0.319657 0.301894 0.285418"
	       << "-0.269947 -0.255285 -0.241292 -0.227858 -0.214901 -0.202353 -0.19016 -0.178278"
	       << "-0.166667 -0.155296 -0.144136 -0.133164 -0.122357 -0.111697 -0.101166 -0.090749"
	       << "-0.080431 -0.070198 -0.060039 -0.049941 -0.039893 -0.029885 -0.019907 -0.009949"
	       << "0 0.009949 0.019907 0.029885 0.039893 0.049941 0.060039 0.070198"
	       << "0 0.009949 0.019907 0.029885 0.039893 0.049941 0.060039 0.070198"
	       << "0.080431 0.090749 0.101166 0.111697 0.122357 0.133164 0.144136 0.155296"
	       << "0.166667 0.178278 0.19016 0.202353 0.214901 0.227858 0.241292 0.255285"
	       << "0.269947 0.285418 0.301894 0.319657 0.339139 0.361068 0.386866 0.420214 0.5";

	  ASinTable.resize(73);
	  ASinTable.setInitValue(init);
	}

	codeblock (asinblock) {
	mar     *,AR0
	lar     AR0,#$addr(input)	;Address input		=> AR0
	lar     AR7,#$addr(output)	;Address output		=> AR7
	bitt    *			;Bit 15 = 1 in input ?
	lacc    *			;Accu = input 
	and     #0fb00h			;normalize input value for table (x1)
	samm    INDX    		;store Accu in INDX
	bsar    10			;shift Accu 10 bits right
	nop				;
	xc      2,TC			;if TC=1 (bit15=1 in input) then
	 add    #($addr(ASinTable)-32),0 ;Accu = Accu+startaddr(negative table)
	 nop				;
	xc      2,NTC			;if TC=0 (bit15=0 in input) then
	 add    #($addr(ASinTable)+32),0  ;Accu = Accu+startaddr(positive table)
	 nop				;
	samm    AR1			;store Accu in AR1
	lacc    *,15,AR1		;Accu = input (x)
	sub     INDX,15			;Accu = x-x1
	sach    TREG0,1			;TREG0 = x-x1
	lacc    *+,15			;Accu = value from table (y1) incr AR1
	neg				;Accu = -y1
	add     *-,15			;Accu = y2-y1
	sach    INDX,1			;INDX = y2-y1
	mpy     INDX			;P-Reg = (x - x1)*(y2 - y1)
	pac				;Accu = P-Reg
	bsar    11			;shift Accu 11 bits right (division 2)
	add     *,0,AR7			;Accu = y1 + (x-x1)*(y2-y1)/2
	sacl    *			;output = Accu
	}

	constructor {
		noInternalState();
	}
	go {
		addCode(asinblock);
	}
	execTime {
		return 28;
	}
}
