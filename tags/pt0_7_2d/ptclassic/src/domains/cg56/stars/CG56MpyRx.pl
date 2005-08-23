defstar {
    name { MpyRx }
    domain { CG56 }
    desc {
Multiply any number of rectangular complex inputs, producing an output.
    }
    version { @(#)CG56MpyRx.pl	1.9	01 Oct 1996 }
    author { Kennard White (ported from Gabriel) }
    copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
    }
    location { CG56 main library }
	htmldoc {
<a name="multiplication"></a>
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
    constructor {
	noInternalState();
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
    setup {
	int n = input_r.numberPorts();
	if ( n != input_i.numberPorts() ) {
	    Error::abortRun(*this,
			    "must have same number of real & imag input ports");
	    return;
	}
	switch ( n ) {
	  case 0:
	    runtime = 2;
	    break;
	  case 1:
            {    
	    CGPortHole* pr = (CGPortHole*) genPortWithName("input_r#1");
	    forkInit(*pr, output_r);
	    CGPortHole *pi = (CGPortHole*) genPortWithName("input_i#1");
	    forkInit( *pi, output_i);
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
	int n = input_r.numberPorts();
	switch ( n ) {
	case 0:
	    addCode(cbZero);
	    break;
	case 1:
	    ; /* forked */
	    break;
	case 2:
	    addCode(cbTwoInput);
	    break;
	default:
	    ; /* not implemented */
	}
    }
    exectime {
        return runtime;
    }
}
