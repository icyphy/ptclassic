defstar {
	name {TkXYPlot}
	domain {CGC}
	desc {
Generate an animated X-Y plot in a Tk window.
	}
	version { $Id$ }
	author { W. Huang, E. A. Lee, D. Niehaus }
	copyright {
Copyright (c) 1994 The Regents of the University of California.
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
		desc { The range of x-coordinate values }
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
		type{float}	
		desc { horizontal coordinate }
	}
	input {
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
	  if ((sscanf((const char*)xRange,"%lf %lf", &xMin, &xMax) != 2) ||
	      (xMin >= xMax)) {
	    Error::abortRun(*this,"xRange parameter values are invalid");
	  }
	  if ((sscanf((const char*)yRange,"%lf %lf", &yMin, &yMax) != 2) ||
	      (yMin >= yMax)) {
	    Error::abortRun(*this,"yRange parameter values are invalid");
	  }
	  addInclude("\"ptkPlot_defs.h\"");
	  addGlobal("Tcl_HashTable $starSymbol(hashTable);");
	  addGlobal("plotWin $starSymbol(plotwin);");
	  addGlobal("plotDataset $starSymbol(plotdataset);");
	  addCode(createHashTable,"tkSetup");

	  // This is sourced multiple times if there are multiple
	  // instances of the star.  Should this be fixed?
	  addCode(srcTclFile,"tkSetup");
	  addCode(createPlot(xMin,xMax,yMin,yMax),"tkSetup");
	}
	go {
	    addCode(updateDisplay);
	}
	codeblock(createHashTable) {
	    Tcl_InitHashTable(&$starSymbol(hashTable),TCL_STRING_KEYS);
	}
	codeblock(srcTclFile) {
	  {
	    char temp[] = "source $$env(PTOLEMY)/src/ptklib/ptkPlot.tcl";
	    if(Tcl_Eval(interp, temp) != TCL_OK)
		 errorReport("Cannot source tcl script for TkXYPlot star");
	  }
	}
	codeblock (createPlot,"double xMin, double xMax, double yMin, double yMax") {
	  if(createPlot(interp,
			&$starSymbol(plotwin),
			&w,
			".$starSymbol(win)",
			"$val(label)",
			"$val(geometry)",
			"$val(xTitle)",
			"$val(yTitle)",
			@xMin,@xMax, @yMin,@yMax) == 0) {
	    errorReport("In TkXYPlot, error creating plot");
	  }
	  if(createDataset(interp,
			   &$starSymbol(plotdataset),
			   &w,
			   $val(persistence),
			   $val(updateSize)) == 0) {
	    errorReport("In TkXYPlot, error creating data set");
	  }
	  assocData(interp,&$starSymbol(plotdataset),&$starSymbol(plotwin),"dataset 1");
	}
        codeblock (updateDisplay) {
	  if (plotPoint(interp, &$starSymbol(plotdataset), $ref(X), $ref(Y)) == 0) {
	    errorReport("in TkXYPlot, error plotting data point");
	  }
	}
}

