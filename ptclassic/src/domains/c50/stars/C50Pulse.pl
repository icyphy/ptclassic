defstar {
	name { Pulse }
	domain { C50 }
	desc { Pulse generator }
	version { $Id$ }
	author { Luis Gutierrez, based on CG56 version }
	copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { C50 main library }
	htmldoc {
<a name="pulse generator"></a>
A variable length pulse generator.
A pulse begins when a non-zero trigger is received.
The pulse duration varies between 1 and <i>maxDuration</i>
as the control varies between -1.0 and 1.0.
	}

        input  {
                name { trigger }
	        type { fix }
	}
	input  { 
		name { duration }
          	type { fix }
	}
        output {
		name { output }
		type { fix }
	}
        state  {
                name { counter }
		type { fix }
		desc { internal }
		default { 0.0 }
                attributes { A_NONCONSTANT|A_NONSETTABLE|A_UMEM }
        }
        state  {
                name { onVal }
		type { fix }
		desc { maximum val }
		default { ONE }
        }
        state  {
                name { offVal }
		type { fix }
		desc { minimum }
		default { 0.0 }
        }
        state  {
                name { maxDuration }
		type { int }
		desc { maximum Duration }
		default { 10 }
        }
	protected{
		unsigned int iOnVal, iOffVal;
	}


	setup {
		double temp = onVal.asDouble();
		if (temp >= 0)	{
			iOnVal = int(32768*temp + 0.5);
		} else {
			iOnVal = int(32768*(2+temp) + 0.5);
		}

		temp = offVal.asDouble();

		if ( temp >= 0) {
			iOffVal = int(32768*temp + 0.5);
		}else{
			iOffVal = int(32768*(2+temp) + 0.5);
		}
	}
	
	codeblock(std,""){
	lar	ar3,#$addr(trigger)	; ar3-> input
	lar	ar0,#$addr(counter)	; ar0-> counter
	mar	*,ar3			; arp = 3
	lacl	*,ar0			; load acc low with input, high acc = 0
	setc	sxm			; set sign extension mode
	xc	1,neq			; if non-zero input clear counter
	sach	*,0
	lacc	*,15,ar1		; load acc with counter/2
	sacb				; accb = counter/2
	lar	ar1,#$addr(duration)	; ar1 -> duration
	lacc	*,0,ar0			; load acc with duration/2
	add	#4000h,1		; acc = 0.5*(duration + 1)
	sfr				; shift acc right for nxt inst
	samm	treg0			; treg0 = 0.5*(duration+1)
	mpy	#$val(maxDuration)	; p = 0.5(maxDur*(duration+1)/2)
	pac				; acc = p = newLength/2
	crlt				
* duration<counter ? C=1 : C=0 and lesser value stored in acc, accb
	lar	ar2,#@iOnVal		; if counter <= newLength outpu onVal 
@(  (iOffVal > 255 ) ? "\txc\t2,C\n":"\txc\t1,C\n")\
	lar	ar2,#@iOffVal		; if counter > newLength output offVal
	smmr	ar2,#$addr(output)	; output result
	add	#0001,15		; increment counter
	sach	*,1			; store counter
	}

        go {
                addCode(std());
        }		

	execTime { 
                 return 24;
	}
}




