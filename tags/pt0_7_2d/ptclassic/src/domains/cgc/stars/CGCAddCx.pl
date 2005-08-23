defstar {
    name {AddCx}
    domain {CGC}
    desc { Output the complex product of the inputs. }
    version { @(#)CGCAddCx.pl	1.4 2/3/96 }
    author { Jose Luis Pino }
    copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
    }
    location { CGC main library }
    inmulti {
	name {input}
	type {complex}
    }
    output {
	name {output}
	type {complex}
    }
    constructor { noInternalState(); }

    go { 
	addCode(startOp);
	int i;
	for (i=2;i<=input.numberPorts();i++) 
	    addCode(doOp(i)); 
    }

    codeblock(startOp) {
	$ref(output).real = $ref(input#1).real;
	$ref(output).imag = $ref(input#1).imag;
    }

    codeblock(doOp,"int i") {
	$ref(output).real += $ref(input#@i).real;
	$ref(output).imag += $ref(input#@i).imag;
    }

    exectime {return 2*input.numberPorts();}
}
