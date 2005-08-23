defstar {
	name { ACos  }
	domain { C50 }
	desc { Arc Cosine }
	version { @(#)C50ACos.pl	1.4	02/03/99 }
	author { A. Baensch and G. Arslan }
	acknowledge { based on the ACos Motorola 56000 star }
	copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { C50 main library }
	htmldoc {
<p>
<a name="arccosine"></a>
<a name="cosine, inverse"></a>
<a name="inverse cosine"></a>
The input is in the range -1.0 to 1.0.  The output is in the principle range of
0 to pi, scaled down by a factor of pi for the fixed point device.
	}
        seealso { ASin, Cos, Sin }

	input {
		name {input}
		type {FIX}
	}                                           
	output {
		name {output}
		type {FIX}
	}

	state {
	  name {ACosTable}
	  type {fixarray}
	  default{ "0[73]" }
	  attributes {A_UMEM | A_NONSETTABLE | A_CONSTANT | A_SHARED}
	}

	setup{
	  StringList init;
	  init << "0.999999 0.920214 0.886866 0.861068 0.839139 0.819657 0.801894 0.785418"
	       << "0.769947 0.755285 0.741292 0.727858 0.714901 0.702353 0.69016 0.678278"
	       << "0.666667 0.655296 0.644139 0.633164 0.622357 0.611697 0.601166 0.590749"
	       << "0.666667 0.655296 0.644139 0.633164 0.622357 0.611697 0.601166 0.590749"
	       << "0.580431 0.570198 0.560039 0.549941 0.539893 0.529885 0.519907 0.509949"
	       << "0.5 0.490051 0.480093 0.470115 0.460107 0.450059 0.439961 0.429802"
	       << "0.419569 0.409251 0.398834 0.388303 0.377643 0.366836 0.355864 0.344704"
	       << "0.333333 0.321722 0.30984 0.297647 0.285099 0.272417 0.258708 0.244715"
	       << "0.230053 0.214582 0.198106 0.180343 0.16086 0.138932 0.113134 0.079786 0";
	  
	  ACosTable.resize(73);
	  ACosTable.setInitValue(init);
	}

	//FIXME: Operand missing error on line bit * !
	codeblock (acosblock) {
	mar     *,AR0
	lar     AR0,#$addr(input)	;Address input		=> AR0
	lar     AR7,#$addr(output)	;Address output		=> AR7
	bit     *                    	;Bit 15 = 1 in input (negative value) ?
	lacc    *                       ;Accu = input
	and     #0fb00h			;normalize input value for table
	samm    INDX    		;store Accu in INDX
	bsar    10			;shift Accu 10 bits right
	nop				;
	xc      2,TC			;if TC=1 (bit15=1 in input) then
	;WHY pointer - 32 ????!!!!		   	
	 add    #($addr(ACosTable)-32),0	;Accu =Accu+startaddr.(negative table)
	 nop				;
	xc      2,NTC			;if TC=0 (bit15=0 in input) then
	;WHY pointer + 32 ????!!!!
	 add    #($addr(ACosTable) +32),0 ;Accu =Accu+startaddr.(positive table) 
	 nop				;
	samm    AR1			;store Accu in AR1
	lacc    *,15,AR1		;Accu = input (x)
	sub     INDX,15			;Accu = input (x) - norm. input (x1) 
	sach    TREG0,1			;TREG0 = Accu
	lacc    *+,15			;Accu = value from table (y1) incr AR1
	neg				;Accu = -y1
	add     *-,15			;Accu = y2 - y1
	sach    INDX,1			;INDX = y2 - y1
	mpy     INDX			;P-Reg= (x - x1)*(y2 - y1)
	pac				;Accu = P-Reg
	bsar    11			;shift Accu 11 bits right (division 2)
	add     *,0,AR7			;Accu = y1 + (x-x1)*(y2-y1)/2
	sacl    *			;output = Accu
	}

	constructor {
		noInternalState();
	}
	go {
		addCode(acosblock);
	}
	execTime { 
		return 27;
	}
}
