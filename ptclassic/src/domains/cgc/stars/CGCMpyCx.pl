defstar {
    name {MpyCx}
    domain {CGC}
    desc { Output the complex product of the inputs. }
    version { $Id$ }
    author { Jose Luis Pino }
    copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
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

    go { addCode(doMath); }

    codeblock(doMath) {
	$ref(output).real = 1;
	$ref(output).imag = 1;
    	{
	    int i;
	    for (i=1; i<=$size(input); i++) {
		complex temp;
		temp.real = $ref(output).real * $ref(input,i).real -
		    $ref(output).imag * $ref(input,i).imag;
		temp.imag = $ref(output).real * $ref(input,i).imag +
		    $ref(output).imag * $ref(input,i).real;
		$ref(output).real += temp.real;
		$ref(output).imag += temp.imag;
	    }
	}
    }

    exectime {return 4*input.numberPorts();}
}
