defstar {
    name { LinToCirc }
    domain { CG }
    derivedFrom { CGVarTime }
    desc {
Copy data from a linear buffer to a circular buffer.  If N is zero (default),
the number transferred per execution equals the number written per execution
by the star that feeds the input.
The CG version does not do anything though; it is there because
AsmTarget needs it and we use AsmTarget sometimes to test schedulers.
}
    version { @(#)CGLinToCirc.pl	1.2	01 Oct 1996 }
    author { Praveen K. Murthy }
    copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
    }
    location { CG demo library }
	htmldoc {
<a name="linear to circular buffer copy"></a>
<a name="buffer, circular"></a>
<a name="buffer, linear"></a>
<a name="linear buffer"></a>
<a name="circular buffer"></a>
This star is normally inserted automatically, as needed.  Data movement
is repeated inline so may not be efficient for large N.
    }
    hinclude {
	"AsmStar.h"
    }
    input {
	name { input }
	type { anytype }
    }
    output {
	name { output }
	type { =input }
	attributes { P_CIRC }
    }
    defstate {
	name { N }
	type { int }
	desc { number to transfer per execution }
	default { 0 }
    }
    protected {
	int n;
    }
    setup {
	n = int(N);
	if (n == 0)
	    n = input.far()->numXfer();
	input.setSDFParams(n,n-1);
	output.setSDFParams(n,n-1);
    }
    go {
	addCode(code);
    }
    
    codeblock(code) {
// Code for linear-to-circular buffer star (CGLinToCirc)
    }
}
