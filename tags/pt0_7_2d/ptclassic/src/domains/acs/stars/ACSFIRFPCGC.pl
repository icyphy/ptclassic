defcore {
    name { FIR }
    domain { ACS }
    coreCategory { FPCGC }
    corona { FIR } 
    desc { Output the product of the inputs, as a floating-point value. }
    version { @(#)ACSFIRFPCGC.pl	1.2 09/08/99 }
    author { James Lundblad }
    copyright {
Copyright (c) 1998 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
    }
    location { ACS main library }

	defstate {
		name {phaseLength}
		type {int}
		default {0}
		attributes { A_NONCONSTANT|A_NONSETTABLE }
	}
	defstate {
		name {tapSize}
		type {int}
		default {0}
		attributes { A_NONCONSTANT|A_NONSETTABLE }
	}
	constructor {
		noInternalState();
	}
	setup {
		int d = corona.decimation;
		int i = corona.interpolation;
		int dP = corona.decimationPhase;
		corona.input.setSDFParams(d, d+1+(corona.taps.size()/i));
		corona.output.setSDFParams(i, i-1);
		if (dP >= d) {
			Error::abortRun (*this, ": decimationPhase too large");
			return;
		}
		// The phaseLength is ceiling(taps.size/interpolation)
		// It is a protected instance variable.
		int temp = corona.taps.size() / i;
		tapSize = corona.taps.size();
		if ((corona.taps.size() % i) != 0)  temp++;
		phaseLength = temp;
	}
   codeblock(bodyDecl) {
	int phase, tapsIndex, inC, i;
	int outCount = $val(interpolation) - 1;
	int inPos;
	double out, tap;
   }
   codeblock(body) {
	/* phase keeps track of which phase of the filter coefficients is used.
	   Starting phase depends on the decimationPhase state. */
	phase = $val(decimation) - $val(decimationPhase) - 1;   
	
	/* Iterate once for each input consumed */
	for (inC = 1; inC <= $val(decimation) ; inC++) {

		/* Produce however many outputs are required for each 
		   input consumed */
		while (phase < $val(interpolation)) {
			out = 0.0;

			/* Compute the inner product. */
			for (i = 0; i < $val(phaseLength); i++) {
				tapsIndex = i * $val(interpolation) + phase;
				if (tapsIndex >= $val(tapSize))
			    		tap = 0.0;
				else
			 		tap = $ref2(taps,tapsIndex);
				inPos = $val(decimation) - inC + i;
				out += tap * $ref2(input,inPos);
			}
			$ref2(output,outCount) = out;
			outCount--;;
			phase += $val(decimation);
		}
		phase -= $val(interpolation);
	}
   }
	go {
		addCode(bodyDecl);
		addCode(body);
	}
	exectime {
		int x = corona.taps.size();
		int i = corona.interpolation;
		int d = corona.decimation;
		if (x % i != 0) x = x/i + 1;
		else x = x/i;
		int y = i/d;
		if (i % d != 0) y++;
		/* count of elementary operations */
		return 1 + y * (6 + x * 6);	
	}
}
