defstar {
    name { MpyCx }
    domain { CG56 }
    desc { Multiply any number of rectangular complex inputs, producing an output. }
    version { $Id$ }
    author { Jose Luis Pino, Kennard White (ported from Gabriel) }
    copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
    }
    location { CG56 arithmetic library }
    explanation {
.Id "multiplication"
The inputs are multiplied and the result is written on the output.
    }
    inmulti {
	name {input}
	type {COMPLEX}
    }
    output {
	name {output}
	type {COMPLEX}
    }
    state  {
	name { inputNum }
	type { int }
	default { 0 }
	desc { input#() }
	attributes { A_NONCONSTANT|A_NONSETTABLE }
    }
    codeblock(cbZero) {
	clr	a
	move	a,$ref(output)
    }
    codeblock(cbTwoInput) {
	move  $ref(input#1),x
	move  $ref(input#2),y
	mpyr	x1,y1,a
	mpyr	x0,y1,b
	macr	-x0,y0,a
	macr	x1,y0,b		a,x:$addr(output)
	move			b,y:$addr(output)
    }
    protected {
	int	runtime;
    }
    setup {
	int n = input.numberPorts();
	switch ( n ) {
	case 0:
	    runtime = 2;
	    break;
	case 1:
            {    
	    const char* buf = "input#1";;
	    CGPortHole *pr = (CGPortHole*) genPortWithName(buf);
	    forkInit( *pr, output);
	    runtime = 0;
	    break;
	    }
	case 2:
	    runtime = 7;
	    break;
	default:
	    Error::abortRun(*this,"more than two inputs not implemented yet");
	    return;
	}
    }
    go {
	int n = input.numberPorts();
	switch ( n ) {
	case 0:
	    addCode(cbZero);
	    break;
	case 1:
	    ; /* fork'd */
	    break;
	case 2:
	    addCode(cbTwoInput);
	    break;
	default:
	    ; // NOT IMP
	}
    }
    exectime {
        return runtime;
    }
}
