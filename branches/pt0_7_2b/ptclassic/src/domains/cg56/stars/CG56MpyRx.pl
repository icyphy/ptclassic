defstar {
    name { MpyRx }
    domain { CG56 }
    desc { Multiply any number of rectangular complex inputs, producing an output. }
    version { $Id$ }
    author { Kennard White (ported from Gabriel) }
    copyright {
Copyright (c) 1990, 1991, 1992 The Regents of the University of California.
All rights reserved.
See the file ~ptolemy/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
    }
    location { CG56 arithmetic library }
    explanation {
.Id "multiplication"
The inputs are multiplied and the result is written on the output.
    }
    inmulti {
	name {input_r}
	type {FIX}
    }
    inmulti {
	name {input_i}
	type {FIX}
    }
    output {
	name {output_r}
	type {FIX}
    }
    output {
	name {output_i}
	type {FIX}
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
	move			$ref(input_r#1),x0
	move			$ref(input_r#2),y0
	mpyr	x0,y0,a		$ref(input_i#1),x1
	mpyr	x1,y0,b		$ref(input_i#2),y1
	macr	-x1,y1,a
	macr	x0,y1,b		a,$ref(output_r)
	move			b,$ref(output_i)
    }
    protected {
	int	runtime;
    }
    start {
	int n = input_r.numberPorts();
	if ( n != input_i.numberPorts() ) {
	    Error::abortRun(*this,"must have same number of real & imag input ports");
	    return;
	}
	switch ( n ) {
	case 0:
	    runtime = 2;
	    break;
	case 1:
	    char buf[256];
	    sprintf( buf, "input_r#%d", 1); // this is absurd
	    CGPortHole *pr = (CGPortHole*) genPortWithName( buf);
	    forkInit( *pr, output_r);
	    sprintf( buf, "input_i#%d", 1); // this is absurd
	    CGPortHole *pi = (CGPortHole*) genPortWithName( buf);
	    forkInit( *pi, output_i);
	    runtime = 0;
	    break;
	case 2:
	    runtime = 7;
	    break;
	default:
	    Error::abortRun(*this,"more than two inputs not implemented yet");
	    return;
	}
    }
    go {
	int n = input_r.numberPorts();
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
