defstar {
    name {MpyInt}
    domain {CGC}
    desc { Output the complex product of the inputs. }
    version { @(#)CGCMpyInt.pl	1.1	2/20/96 }
    author { Brian L. Evans and Jose Luis Pino }
    copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
    }
    location { CGC main library }
    inmulti {
	name {input}
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
	int sum = $ref(input#1);
    }
    codeblock(doOp,"int i") {
	sum *= $ref(input#@i);
    }
    codeblock(saveResult) {
	$ref(output) = sum;
    }
    go { 
	addCode(startOp);
	for (int i = 2; i <= input.numberPorts(); i++) 
	    addCode(doOp(i)); 
	addCode(saveResult);
    }
    exectime {return input.numberPorts() + 1;}
}
