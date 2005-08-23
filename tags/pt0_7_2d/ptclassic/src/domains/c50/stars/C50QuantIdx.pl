defstar {
	name { QuantIdx }
	domain { C50 }
	derived { Quant }
	desc {
The star quantizes the input to one of N+1 possible output levels
using N thresholds.  It also outputs the index of the quantization
level used.  See the Quant star for more information.
	} 
	version { @(#)C50QuantIdx.pl	1.5 	7/10/96 }
	author { Luis Gutierrez, based on CG56 version }
	copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}

	location { C50 main library }
        output {
		name {stepNumber}
		type {int}
		desc {Level of the quantization from 0 to N-1}
	}

	codeblock(outputStepNumber){
	smmr	ar1,#$addr(stepNumber)
	}

	// Inherit the setup method

	go {
		C50Quant :: go();
		addCode(outputStepNumber);
		
	}

	exectime {
	        return 1 + C50Quant::myExecTime();
	}

    }







