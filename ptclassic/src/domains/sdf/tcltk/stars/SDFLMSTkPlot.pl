defstar {
	name {LMSTkPlot}
	domain {SDF}
	derivedFrom { LMS }
	desc { 
Just like the LMS filter, but with an animated Tk display of
the taps, plus associated controls.
	}
	version { @(#)SDFLMSTkPlot.pl	1.1	10/8/93 }
	author { E. A. Lee }
	copyright {
Copyright (c) 1993 The Regents of the University of California.
All rights reserved.
See the file ~ptolemy/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	seealso {LMS}
	location { SDF tcltk library }
	defstate {
		name {stepSizeLow}
		type{float}
		default {"0.0"}
		desc { Low end of the step size scale }
	}
	defstate {
		name {stepSizeHigh}
		type{float}
		default {"0.1"}
		desc { High end of the step size scale }
	}
	defstate {
		name {fullScale}
		type{float}
		default {"1.0"}
		desc {Full scale on the tap display}
	}
	defstate {
		name {geometry}
		type{string}
		default { "+500+000" }
		desc { Specified the location and/or size of the window }
	}
	defstate {
		name {width}
		type{float}
		default { 10.0 }
		desc { Width of the bar chart display, in centimeters }
	}
	defstate {
		name {height}
		type{float}
		default { 5.0 }
		desc { Height of the bar chart display, in centimeters }
	}
	defstate {
		name {identifier}
		type{string}
		default {"LMS filter taps"}
		desc {The string to identify the run-time display.}
	}
	defstate {
		name {updateInterval}
		type{int}
		default {"10"}
		desc {Number of invocations between display updates.}
	}
	hinclude { "ptk.h" }
	ccinclude { "ptkBarGraph.h" }
	private {
	    // Count iterations
	    int invCount;

	    // Store the tk id's of the rectangles
	    int *ids;

	    // To create a unique name for each instance of the star
	    static int instCount;
	    int myInst;
	    char winName[128];
	    char butName[20];
	    char sliderName[20];
	    char command[128];

	    // Keep a modifiable version of the fullScale parameter
	    double scale;

	    // Keep track of whether the setup routine has run already.
	    // Avoid recreating the window if so.
	    int setupRun;
	}
	code {
	    // The following line initializes the member "instCount"
	    // when the .o file is loaded.
	    int SDFLMSTkPlot::instCount = 0;
	}
	constructor {
	    // Used to generate a unique name
	    myInst = instCount++;
	    sprintf(butName,"reset%d",myInst);
	    sprintf(sliderName,"scale%d",myInst);
	    setupRun = 0;
	    ids = 0;
	}
	destructor {
	    // Remove Tcl commands
	    sprintf(command, "%sLMSTkCB", sliderName);
	    Tcl_DeleteCommand (ptkInterp, command);

	    sprintf(command, "%sLMSTkCB", butName);
	    Tcl_DeleteCommand(ptkInterp, command);

	    sprintf(command, "%sLMSTkCBredraw", winName);
	    Tcl_DeleteCommand(ptkInterp, command);

	    LOG_DEL; delete ids;
	}
	setup {

	    scale = double(fullScale);
	    // Initialize iteration count
	    invCount = 0;
    
	    // Compute slider position from parameters
	    int position = int(0.5 + 100*(double(stepSize)
			    - double(stepSizeLow))/(double(stepSizeHigh)
			    - double(stepSizeLow)));
	    // Check that the slider position is in the range of 0 to 100
	    if ((position < 0) || (position > 100)) {
        	    Error::abortRun(*this,
			"Step size is out of range of control");
        	    return;
	    }
	    if (setupRun == 0) {
		setupRun = 1;
		// Create a new window
	        LOG_DEL; delete ids;
	        LOG_NEW; ids = new int[taps.size()];

		// First check to see whether a global Tcl variable named
		// "ptkControlPanel" is defined.  If so, then use it as the
		// stem of the window name.  This means that the window will
		// be a child window of the control panel, and hence will be
		// destroyed when the control panel window is destroyed.
		if(Tcl_VarEval(ptkInterp,
		    "global ptkControlPanel;set ptkControlPanel",
		    (char*)NULL) != TCL_OK)
			sprintf(winName,".sdfLMSTkPlot%d",myInst);
		else
			sprintf(winName,"%s.sdfLMSTkPlot%d",
				ptkInterp->result, myInst);

		// Register the callback functions with Tcl
		sprintf(command, "%sLMSTkCB", sliderName);
		Tcl_CreateCommand (ptkInterp, command, setStep,
			(ClientData)this, NULL);
		sprintf(command, "%sLMSTkCB", butName);
		Tcl_CreateCommand(ptkInterp, command, reset,
			(ClientData)this, NULL);
		sprintf(command, "%sLMSTkCBredraw", winName);
		Tcl_CreateCommand(ptkInterp, command, redraw,
			(ClientData)this, NULL);

	        // Hack alert: passes a pointer to the implemenation
	        // array for the taps.  Thus makeBarChart directly reads the 
	        // value of the taps, violating the object-oriented nature of
	        // the FloatArrayState object.  This was done so that the same
	        // code could be shared with CGC, and so that it would be very
	        // efficient in both cases.
	        if(ptkMakeBarGraph(
		    ptkInterp,			// Tcl interpreter
		    ptkW,			// reference window
		    winName,			// name of top level window
		    (const char*)identifier,	// identifying string
		    &taps[0],			// data to be plotted
		    taps.size(),		// number of data points
		    &scale,			// full scale value
		    ids,			// array to store item ids
		    (const char*)geometry,	// shape and position, window
		    double(width),		// width, in cm
		    double(height)		// height, in cm
	        ) == 0) {
		    Error::abortRun(*this, "Cannot create bar chart");
		    return;
	        }
    
	        // Put controls entries into the window
	        // First, a button to reset the taps
	        sprintf(command,
		    "ptkMakeButton %s.middle %s \"Reset taps\" %sLMSTkCB",
		    winName, butName, butName);
	        Tcl_GlobalEval(ptkInterp, command);
	    	
	        // Next, a slider to control the step size
	        sprintf(command,
		    "ptkMakeScale %s.low %s \"Step size\" %d %sLMSTkCB",
		    winName, sliderName, position, sliderName);
	        if(Tcl_GlobalEval(ptkInterp, command) != TCL_OK)
		    Error::warn(*this,"Cannot make step size control");
    
	    }
	    // display the slider value
	    sprintf(command, "%s.low.%s.value configure -text \"%.4f \"",
		    winName, sliderName, double(stepSize));
	    if(Tcl_GlobalEval(ptkInterp, command) != TCL_OK)
		    Error::warn(*this,"Cannot update step size display");

	    // set the scale position
	    sprintf(command, "%s.low.%s.scale set %d",
		    winName, sliderName, position);
	    SDFLMS :: setup();
	}
	go {
	    SDFLMS :: go();
	    if (invCount++ >= int(updateInterval)) {
		invCount = 0;
		redrawTaps();
	    }
	}
	method {
	    name { resetTaps }
	    type { void }
	    access { public }
	    arglist { "()" }
	    code {
		taps.initialize();
		redrawTaps();
	    }
	}
	method {
	    name { redrawTaps }
	    type { void }
	    access { public }
	    arglist { "()" }
	    code {
		if (ptkSetBarGraph(
			ptkInterp,		// tcl interpreter
			ptkW,			// reference window
			winName,		// window name
			&taps[0],		// data to be plotted
			taps.size(),		// number of data points
			&scale,			// full scale value
			ids			// array to store item ids
		   ) == 0)
			Error::abortRun(*this,"Cannot update bar graph.");
	    }
	}
	method {
	    name { setStepSize }
	    type { void }
	    access { public }
	    arglist { "(char *newValue)" }
	    code {
		int position;
		if(sscanf(newValue, "%d", &position) != 1) {
		    Error::abortRun(*this,"Invalid value for step size");
		    return;
		}
		stepSize = double(stepSizeLow)
		    + (double(stepSizeHigh) - double(stepSizeLow))
		    * (position/100.0);
		// display the slider value
		sprintf(command, "%s.low.%s.value configure -text \"%.4f \"",
			winName, sliderName, double(stepSize));
		if(Tcl_GlobalEval(ptkInterp, command) != TCL_OK)
		    Error::abortRun(*this,"Cannot update step size display");
	    }
	}
	code {
	    static int
	    reset(ClientData star, Tcl_Interp *interp, int argc, char **argv)
	    {
		((SDFLMSTkPlot*)star)->resetTaps();
		return TCL_OK;
	    }
	    static int
	    setStep(ClientData star, Tcl_Interp *interp, int argc, char **argv)
	    {
		((SDFLMSTkPlot*)star)->setStepSize(argv[1]);
		return TCL_OK;
	    }
	    static int
	    redraw(ClientData star, Tcl_Interp *interp, int argc, char **argv)
	    {
		((SDFLMSTkPlot*)star)->redrawTaps();
		return TCL_OK;
	    }
	}
}
