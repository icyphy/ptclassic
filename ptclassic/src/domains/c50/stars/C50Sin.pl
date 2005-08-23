defstar {
	name { Sin }
	domain { C50 }
	desc {
Sine function.  Input range of (-1,1) scaled by pi.  Output is sin(pi*input).
	}
	version { $Id$ }
	acknowledge { Gabriel version by Maureen O'Reilly }
	author { A. Baensch }
	copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { C50 nonlinear functions library }
	explanation {
This star computes the sine of the input, which must be in the range
(-1.0, 1.0).
The output equals sin($~pi~cdot~$\fIin\fR$+\fIphase\fR$),
so the input range is effectively (-$~pi$, $pi~$).
The output is in the range (-1.0, 1.0).
The parameter \fIphase\fR is in degrees (e.g., cos() would use phase=90).
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
		name {phase}
		type {FLOAT}
		default { "0" }
		desc { "Additive phase relative to the sin() function." }
	}
	state {
		name {phaseOffset}
		type {INT}
		default { "0" }
		desc { "Munged form of phase." }
		attributes { A_NONSETTABLE }
	}
	codeblock (SinTable) {
	.ds     02900h
	.q15    0,-0.09802,-0.1951,-0.290285,-0.3827,-0.4714,-0.5556,-0.63439
	.q15    -0.7071,-0.77301,-0.8315,-0.881921,-0.9239,-0.95694,-0.980785
		.q15    -1.0,-0.995185,-0.980785,-0.95694,-0.9239,-0.881921,-0.8315,-0.77301
	.q15    -0.7071,-0.63439,-0.5556,-0.4714,-0.3827,-0.290285,-0.1951,-0.09802
	.q15    0,0.09802,0.1951,0.290285,0.3827,0.4714,0.5556,0.63439
	.q15    0.7071,0.77301,0.8315,0.881921,0.9239,0.95694,0.980785,0.995185
	.q15    0.9999999,0.995185,0.980785,0.95694,0.9239,0.881921,0.8315,0.77301
	.q15    0.7071,0.63439,0.5556,0.4714,0.3827,0.290285,0.1951,0.09802
	.text
	}
 	codeblock (cbmain) {
	mar     *,AR0			;
	lar     AR0,#$addr(input)	;Address input		=> AR0
	lar     AR7,#$addr(output)	;Address output		=> AR1
	bit     *                       ;Bit 15 = 1 in input (negativ value) ?
	lacc    *                       ;Accu =  input (x)  
	and     #0fb00h			;normalize input value for table (x1)
	samm    INDX    		;store Accu in INDX
	bsar    10			;shift Accu 10 bits right
	nop				;
	xc      2,TC			;if TC=1 (bit15=1 in input) then
	 add     #028e0h,0		;Accu = Accu +startaddr(negative table)
	 nop				;
	xc      2,NTC			;if TC=0 (bit15=0 in input) then
	 add    #02920h,0		;Accu = Accu +startaddr(positive table)
	 nop				;
	samm    AR1			;store Accu in AR1
	lacc    *,15,AR1		;Accu = x
	sub     INDX,15			;Accu = x- x1
	sach    TREG0,1			;TREG0 = x - x1
	lacc    *+,15			;Accu =value from table (y1)  incr. AR1
	neg				;Accu = -y1
	add     *-,15			;Accu = y2 - y1
	sach    INDX,1			;INDX = y2 - y1
	mpy     INDX			;P-Reg = (x - x1)*(y2 - y1)
	pac				;Accu = P-Reg
	bsar    11			;shift Accu 11 bits right (division 2)
	add     *,0,AR7			;Accu = y1 + (x - x1)*(y2 - y1)/2
	sacl    *			;output = Accu
	}
 
	code {
		// fn to round to nearest int.  Done this way because
		// truncation of negative values to integers is machine
		// dependent in C/C++.
		inline int round(double x) {
			return (x >= 0) ? int(x+0.5) : -int(0.5-x);
		}
	}
	setup {
		double ph = double(phase);
		if ( ph < 0 ) {
			int n = (int) floor(-ph / 360.0);
			ph += 360.0 * (n+1);
		} else if ( ph >= 360.0 ) {
			int n = (int) floor(ph / 360.0);
			ph -= 360.0 * n;
		}
		phaseOffset = round(ph/360.0*6.28319);
	}
	initCode {
		addCode(SinTable);
	}
	go {
		addCode(cbmain);
	}
	execTime {
		return 28;
	}
}
