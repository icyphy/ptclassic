defstar {
	name { ACos  }
	domain { CG56 }
	desc { Arc Cosine }
	version { $Id$ }
	author { Chih-Tsung, ported from Gabriel }
	copyright {
Copyright (c) 1990, 1991, 1992 The Regents of the University of California.
All rights reserved.
See the file ~ptolemy/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { CG56 nonlinear functions library }
	explanation {
.PP
.Id "arccosine"
.Id "cosine, inverse"
.Id "inverse cosine"
.Ir "power series"
A power series expansion is used to compute the arc cosine of the input,
which is in the range -1.0 to 1.0.  The output is in the principle range of
0 to pi, scaled down by a factor of pi for the fixed point device.
.PP
The power series is factorized carefully so that intermediate factors never
exceed 1.0 in magnitude.  This is essential because these intermediate results
have to be moved from the accumulator back to the \fIx\fR or \fIy\fR
data registers for subsequent multiplication.
.PP
The runtime of this star can vary from a few instruction cycles, if
the input is close to certain values such as +/-0.5 and +/-1.0, to some 55
instructions.  This is because the input value is first tested against certain
particular values to which handy cosine values are available.  Failing that,
the power series expansion is used.  This approach speeds up the runtime
execution for a small number of cases but may have serious impact on parallel
scheduling.  It may thus be desirable to remove the initial boundary value
checks.
.PP
The runtime can be improved by reducing the order of the power series
expansion, at the expense of precision.
	}
        seealso { ASin, Cos, Sin }
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
	codeblock (acosblock) {
        clr     a               $ref(input),x0
        cmp     x0,a            #.999999999,b
        jne     $label(l22)
        move    #0.5,a
        jmp     $label(l23)
$label(l22)
        cmp     x0,b            #-1.0,a
        jne     $label(l24)
        clr     a
        jmp     $label(l23)
$label(l24)
        cmp     x0,a
        jne     $label(l25)
        move    #.99999999,a
        jmp     $label(l23)
$label(l25)
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

        neg     a               #0.5,x0
        add     x0,a

$label(l23)
        move    a,$ref(output)
	}
	go {
		addCode(acosblock);
	}
}



