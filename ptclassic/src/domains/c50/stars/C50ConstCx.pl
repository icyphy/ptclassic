defstar {
    name { ConstCx }
    domain { C50 }
    desc { Constant source }
    version { $Id$ }
    author { Jose Luis Pino }
    copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
    }
    location { C50 main library }
	htmldoc {
There are no runtime instructions associated with this star.
The output buffer is initialized with the specified DC value.
    }
    output {
	name {output}
	type {COMPLEX}
	attributes{P_NOINIT}
    }
    state {
	name {real}
	type {float}
	default {0.0}
	descriptor { Real part of DC value. }
    }
    state {
	name {imag}
	type {float}
	default {0.0}
	descriptor { Imaginary part of DC value. }
   }

	codeblock(data){
	.ds	$addr(output)
	.q15	$val(real) 
	.q15	$val(imag)
	.text
	}
    constructor {
	noInternalState();
    }
    go{
	addCode(data);
	}
    execTime { return 0; }
}


