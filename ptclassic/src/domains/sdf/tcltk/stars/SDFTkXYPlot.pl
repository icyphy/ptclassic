defstar {
	name {TkXYPlot}
	domain {SDF}
	desc {
Dynamically display points in an XY plot
	}
	version { $Id$ }
	author { E. A. Lee and D. Niehaus }
	copyright {
Copyright (c) 1990-1994 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { SDF tcltk library }
	hinclude { "ptk.h" }
	hinclude { "XYPlot.h" }

	defstate {
                name {label}
                type{string}
                default {"Tk XY Plot"}
                desc {The label used for the icon and title names}
        }
        defstate {
		name {geometry}
		type{string}
                default { "600x600+100+100" }
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
	        name {updateSize}
		type{int}
		default {10}
		desc { The number of points drawn simulataneously. Higher numbers make the response faster.  }
	}
	
	input {
		name {X}
		type {float}
		desc { horizontal coordinate }
	}
	input {
		name {Y}
		type {float}
		desc { vertical coordinate }
	}
	protected {
		XYPlot xyplot;
	}

	begin {
	  // Need to make non-const copies of  strings to
	  // avoid compilation warnings
	  InfString labCopy((const char*)label);
	  InfString geoCopy((const char*)geometry);
	  InfString xtCopy((const char*)xTitle);
	  InfString ytCopy((const char*)yTitle);

	  // parse the x and y ranges which are
	  // specified as strings for user convenience
	  double xMin, xMax, yMin, yMax;

	  if ((sscanf((const char *)xRange,"%lf %lf", &xMin, &xMax) != 2) ||
	      (xMax <= xMin )) {
	    Error::abortRun(*this, "xRange parameter values are invalid");
	  }
	  if ((sscanf((const char *)yRange,"%lf %lf", &yMin, &yMax) != 2) ||
	      (yMax <= yMin )) {
	    Error::abortRun(*this, "yRange parameter values are invalid");
	  }
	  
	  // create the XYplot window labeling, scaling,
	  // and ranging as specified by the parameters
	  xyplot.setup(this,	   
		       (char*)  labCopy,     // Label for the XY plot
		       (int)    persistence, // The number of data points to retain
		       (int)    updateSize,  // The number of data points between refreshes
		       (char*)  geoCopy,     // Geometry for the window
		       (char*)  xtCopy,      // Title for X-axis
		                xMin,        // minimum X range value
		                xMax,        // maximum X range value
		       (char*)  ytCopy,      // Title for Y-axis
		                yMin,	     // minimum Y range value
		                yMax);       // maximum Y range value

	}
	go {
	  xyplot.addPoint(double(X%0),double(Y%0));
	}
}
