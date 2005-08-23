defstar {
	name {TkXYPlot}
	domain {DE}
	desc {
Plot Y input(s) vs. X input(s) with dynamic updating.
Time stamps are ignored.  If there is an event on only
one of a matching pair of X and Y inputs, then the previously
received value (or zero if none) is used for the other.
Two styles are currently supported: "dot" causes
points to be plotted, whereas "connect" causes connected
lines to be plotted. Drawing a box in the plot will
reset the plot area to that outlined by the box.
There are also buttons for zooming in and out, and for
resizing the box to just fit the data in view.
	}
	version { @(#)DETkXYPlot.pl	1.8    10/23/95 }
	author { Edward A. Lee }
	copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { DE Tcl/Tk library }
	hinclude { "ptk.h" }
	hinclude { "XYPlot.h" }
	defstate {
                name {label}
                type{string}
                default {"Tk XY Plot"}
                desc {The title of the plot }
        }
        defstate {
		name {geometry}
		type{string}
                default { "600x400+0+0" }
                desc { Specified the location and/or size of the window }
	}
	defstate {
		name {xTitle}
		type{string}
		default { "x" }
		desc { The x-axis label }
	}
	defstate {
		name {yTitle}
		type{string}
		default { "y" }
		desc { The y-axis label }
	}
	defstate {
		name {xRange}
		type{string}
		default {"-1.5 1.5"}
		desc { The range of x-coordinate values }
	}
	defstate {
		name {yRange}
		type{string} 
		default {"-1.5 1.5"}
		desc { The range of y-coordinate values }
	}
	defstate {
		name {persistence}
		type{int}
		default {"100"}
		desc {The number of points displayed at any one time }
	}
	defstate {
	        name {style}
		type {string}
		default {"dot"}
		desc {Plot styles are dot or connect}
	}
	defstate {
	        name {updateSize}
		type{int}
		default {10}
		desc { The number of points drawn simultaneously. Higher numbers make the response faster.  }
	}
	
	inmulti {
		name {X}
		type {float}
		desc { horizontal coordinate }
	}
	inmulti { 
		name {Y}
		type {float}
		desc { vertical coordinate }
	}
	protected {
	  XYPlot xyplot;
	  InfString labCopy, geoCopy, xtCopy, ytCopy;
	  double xMin, xMax, yMin, yMax;
	}
	setup {
	  // parse the x and y ranges which are
	  // specified as strings for user convenience
	  if ((sscanf((const char *)xRange, "%lf %lf", &xMin, &xMax) != 2) ||
	      (xMax <= xMin)) {
	    Error::abortRun(*this, "xRange parameter values are invalid");
	  }
	  if ((sscanf((const char *)yRange, "%lf %lf", &yMin, &yMax) != 2) ||
	      (yMax <= yMin)) {
	    Error::abortRun(*this, "yRange parameter values are invalid");
	  }
	}
	begin {
	  // Need to make non-const copies of  strings to
	  // avoid compilation warnings
	  labCopy = (const char*)label;
	  geoCopy = (const char*)geometry;
	  xtCopy = (const char*)xTitle;
	  ytCopy = (const char*)yTitle;

	  int plotstyle = 0;
	  if (strcmp(style,"connect") == 0) plotstyle = 1;

	  // create the XYplot window labeling, scaling,
	  // and ranging as specified by the parameters
	  xyplot.setup(this,	   
		       (char*)  labCopy,     // Label for the XY plot
		       (int)    persistence, // no. data points to retain
		       (int)    updateSize,  // no. data points between refreshes
		       (char*)  geoCopy,     // Geometry for the window
		       (char*)  xtCopy,      // Title for X-axis
		                xMin,        // minimum X range value
		                xMax,        // maximum X range value
		       (char*)  ytCopy,      // Title for Y-axis
		                yMin,	     // minimum Y range value
		                yMax,        // maximum Y range value
		                Y.numberPorts(),   // The number of data sets
		       (int)    plotstyle);  // the plot style to use

	}
	go {
	  InDEMPHIter nextx(X), nexty(Y);
	  InDEPort *px, *py = (InDEPort*)NULL;
	  int set = 1;
	  while ((px = nextx++) != 0 && (py = nexty++) != 0) {
	    if (px->dataNew || py->dataNew) {
	      xyplot.addPoint((double)px->get(),(double)py->get(), set);
	    }
	    set++;
	  }
	}
}
