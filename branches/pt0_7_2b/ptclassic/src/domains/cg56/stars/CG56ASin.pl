defstar {
	name { ASin  }
	domain { CG56 }
	desc { Arc Cosine }
	version { $Id$ }
	author { Chih-Tsung, ported from Gabriel }
	copyright { 1992 The Regents of the University of California }
	location { CG56 demo library }
	explanation {
Arc Sin function using power series approximation.  
The output, in principal range -pi/2 to pi/2, is scaled down by pi.
	}
	execTime {
		return 55;
	}
	input {
		name {input}
		type {FIX}
	}
	output {
		name {output}
		type {FIX}
	}
	codeblock (asinblock) {
        clr     a               $ref(input),x0
        cmp     x0,a            #.999999999,b
        jne     l1
	jmp	l5
l1
        cmp     x0,b            #-1.0,a
        jne     l2
	move	#0.5,a
	jmp 	l5
l2
	cmp	x0,a
	jne	l3
	move	#-0.5,a
	jmp	15
l3
        mpyr    x0,x0,b         #.445156695,y1
        move    #0.5,a          b,y0
        mac     y1,y0,a         #.440833333,y1
        mpyr    y0,y1,b         a,x1
        move    #0.25,a         b,y1
        mac     y1,y0,a         #.435770751,y1
        mpyr    y0,y1,b         a,x1
        move    #0.125,a        b,y1
        mac     y1,y0,a         #.859523809,y1
        mpyr    y0,y1,b         a,x1
        move    #0.125,a        b,y1
        mac     y1,y0,a         #.845029239,y1
        mpyr    y0,y1,b         a,x1
        move    #0.125,a        b,y1
        mac     y1,y0,a         #.827205882,y1
        mpyr    y0,y1,b         a,x1
        move    #0.125,a        b,y1
        mac     y1,y0,a         #.804761904,y1
        mpyr    y0,y1,b         a,x1
        move    #0.125,a        b,y1
        mac     y1,y0,a         #.775641025,y1
        mpyr    y0,y1,b         a,x1
        move    #0.125,a        b,y1
        mac     y1,y0,a         #.736363636,y1
        mpyr    y0,y1,b         a,x1
        move    #0.125,a        b,y1
        mac     y1,y0,a         #.680555555,y1
        mpyr    y0,y1,b         a,x1
        move    #0.125,a        b,y1
        mac     x1,y1,a         #.595238095,y1
        mpyr    y0,y1,b         a,x1
        move    #0.125,a        b,y1
        mac     x1,y1,a         #.450000000,y1
        mpyr    y0,y1,b         a,x1
        move    #0.125,a        b,y1
        mac     x1,y1,a         #.166666666,y1
        mpyr    y0,y1,b         a,x1
        move    #0.125,a        b,y1
        macr    x1,y1,a         #0.318309886,y1
        move    a,x1
        mpyr    x0,x1,a
        move    a,x1
        mpyr    x1,y1,a

        dup     3
        asl     a
        endm
l5
         move    a,$ref(output)
	}
	go {
		gencode(asinblock);
	}
}


