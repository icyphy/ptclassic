defstar {
	name {LMSCxTkPlot}
	domain {SDF}
	derivedFrom { LMSCx }
	desc { 
This is just like the LMSCx star, but with an animated
Tk display of the taps, plus associated controls.
	}
	version { $Id$ }
	author { E. A. Lee }
	copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	seealso {LMSTkPlot, LMSCx}
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
		default {"LMSCx filter taps: Real (red) & Imag (blue)"}
		desc {The string to identify the run-time display.}
	}
	defstate {
		name {updateInterval}
		type{int}
		default {"10"}
		desc {Number of invocations between display updates.}
	}
	hinclude { "BarGraph.h" }
	private {
	    // Count iterations
	    int invCount;

	    // To create unique Tcl command names, we generate unique symbols
	    static int instCount;
	    int myInst;

	    // Bar graph object
	    BarGraph bg;

	    InfString butName;
	    InfString sliderName;
	    InfString command;

	    // Keep track of whether the setup routine has run already.
	    // Avoid recreating the window if so.
	    int setupRun;
	}
	constructor {
	    // Name of the reset button
	    myInst = instCount++;
	    butName = "sdfLMSCxTkPlotreset";
	    butName += myInst;
	    sliderName = "sdfLMSCxTkPlotscale";
	    sliderName += myInst;
	    setupRun = 0;
	}
	destructor {
	    // exit if no interpreter
	    if (!ptkInterp) return;

	    // Remove Tcl commands
	    command = butName;
	    Tcl_DeleteCommand (ptkInterp, (char*)command);

	    command = sliderName;
	    Tcl_DeleteCommand(ptkInterp, (char*)command);
	}
	setup {
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

	    if (setupRun == 0 || !bg.windowExists()) {
		setupRun = 1;

		InfString idCopy((const char*)identifier);
		InfString geoCopy((const char*)geometry);
	        if(bg.setup(this,(char*)idCopy,2,taps.size(),
		    double(fullScale), - double(fullScale), (char*)geoCopy,
		    double(width), double(height)) == 0) {
			Error::abortRun(*this, "Cannot create bar chart");
			return;
	        }

		// Register the callback functions with Tcl
		command = sliderName;
		Tcl_CreateCommand (ptkInterp, (char*)command, setStep,
			(ClientData)this, NULL);
		command = butName;
		Tcl_CreateCommand(ptkInterp, (char*)command, reset,
			(ClientData)this, NULL);

	        // Put controls entries into the window
	        // First, a button to reset the taps
	        command = "ptkMakeButton ";
	        command += bg.winName;
		command += ".middle ";
	        command += butName;
		command += " {Reset taps} ";
	        command += butName;
	        Tcl_GlobalEval(ptkInterp, (char*)command);
	    	
	        // Next, a slider to control the step size
	        command = "ptkMakeScale ";
	        command += bg.winName;
		command += ".low ";
	        command += sliderName;
		command += " {step size} ";
	        command += position;
	        command += " ";
	        command += sliderName;
	        if(Tcl_GlobalEval(ptkInterp, (char*)command) != TCL_OK)
		    Error::warn(*this,"Cannot make step size control");
    
	    }
	    // display the slider value
	    command = bg.winName;
	    command += ".low.";
	    command += sliderName;
	    command += ".value configure -text {";
	    command += double(stepSize);
	    command += "}";
	    if(Tcl_GlobalEval(ptkInterp, (char*)command) != TCL_OK)
		    Error::warn(*this,"Cannot update step size display");

	    // set the scale position
	    command = bg.winName;
	    command += ".low.";
	    command += sliderName;
	    command += ".scale set ";
	    command += position;
	    if(Tcl_GlobalEval(ptkInterp, (char*)command) != TCL_OK)
		    Error::warn(*this,"Cannot set step size slider position");

	    SDFLMSCx :: setup();
	}
	go {
	    SDFLMSCx :: go();
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
		for(int i=0; i < taps.size(); i++)
		    if((bg.update(1,i,imag(taps[i])) == 0) ||
		       (bg.update(0,i,real(taps[i])) == 0))
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
		command = bg.winName;
		command += ".low.";
		command += sliderName;
		command += ".value configure -text {";
		command += double(stepSize);
		command += "}";
		if(Tcl_GlobalEval(ptkInterp, (char*)command) != TCL_OK)
		    Error::abortRun(*this,"Cannot update step size display");
	    }
	}
	code {
	    static int
	    reset(ClientData star, Tcl_Interp*, int, char*[])
	    {
		((SDFLMSCxTkPlot*)star)->resetTaps();
		return TCL_OK;
	    }
	    static int
	    setStep(ClientData star, Tcl_Interp*, int, char *argv[])
	    {
		((SDFLMSCxTkPlot*)star)->setStepSize(argv[1]);
		return TCL_OK;
	    }

	    // The following line initializes the member "instCount"
	    // when the .o file is loaded.
	    int SDFLMSCxTkPlot::instCount = 0;
	}
}
