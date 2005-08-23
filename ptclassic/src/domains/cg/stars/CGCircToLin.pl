defstar {
    name { CircToLin }
    domain { CG }
    derivedFrom { CGVarTime }
    desc {
Copy data from a circular buffer to a linear buffer.
If N is zero (the default), the number of samples transferred per execution
equals the number of samples read per execution by the star that reads
from the output. 
The CG version does not do anything though; it is there because
AsmTarget needs it and we use AsmTarget sometimes to test schedulers.
    }
    version { @(#)CGCircToLin.pl	1.2	01 Oct 1996 }
    author { Praveen K. Murthy }
    copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
    }
    location { CG demo library }
	htmldoc {
<a name="circular to linear buffer copy"></a>
<a name="buffer, circular"></a>
<a name="buffer, linear"></a>
<a name="linear buffer"></a>
<a name="circular buffer"></a>
This star is normally inserted automatically, as needed.
Data movement is repeated inline so may not be efficient for large N.
    }
    hinclude {
	"AsmStar.h"
    }
    input {
        name { input }
        type { anytype }
        attributes { P_CIRC}
    }
    output {
	name { output }
	type { =input }
    }
    protected {
	int n;
    }
    defstate {
	name { N }
	type { int }
	desc { number to transfer per execution }
	default { 0 }
    }
    setup {
	n = int(N);
	if (n == 0) {
	    n = output.far()->numXfer();
	}
	input.setSDFParams(n,n-1);
	output.setSDFParams(n,n-1);
    }
    go {
	addCode(code);
    }

    codeblock(code) {
// Code for circular-to-linear buffer star (CGCircToLin)
    }
}


