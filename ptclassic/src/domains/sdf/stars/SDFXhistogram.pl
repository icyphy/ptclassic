ident {
/**************************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  E. A. Lee
 Date of creation: 10/20/90

 Creates a histogram with the xgraph function.  It is assumed that "xgraph"
 is on your path, or this will not work!!!

**************************************************************************/
}

defstar {
	name { Xhistogram }
	domain { SDF }
	desc { "Generate a histogram with the xgraph program." }
	input {
		name { input }
		type { anytype }
	}
	defstate {
		name {title}
		type {string}
		default {"Xhistogram"}
		desc {"graph title"}
	}
	defstate {
		name {saveFile}
		type {string}
		default {""}
		desc {"file to save Xhistogram input"}
	}
	defstate {
		name {binWidth}
		type {float}
		default {"1.0"}
		desc {"Width of bins for histogram."}
	}
	defstate {
		name {options}
		type {string}
		default {"-bar -nl"}
		desc {"command line options for xgraph"}
	}
	protected {
		SequentialList bins;
		float lowValue, highValue;	// current range of bins
	}
	ccinclude { "miscFuncs.h" <math.h> }
	hinclude {"DataStruct.h" "Display.h", "StringList.h" }
	go {
	    float data = float(input%0);
	    int i, numToAdd;
	    int* count;
	    if (bins.size() == 0) {
		// first element
		count = new int;
		*count = 1;
		bins.put(count);
		lowValue = int(data/double(binWidth))*double(binWidth);
		highValue = lowValue + double(binWidth);
		return;
	    }
	    if (data < lowValue) {
		// add elements to the beginning of the list until
		// the data value has a bin.
		// Figure out how many that is
		numToAdd = int(ceil((lowValue-data)/double(binWidth)));
		// Add the zero-valued elements
		for ( i = numToAdd-1; i > 0; i--) {
		    count = new int;
		    *count = 0;
		    bins.tup(count);
		}
		// Add the one-valued element
		count = new int;
		*count = 1;
		bins.tup(count);
		lowValue = int(data/double(binWidth))*double(binWidth);
		return;
	    }
	    if (data >= highValue) {
		// add elements to the end of the list until
		// the data value has a bin.
		// Figure out how many that is (minus one)
		numToAdd = int((data-highValue)/double(binWidth));
		// Add the zero-valued elements
		for ( i = numToAdd; i > 0; i--) {
		    count = new int;
		    *count = 0;
		    bins.put(count);
		}
		// Add the one-valued element
		count = new int;
		*count = 1;
		bins.put(count);
		highValue = int(data/double(binWidth))*double(binWidth)
			+ double(binWidth);
		return;
	    }
	    // If we get to this point, the data value fits within the range
	    // Compute the index
	    int index = int((data - lowValue)/double(binWidth));
	    bins.reset();
	    for (int t = index; t>=0; t--)
	    	count = (int*)bins.next();
	    *count = *count + 1;
	}
	wrapup {
		// Begin by constructing the options string
		StringList exOptions;
		char barWidth[128];
		sprintf(barWidth, " -brw %g", double(binWidth)/2);
		exOptions += (char *) options;
		exOptions += barWidth;

		XGraph graph;
		graph.initialize(1, (const char*) exOptions,
				    (const char*) title,
				    (const char*) saveFile);

		// Write data from bin structure into file
		bins.reset();
		int size = bins.size();
		float x = lowValue + double(binWidth)/2;
		for (int i = 1; i <= size; i++ ) {
		   graph.addPoint(x, double(*(int*)bins.next()));
		   x += double(binWidth);
		}
		graph.terminate();
	}
}
