defstar {
	name { Waterfall }
	derivedFrom { Xgraph }
	domain { SDF }
	desc {
Plots a series of traces in the style of a "waterfall" plot.
This is a type of three-dimensional plot used to show the evolution
of signals or spectra.  Optionally, each plot can be made opaque, so
that lines that would appear behind the plot are eliminated.
The star is derived from Xgraph.
	}
	version {$Id$}
	author { E. A. Lee }
	copyright {
Copyright (c) 1990, 1991, 1992 The Regents of the University of California.
All rights reserved.
See the file ~ptolemy/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { SDF main library }
	seealso {Xgraph XMgraph XYgraph xgraph Xhistogram timeVarSpec}
	defstate {
		name {traceLength}
		type {int}
		default {"128"}
		desc { Number of samples per trace. }
	}
	defstate {
		name {xUnits}
		type {float}
		default {"1.0"}
		desc { Number of horizontal units per input sample. }
	}
	defstate {
		name {xInit}
		type {float}
		default {"0.0"}
		desc { Horizontal value of the first input sample. }
	}
	defstate {
		name {xOffset}
		type {int}
		default {"4"}
		desc {
Amount of right shift of each successive trace, in number of samples.
		}
	}
	defstate {
		name {yOffset}
		type {float}
		default {"4.0"}
		desc {
Amount of upward shift of each successive trace, in vertical units.
		}
	}
	defstate {
		name {showHiddenLines}
		type {int}
		default {NO}
		desc {
Turns on or off hidden-line elimination.
		}
	}
	defstate {
		name {showZeroPlane}
		type {int}
		default {YES}
		desc {
Turns on or off display of the plane where y=0.
		}
	}
	protected {
		int traceCount;
		int nTracesSoFar;
		double xval;
		double* highestValue;
		int inBreak;
	}
	constructor { highestValue = 0;}
	destructor { LOG_DEL; delete highestValue;}
	start {
		graph.initialize(this, 2, options, title, saveFile, ignore);
		// allow access to one past sample
		input.setSDFParams(1,1);
		traceCount = 0;
		nTracesSoFar = 0;
		graph.setIgnore(0);
		xval = double(xInit);
		LOG_DEL; delete highestValue;
		LOG_NEW; highestValue = new double [int(traceLength)];
		inBreak = FALSE;
	}
	method {
		name {visiblePoint}
		type {void}
		access {protected}
		arglist {"(const double pointVal)"}
		code {
		    double x,y,in0,in1,hv0,hv1,d;
		    if(inBreak) graph.newTrace();
		    if(inBreak && traceCount != 0) {
			// Plot opening point
			in0 = double(yOffset)*nTracesSoFar + pointVal;
			in1 = double(yOffset)*nTracesSoFar + double(input%1);
			hv0 = highestValue[traceCount];	// current highest val
			hv1 = highestValue[traceCount-1]; // old highest val
			// Compute denominator (can't be zero)
			d = (in0-in1) - (hv0-hv1);
			// Compute x coordinate
			x = xval - double(xUnits) + (hv1-in1)*double(xUnits)/d;
			// Compute the y coordinate
			y = in1 + (hv1-in1)*(in0-in1)/d;
			graph.addPoint(1,x,y);
		    }
		    graph.addPoint(1,xval,
			double(yOffset)*nTracesSoFar + pointVal);
		    inBreak = FALSE;
		    xval += double(xUnits);
		    if(traceCount >= int(xOffset))
			highestValue[traceCount-int(xOffset)]
			    = double(yOffset)*nTracesSoFar + pointVal;
		}
	}
	method {
		name {hiddenPoint}
		type {void}
		access {protected}
		arglist {"(const double pointVal)"}
		code {
		    double x,y,in0,in1,hv0,hv1,d;
		    if(!inBreak) {
			// Plot closing point
			in0 = double(yOffset)*nTracesSoFar + pointVal;
			in1 = double(yOffset)*nTracesSoFar + double(input%1);
			hv0 = highestValue[traceCount];	// current highest val
			hv1 = highestValue[traceCount-1]; // old highest val
			// Compute denominator (can't be zero)
			d = (hv0-hv1) - (in0-in1);
			// Compute x coordinate
			x = xval - double(xUnits) + (in1-hv1)*double(xUnits)/d;
			// Compute the y coordinate
			y = hv1 + (in1-hv1)*(hv0-hv1)/d;
			graph.addPoint(1,x,y);
		    }
		    inBreak=TRUE;
		    xval += double(xUnits);
		    if((traceCount >= int(xOffset))
			&& (traceCount < int(traceLength) - int(xOffset)))
			    highestValue[traceCount-int(xOffset)]
				= highestValue[traceCount];
		}
	}
	go {
		if (traceCount >= int(traceLength)) {
			traceCount = 0;
			if (nTracesSoFar >= int(ignore)) inBreak = TRUE;
			nTracesSoFar++;
			xval = double(xInit)
				+ nTracesSoFar*int(xOffset)*double(xUnits);
		}
		double in;
		if (nTracesSoFar >= int(ignore)) {
		    in = double(input%0);
		    if (nTracesSoFar - int(ignore) == 0) visiblePoint(in);
		    else if (traceCount >= int(traceLength) - int(xOffset)) {
			// In this case, prevent connection
		        if(inBreak) graph.newTrace();
			inBreak = FALSE;
			visiblePoint(in);
		    } else {
			if (double(yOffset)*nTracesSoFar + in
			     >= highestValue[traceCount]||int(showHiddenLines))
			    visiblePoint(in);
			else
			    hiddenPoint(in);
		    }
		}
		traceCount++;
	}
	wrapup {
		if(showZeroPlane) {
			graph.addPoint(2,double(xInit),0.0);
			graph.addPoint(2,
			  double(xInit)+int(traceLength)*double(xUnits),0.0);
			graph.addPoint(2,
			  double(xInit)+int(traceLength)*double(xUnits)
			  + nTracesSoFar*int(xOffset)*double(xUnits),
			  nTracesSoFar*double(yOffset));
			graph.addPoint(2,
			  double(xInit)
			  + nTracesSoFar*int(xOffset)*double(xUnits),
			  nTracesSoFar*double(yOffset));
			graph.addPoint(2,double(xInit),0.0);
		}
		SDFXgraph::wrapup();
	}
}
