defstar {
	name { Sin }
	domain { C50 }
	desc {
Sine function calculated by table lookup.  Input range of [-1,1) is scaled
by pi.  Output is sin(pi*input).
	}
	version {@(#)C50Sin.pl	1.11	05/26/98}
	acknowledge { Gabriel version by Maureen O'Reilly }
	author { Luis Gutierrez, G. Arslan }
	copyright {
Copyright (c) 1990-1998 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { C50 main library }
	htmldoc {
<a name="sin"> </a>
This star computes the sine of the input, which must be in the range
[-1.0, 1.0).  The output equals sin(<i>pi</i> &#183 <i>in</i> &#43 <i>phase</i>),
so the input range is effectively [-<i> pi</i>, <i>pi </i>).  The output is in
the range [-1.0, 1.0).  The parameter <i>phase</i> is in degrees
(e.g., cosine would use a phase of 90).
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
		type {float}
		default { "0" }
		desc { "Additive phase relative to the sin() function." }
	}
	state {
		name { tbl }
		type { fixarray }
		default { "0[129]" }
		desc { internal state to hold sin table }
		attributes { A_UMEM|A_CIRC|A_NONSETTABLE}
	}

	protected{
		int	phaseAsInt;
		StringList sinTable;
		int	C50SinTable;
	}
 
	code {
	#include "TITarget.h"
	}

	method{
		name { setC50SinTable }
		type { "void" }
		arglist { "()" }
		access { protected }
		code {

		  TITarget * myTarget = (TITarget *) this->target();
		  if (myTarget->testFlag("sinTableFlag")) {
			C50SinTable = FALSE;
		  } else {
			C50SinTable = TRUE;
			myTarget->setFlag("sinTableFlag ");
		  }
		

		}
	}
	

	begin {
//FIXME The table is not allocated in both cases if the following comment
//      line will be activated. In this case the table is allocated every 
//      time the function is called. 


                setC50SinTable();
		
		if (C50SinTable) {
			tbl.resize(129);
		} else {
//tbl.clearAttributes(A_UMEM);
			tbl.clearAttributes(A_SHARED);
		}
	}

	setup {
		int tempPhase;
		double temp;
		
		tempPhase = int(double(phase));

	// first make phase fall in range of [-180,180)

		if (tempPhase < 0) {
		    tempPhase = -tempPhase;
		    tempPhase = tempPhase % 360;
		    if (tempPhase > 180) {
			tempPhase = 360 - tempPhase;
		    } else tempPhase = -tempPhase;
		} else {
		    tempPhase = tempPhase % 360;
		    if (tempPhase >= 180 ) {
			tempPhase = tempPhase - 360;
		    }
		}
		 
	// now find integer representation 

		temp = double(tempPhase)/180;

		if (temp >= 0) {
			phaseAsInt = int(double(32768*temp) + 0.5);
		} else {
			phaseAsInt = int(double(32768*(2+temp)) + 0.5);
		}
	}	



	initCode {

	//add sin table only if it has not been added yet
        //initialization is not working ! so sin and cos cannot be used !
 
	
	if (C50SinTable) {
	
		sinTable.initialize();
		double value;
		double floor;
		floor = double(1/32768); // floor = smallest pos. value
		
		for (int i = 0; i< 128; i++){
			value = sin(double(i*M_PI/128));
			if ((value < floor) && (value > -floor)) 
			    sinTable<<"0.0 ";
			else if (value > C50_ONE )
			    sinTable<<"0.999999999 ";
			else {
			    sinTable << value << " ";
			}
		}
		sinTable<<"0.0 ";
		tbl.setInitValue(sinTable);
	}

	}

	codeblock(cbmain,""){
	lar	ar1,#$addr(input)	; ar1-> input
	lar	ar2,#13h		; ar2->ar3
	mar	*,ar1			; 
	lacc	*,15,ar2		; acc = input
	add	#@(int(phaseAsInt)),15  ; add phase(wrapping around in case of
	sach	*,1			; overflow); store result in ar3
	setc	ovm			; set overflow mode for abs
	sfl				; shift acc left 1 bit
	abs				; acc = abs(input) with saturation
	bsar	9			; get 7 MSB in accH
	add	#1,14			; round up
	add	#$addr(tbl),15	        ; add base address of table
	bit	*,0			; test to see if input is negative
	sach	*,1,ar3			; ar3 = address of entry on table
	clrc	ovm			; unset overflow mode
	lar	ar1,#$addr(output)
	lacc	*,16,ar1		; acc = entry from table
	xc	1,TC			; if input is negative
	neg				; negate acc
	sach	*			; output result
	}


	go {
		addCode(cbmain());
	}
	execTime {
		return 19;
	}
}







