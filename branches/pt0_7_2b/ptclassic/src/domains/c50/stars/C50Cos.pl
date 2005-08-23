defstar {
	name { Cos }
	domain { C50 }
	desc { 
Cosine function.
Calculation by table lookup.  Input range of (-1,1) scaled by pi.
	}
	version { $Id$ }
	acknowledge { Gabriel version by Maureen O'Reilly }
	author { A. Baensch,  ported from Gabriel }
	copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { C50 nonlinear functions library }
	explanation {
.Id "cosine"
This star computes the cosine of the input, which must be in the range
(-1.0, 1.0).
The output equals cos($~pi~cdot~$\fIin\fR), so the input range is 
effectively (-$~pi$, $pi~$).
The output is in the range (-1.0, 1.0).
	}
	input {
		name {input}
		type {FIX}
	}
	output {
		name {output}
		type {FIX}
	}
	codeblock (CosTable) {
	.ds     02800h
	.q15    -1.0,-0.995185,-0.980785,-0.95694,-0.9239,-0.881921,-0.8315,-0.77301
	.q15    -0.7071,-0.63439,-0.5556,-0.4714,-0.3827,-0.290285,-0.1951,-0.09802
	.q15    0,0.09802,0.1951,0.290285,0.3827,0.4714,0.5556,0.63439
	.q15    0.7071,0.77301,0.8315,0.881921,0.9239,0.95694,0.980785,0.995185
	.q15    0.9999999,0.995185,0.980785,0.95694,0.9239,0.881921,0.8315,0.77301
	.q15    0.7071,0.63439,0.5556,0.4714,0.3827,0.290285,0.1951,0.09802
	.q15    0,-0.09802,-0.1951,-0.290285,-0.3827,-0.4714,-0.5556,-0.63439
	.q15    -0.7071,-0.77301,-0.8315,-0.881921,-0.9239,-0.95694,-0.980785
	.text
	}
 	codeblock (main) {
	mar     *,AR0
	lar     AR0,#$addr(input)	;Address input		=> AR0
	lar     AR7,#$addr(output)	;Address output		=> AR7
	bit     *,0                     ;bit 15 = 1 in input ?
	lacc    *                       ;Accu = input (x)
	and     #0fb00h			;normalize input value for table (x1)
	samm    INDX    		;store Accu in INDX
	bsar    10			;shift Accu 10 bits right
	nop				;
	xc      2,TC			;if TC=1 (bit15=1 in input) then
	 add     #027e0h,0		;Accu=Accu+startaddr(negative table)
	 nop				;
	xc      2,NTC			;if TC=0 (bit15=0 in input) then
	 add    #02820h,0		;Accu=Accu+startaddr(positive table)
	 nop				;
	samm    AR1			;store Accu in AR1
	lacc    *,15,AR1		;Accu = x
	sub     INDX,15			;Accu = x - x1
	sach    TREG0,1			;TREGO = x - x1
	lacc    *+,15			;Accu = value from table (y1) incr AR1
	neg				;Accu = -y1
	add     *-,15			;Accu = y2 - y1
	sach    INDX,1			;INDX = y2 - y1
	mpy     INDX			;P-Reg = (x - x1)*(y2 - y1)
	pac				;Accu = P-Reg
	bsar    11			;shift Accu 11 bits right (division 2)
	add     *,0,AR7			;Accu = y1 + (x - x1)*(y2 - y1)/2
	sacl    *			;ouput = Accu
	}
	initCode {
		addCode(CosTable);
	}
	go {
		addCode(main);
	}
	execTime {
		return 28;
	}
}
