ident {
/**************************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  J. T. Buck
 Date of creation: 6/12/90
 Revised to use preprocessor, 10/3/90
 Revised to use XGraph class, 10/20/90

 This is derived from the basic Xgraph star.  It does an X-Y plot.
 The start and wrapup functions from the base class are used.

**************************************************************************/
}
defstar {
	name { XYgraph }
	derivedFrom { Xgraph }
	domain { SDF }
	desc {
		"Generates an X-Y plot with the xgraph program.\n"
		"The X data is on xInput and the Y data is on input."
	}
	input {
		name { xInput }
		type { anytype }
	}
	go {
		graph.addPoint(float(xInput%0), float(input%0));
	}
}
