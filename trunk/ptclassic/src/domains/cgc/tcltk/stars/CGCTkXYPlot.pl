defstar {
	name {TkXYPlot}
	domain {CGC}
	desc {
Plot Y input(s) vs. X input(s) with dynamic updating.
Two styles are currently supported: "dot" causes
points to be plotted, whereas "connect" causes connected
lines to be plotted. Drawing a box in the plot will
reset the plot area to that outlined by the box.
There are also buttons for zooming in and out, and for
resizing the box to just fit the data in view.
Generate an animated X-Y plot in a Tk window.
	}
	version { $Id$ }
	author { W.-J. Huang, E. A. Lee, D. Niehaus }
	copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { CGC tcltk library }
	defstate {
                name {label}
                type{string}
                default {"Tk XY Plot"}
                desc {The label used for the icon and title names}
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
		desc { The range of x-coordinate values }
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
		desc {Plot style is dot or connect}
	}
	defstate {
	        name {updateSize}
		type{int}
		default {10}
		desc { The number of points drawn simulataneously. Higher numbers make the response faster.  }
	}
	inmulti {
		name {X}
		type{float}	
		desc { horizontal coordinate }
	}
	inmulti {
		name {Y}
		type{float}
		desc { vertical coordinate }
	}
	initCode {
	  if (int(persistence) <= 0) {
	    Error::abortRun(*this, "invalid persistence");
	    return;
	  }
	  double xMin, xMax, yMin, yMax;
	  // Store xMin and xMax for the benefit of the line style in TkPlot
	  addGlobal("double $starSymbol(xMin);");
	  addGlobal("double $starSymbol(xMax);");

	  if ((sscanf((const char*)xRange,"%lf %lf", &xMin, &xMax) != 2) ||
	      (xMin >= xMax)) {
	    Error::abortRun(*this,"xRange parameter values are invalid");
	  }
	  if ((sscanf((const char*)yRange,"%lf %lf", &yMin, &yMax) != 2) ||
	      (yMin >= yMax)) {
	    Error::abortRun(*this,"yRange parameter values are invalid");
	  }
	  addInclude("\"ptkPlot_defs.h\"");
	  addGlobal("ptkPlotWin $starSymbol(plotwin);");
	  addGlobal("ptkPlotDataset $starSymbol(plotdataset);");

	  // This is sourced multiple times if there are multiple
	  // instances of the star.  Should this be fixed?
	  addCode(srcTclFile,"tkSetup");

	  int plotstyle = 0;
	  if (strcmp(style,"connect") == 0) plotstyle = 1;
	  addCode(createPlot(xMin,xMax,yMin,yMax,Y.numberPorts(),plotstyle),
		  "tkSetup");
	}
	go {
	  for (int i = 0; i < X.numberPorts(); i++) {
	    addCode(updateDisplay(i+1));
	  }
	}
	codeblock(srcTclFile) {
	  {
	    /* This ridiculosity is required because Tcl scribbles on strings */
	    static char temp[100];
	    sprintf(temp,"source $$env(PTOLEMY)/src/ptklib/ptkPlot.tcl");
	    if(Tcl_Eval(interp, temp) != TCL_OK)
		 errorReport("Cannot source tcl script for TkXYPlot star");
	  }
	}
	codeblock (createPlot,
		   "double xMin, double xMax, double yMin, double yMax, int numsets, int plotstyle") {
	  ptkInitPlot(&$starSymbol(plotwin));
	  $starSymbol(xMin) = @xMin;
	  $starSymbol(xMax) = @xMax;
	  if(ptkCreatePlot(interp,
			   &$starSymbol(plotwin),
			   &w,
			   ".$starSymbol(win)",
			   "$val(label)",
			   "$val(geometry)",
			   "$val(xTitle)",
			   "$val(yTitle)",
			   @xMin,@xMax, @yMin,@yMax,
			   @numsets,
			   $val(updateSize),
			   @plotstyle,
			   $val(persistence)) == 0) {
	    errorReport(ptkPlotErrorMsg());
	  }
	}
        codeblock (updateDisplay,"int setnum") {
	  if (ptkPlotPoint(interp, &$starSymbol(plotwin), @setnum,
			   $ref(X#@setnum), $ref(Y#@setnum)) == 0) {
	    errorReport(ptkPlotErrorMsg());
	  }
	}
}

