defstar {
	name { Rect }
	domain { CG56 }
	desc { Rectangular Pulse Generator }
	version { $Id$ }
	author { Chih-Tsung Huang }
	copyright { 1992 The Regents of the University of California }
	location { CG56 demo library }
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
                attributes {A_YMEM|A_NONCONSTANT|A_NONSETTABLE}
        }
        state {
                name {durationCounter}
	        type {int}
	        desc { internal }
	        default { 0 }
                attributes {A_YMEM|A_NONCONSTANT|A_NONSETTABLE}
        }
        state {
                name {amp}
	        type {fix}
	        desc { internal }
	        default { 0 }
                attributes {A_YMEM|A_NONCONSTANT|A_NONSETTABLE}
        }

        codeblock(main) {
        move    $ref(durationCounter),a
        move	#$val(width),b
        cmp	a,b
        jneq	<$label(val)
        move	$ref(periodCounter),a
        move    #$val(period)-1,b
        cmp     a,b
        jeq	<$label(restart)
        move    #1,x0
        add     x0,a
        move	a,$ref(periodCounter)
        jmp	<$label(zero)
$label(val)
        move    #1,x0
        add     x0,a
 	move	a,$ref(durationCounter)
        move    $ref(periodCounter),b
        move    #1,x0
        add     x0,b
        move	b,$ref(periodCounter)
        move    $ref(amp),a
        move    a,$ref(output)
        jmp     <$label(end)
$label(restart)
        clr     a
        move	a,$ref(durationCounter)
        move    a,$ref(periodCounter)
$label(zero)
        clr     a
        move    a,$ref(output)
$label(end)
        }

        start {
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
                gencode(main);
	}
	execTime {
		return 26;
	}

}
