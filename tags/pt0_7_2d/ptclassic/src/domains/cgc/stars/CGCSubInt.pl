defstar {
    name {SubInt}
    domain {CGC}
    desc { Output the "pos" input minus all "neg" inputs. }
    version { @(#)CGCSubInt.pl	1.2	2/20/96 }
    author { Brian L. Evans and Jose Luis Pino }
    copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
    }
    location { CGC main library }
    input {
	name {pos}
	type {int}
    }
    inmulti {
	name {neg}
	type {int}
    }
    output {
	name {output}
	type {int}
    }
    constructor {
	noInternalState();
    }
    codeblock(startOp) {
	int diff = $ref(pos);
    }
    codeblock(doOp,"int i") {
	diff -= $ref(neg#@i);
    }
    codeblock(saveResult) {
	$ref(output) = diff;
    }
    go { 
	addCode(startOp);
	for (int i = 1; i <= neg.numberPorts(); i++) 
	    addCode(doOp(i)); 
	addCode(saveResult);
    }
    exectime {return neg.numberPorts() + 1;}
}
