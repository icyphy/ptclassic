defstar {
	name { XYgraph }
	derivedFrom { Xgraph }
	domain { SDF }
	desc {
Generates an X-Y plot with the pxgraph program.
The X data is on "xInput" and the Y data is on "input".
	}
	version {@(#)SDFXYgraph.pl	1.13 10/06/96}
	author { J. T. Buck }
	copyright {
Copyright (c) 1990-1997 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { SDF main library }
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
		graph.addPoint(float(xInput%0), float(input%0));
	}
}
