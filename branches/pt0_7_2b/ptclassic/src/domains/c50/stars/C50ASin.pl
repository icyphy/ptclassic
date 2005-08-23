defstar {
	name { ASin  }
	domain { C50 }
	desc { Arc Sine }
	version { $Id$ }
	author { A. Baensch , ported from Gabriel }
	copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { C50 nonlinear functions library }
	explanation {
.Id "arcsine"
.Id "sine, inverse"
.Id "inverse sine"
The output, in principal range -pi/2 to pi/2, is scaled down by pi.
}
	seealso { ACos, Cos, Sin }
	execTime {
		return 47;
	}
	input {
		name {input}
		type {FIX}
	}
	output {
		name {output}
		type {FIX}
	}
	codeblock (ASinTable) {
	.ds	02b00h
	.q15	-0.5,-0.420214,-0.386866,-0.361068,-0.339139,-0.319657,0.301894,0.285418
	.q15	-0.269947,-0.255285,-0.241292,-0.227858,-0.214901,-0.202353,-0.19016,-0.178278
	.q15	-0.166667,-0.155296,-0.144136,-0.133164,-0.122357,-0.111697,-0.101166,-0.090749
	.q15	-0.080431,-0.070198,-0.060039,-0.049941,-0.039893,-0.029885,-0.019907,-0.009949
	.q15	0,0.009949,0.019907,0.029885,0.039893,0.049941,0.060039,0.070198
	.q15	0.080431,0.090749,0.101166,0.111697,0.122357,0.133164,0.144136,0.155296
	.q15	0.166667,0.178278,0.19016,0.202353,0.214901,0.227858,0.241292,0.255285
	.q15	0.269947,0.285418,0.301894,0.319657,0.339139,0.361068,0.386866,0.420214,0.5
	}
	codeblock (asinblock) {
	mar     *,AR0
	lar     AR0,#$addr(input)	;Address input		=> AR0
	lar     AR7,#$addr(output)	;Address output		=> AR7
	bitt    *                       ;Bit 15 = 1 in input ?
	lacc    *                       ;Accu = input 
	and     #0fb00h			;normalize input value for table (x1)
	samm    INDX    		;store Accu in INDX
	bsar    10			;shift Accu 10 bits right
	nop				;
	xc      2,TC			;if TC=1 (bit15=1 in input) then
	 add     #02ae0h,0		;Accu = Accu+startaddr(negative table)
	 nop				;
	xc      2,NTC			;if TC=0 (bit15=0 in input) then
	 add    #02b20h,0		;Accu = Accu+startaddr(positive table)
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
	initCode {
		addCode(ASinTable);
	}
	go {
		addCode(asinblock);
	}
	execTime {
		 return 28;
	}
}



