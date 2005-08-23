defstar {
    name {SubCx}
    domain {CGC}
    desc { Output the "pos" input minus all "neg" inputs. }
    version { @(#)CGCSubCx.pl	1.1	2/20/96 }
    author { Jose Luis Pino }
    copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
    }
    location { CGC main library }
    input {
	name {pos}
	type {complex}
    }
    inmulti {
	name {neg}
	type {complex}
    }
    output {
	name {output}
	type {complex}
    }
    constructor {
	noInternalState();
    }

    codeblock(startOp) {
	$ref(output).real = $ref(pos).real;
	$ref(output).imag = $ref(pos).imag;
    }

    codeblock(doOp,"int i") {
	$ref(output).real -= $ref(neg#@i).real;
	$ref(output).imag -= $ref(neg#@i).imag;
    }

    go { 
	addCode(startOp);
	for (int i = 1; i <= neg.numberPorts(); i++) 
	    addCode(doOp(i)); 
    }

    exectime {
	return 2*(1 + neg.numberPorts());
    }
}
