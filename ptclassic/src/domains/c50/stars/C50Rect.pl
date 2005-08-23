defstar {
	name { Rect }
	domain { C50 }
	desc { Rectangular Pulse Generator }
	version { $Id$ }
	author { A. Baensch }
	copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { C50 signal sources library }
	explanation {
Rectangular Pulse generator.
	}
	output {
		name {output}
		type {fix}
	}
        state {
                name {height}
	        type {fix}
	        desc { height of rectangular pulse. }
	        default { ONE }
        }
        state {
                name {width}
	        type {int}
	        desc { width of rectangular pulse. }
	        default { 1 }
        }
        state {
                name {period}
	        type {int}
	        desc { period of pulse }
	        default { 10 }
        }
        state {
                name {periodCounter}
	        type {int}
	        desc { internal }
	        default { 0 }
                attributes {A_UMEM|A_NONCONSTANT|A_NONSETTABLE}
        }
        state {
                name {durationCounter}
	        type {int}
	        desc { internal }
	        default { 0 }
                attributes {A_UMEM|A_NONCONSTANT|A_NONSETTABLE}
        }
        state {
                name {amp}
	        type {fix}
	        desc { internal }
	        default { 0 }
                attributes {A_UMEM|A_NONCONSTANT|A_NONSETTABLE}
        }

        codeblock(main) {
	mar 	*,AR0
        lar	AR0,#$val(durationCounter)	;Address durCount	=>AR0
        lmmr	ARCR,#$val(width)		;width => ARCR
        cmpr	3				;AR0 not even ARCR ?
        bcnd	$label(val),TC			;if yes jump to label(val)
        lar	AR0,#$val(periodCounter)	;periodCounter		=>AR0
        lmmr    ARCR,#$val(period)-1		;period-1 =>ARCR
        cmpr    1				;AR0=ARCR ?
        bcnd	$label(rest),TC			;if yes jump to label(rest)
        adrk    #1.0				;AR0 = AR0 +1
        smmr	AR0,#$addr(periodCounter)	;store AR0 as periodCounter
        b	$label(zero)			;jump to label(zero)
$label(val)
        adrk    #1.0				;AR0= AR0 +1
 	smmr	AR0,#$addr(durationCounter)	;store AR0 as durationCounter
        lar   	AR0,#$val(periodCounter)	;periodCounter		=> AR0
        adrk    #1.0				;AR0 = AR0 +1
        smmr	AR0,#$addr(periodCounter)	;store AR0 as period Counter
        lmmr    AR7,#$addr(amp)			;amp	=> AR7
        smmr    AR7,#$addr(output)		;amp => output
        b     	$label(end)			;jump label(end)
$label(rest)
        splk	#0.0,AR7			;set AR7 to zero
        smmr	AR7,#$addr(durationCounter)	;durationCounter = 0
        smmr    AR7,#$addr(periodCounter)	;periodCounter =0
$label(zero)
        splk	#0.0,AR7			;set AR7 to zero
        smmr    AR7,#$addr(output)		;output =0
$label(end)
        }

        setup {
		 int w=width;
		 int p=period;
		 amp=height;

		 if(w<1) 
		     Error::abortRun(*this, ": Invalid width value.");
		 if(p<1)    
		     Error::abortRun(*this, ": Invalid period value.");
		 if(p<=w)
		     Error::abortRun(*this,
		              "Period must be greater than width");
        }			      
	go {
                addCode(main);
	}
	execTime {
		return 17;
	}

}
