defstar {
	name { XYgraph }
	derivedFrom { Xgraph }
	domain { CGC }
	desc {
Generates an X-Y plot with the pxgraph program.
The X data is on "xInput" and the Y data is on "input".
	}
	version {@(#)CGCXYgraph.pl	1.11	06 Oct 1996}
	author { S. Ha }
	copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { CGC main library }
	htmldoc {
The input signal is plotted using the <i>pxgraph</i> program, with one
input interpreted as the x-axis data, and the other input as y-axis data.
<a name="graph, X-Y"></a>
<a name="pxgraph program"></a>
	}
	seealso {Xgraph XMgraph $PTOLEMY/src/pxgraph/pxgraph.htm Xhistogram}
	input {
		name { xInput }
		type { anytype }
	}
	constructor {
		// make xInit and xUnits invisible
		xUnits.setAttributes(A_NONSETTABLE);
		xInit.setAttributes(A_NONSETTABLE);
	}
	go {
@	if (++$ref(count) >= $val(ignore)) 
@		fprintf($starSymbol(fp),"%g %g\n",$ref(xInput),$ref(input));
	}
	exectime {
		return 6;
	}
}
