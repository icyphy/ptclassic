defstar {
	name { XYgraph }
	derivedFrom { Xgraph }
	domain { CGC }
	desc {
Generates an X-Y plot with the xgraph program.
The X data is on "xInput" and the Y data is on "input".
	}
	version {$Id$}
	author { S. Ha }
	copyright {
Copyright (c) 1990, 1991, 1992 The Regents of the University of California.
All rights reserved.
See the file ~ptolemy/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { CGC main library }
	explanation {
The input signal is plotted using the \fIxgraph\fR program, with one
input interpreted as the x-axis data, and the other input as y-axis data.
.Id "graph, X-Y"
.Ir "xgraph program"
	}
	seealso {Xgraph XMgraph xgraph Xhistogram}
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
		count++;
		if (count <= int(ignore)) return;
                addCode(
"\tfprintf($starSymbol(fp),\"%g %g\\n\",$ref(xInput),$ref(input));\n");
	}
	exectime {
		return 6;
	}
}
