defstar {
	name {TkXYPlot}
	domain {SDF}
	desc {
Plot Y input(s) vs. X input(s) with dynamic updating.
Two styles are currently supported: "dot" causes
points to be plotted, whereas "connect" causes connected
lines to be plotted. Drawing a box in the plot will
reset the plot area to that outlined by the box.
There are also buttons for zooming in and out, and for
resizing the box to just fit the data in view.
	}
	version { @(#)SDFTkXYPlot.pl	1.11    2/28/96 }
	author { Wei-Jen Huang, Edward A. Lee, and Douglas Niehaus }
	copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { SDF Tcl/Tk library }
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
                default { "720x400+0+0" }
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
		type{floatarray}
		default {"-1.5 1.5"}
		desc { The range of x-coordinate values }
	}
	defstate {
		name {yRange}
		type{floatarray} 
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
		desc { The number of points drawn simulataneously. Higher numbers make the response faster.  }
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
	  char *labCopy;
	  char *geoCopy;
	  char *xtCopy;
	  char *ytCopy;
	}

	// The private method "validRange" takes one argument: a float
	// array state "range".  It is only intended to be invoked by
	// the "begin" method on the "xRange" and "yRange" states of 
	// the star.
	//
	// "validRange" returns TRUE if the array state "range" has two
	// elements, with the first element (the low end of the range)
	// being less than the second element (the high end of the range).
	// Otherwise, it returns FALSE.
	method {
	  name { validRange }
	  access { private }
	  arglist { "(FloatArrayState& range)" }
	  type { int }
	  code {
	    if (range.size() != 2) {
	      StringList msg;
	      msg << "State \"" << range.name() << "\" should be a float "
	        << "array with two elements, but it has " << range.size() 
		<< " elements.";
	      Error::abortRun(*this, msg);
	      return FALSE;
	    }
	    if (range[0] >= range[1]) {
	      StringList msg;
	      msg << "The first number in the float array state \""
	          << range.name() << "\" should be less than the second, "
		  << "but they are " << range[0] << " and " << range[1]
	          << ", respectively."; 
	      Error::abortRun(*this, msg);
	      return FALSE;
	    }
	    return TRUE;  // the float array "range" is valid
	  }
	}

	constructor {
	  labCopy = geoCopy = xtCopy = ytCopy = 0;
	}

	setup {
	  // Check for valid x and y ranges.  They should each be a
	  // float array with the two elements, with the first element
	  // being less than the second.
	  if (!validRange(xRange)) return;
	  if (!validRange(yRange)) return;
	}

	destructor {
	  delete [] labCopy;
	  delete [] geoCopy;
	  delete [] xtCopy;
	  delete [] ytCopy;
	}

	begin {
	  // Need to make non-const copies of strings to
	  // avoid compilation warnings
	  delete [] labCopy;
	  labCopy = savestring( (const char*)label );
	  delete [] geoCopy;
	  geoCopy = savestring( (const char*)geometry );
	  delete [] xtCopy;
	  xtCopy = savestring( (const char*)xTitle );
	  delete [] ytCopy;
	  ytCopy = savestring( (const char*)yTitle );

	  int plotstyle = 0;
	  if (strcmp(style, "connect") == 0) plotstyle = 1;

	  // create the XYplot window labeling, scaling,
	  // and ranging as specified by the parameters
	  xyplot.setup(		this,
		       		labCopy,     // label for XY plot
			(int)	persistence, // number data points to retain
			(int)	updateSize,  // number data points between refreshes
				geoCopy,     // geometry for the window
				xtCopy,      // title for X-axis
				xRange[0],   // minimum X range value
				xRange[1],   // maximum X range value
				ytCopy,      // title for Y-axis
				yRange[0],   // minimum Y range value
				yRange[1],   // maximum Y range value
				Y.numberPorts(),   // number of data sets
			(int)	plotstyle);  // plot style to use
	}

	go {
	  MPHIter nextx(X), nexty(Y);
	  PortHole *px, *py = (PortHole *)NULL;
	  int set = 1;
	  while ((px = nextx++) != 0 && (py = nexty++) != 0) {
	    xyplot.addPoint((double)((*px)%0),(double)((*py)%0), set++);
	  }
	}
}
