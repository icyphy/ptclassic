defstar {
	name {TkLMS}
	domain {CGC}
	derivedFrom { LMS }
	desc { 
Just like the LMS filter, but with an animated Tk display of
the taps, plus associated controls.
	}
	version { $Id$ }
	author { E. A. Lee }
	copyright {
Copyright (c) 1993 The Regents of the University of California.
All rights reserved.
See the file ~ptolemy/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	seealso {LMS}
	location { CGC tcltk library }
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
		attributes { A_GLOBAL }
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
	setup {
	    taps.setAttributes(A_GLOBAL);
	    stepSize.setAttributes(A_GLOBAL);
	    CGCLMS::setup();
	}
	initCode {
            addGlobal("int $starSymbol(invCount);");
            addGlobal("int $starSymbol(ids)[$val(tapSize)];");

	    // initialize backup values to equal taps initial values.
            addGlobal("double $starSymbol(backup)[$val(tapSize)] = { ");
	    StringList initvalues;
	    for (int i = 0; i < taps.size()-1; i++) {
		initvalues << taps[i] << ", ";
	    }
	    initvalues << taps[taps.size()-1] << "};";
	    addGlobal(initvalues);

	    addCode(makeChart,"tkSetup");
	    addCode(makeControls,"tkSetup");
	    addCode(procDefs,"procedure");
	    CGCLMS :: initCode();
	    addCode(copyTaps);
	}
	go {
	    CGCLMS :: go();
	    addCode(updateDisplay);
	}
        codeblock (updateDisplay) {
            if ($starSymbol(invCount)++ >= $val(updateInterval)) {
                $starSymbol(invCount) = 0;
                setBarChart("$starSymbol(bar)",
			    $ref(taps),
                            $val(tapSize),
			    &$ref(fullScale),
			    $starSymbol(ids));
            }
        }
        codeblock (makeChart) {
	    makeBarChart("$starSymbol(bar)",	/* name of top level window */
			 "$val(identifier)",	/* identifying string */
			 $ref(taps),		/* data to be plotted */
			 $val(tapSize),		/* number of data points */
			 &$ref(fullScale),	/* full scale value */
			 $starSymbol(ids),	/* array to store item ids */
			 "$val(geometry)",	/* shape and position, window*/
			 $val(width),		/* width, in cm */
			 $val(height));		/* height, in cm */
            $starSymbol(invCount) = 0;
        }
	codeblock (makeControls) {
	    makeButton("$starSymbol(bar).cp_middle",	/* location of button */
		       "$starSymbol(reset)",		/* name of button */
		       "Reset taps",			/* identifier */
		       $starSymbol(reset));		/* callback procedure */
	    {
	    int position;
	    /* Compute slider position from parameters */
	    position = 0.5 + 100*($val(stepSize)
                        - $val(stepSizeLow))/($val(stepSizeHigh)
			- $val(stepSizeLow));

	    /* Make a slider for controlling the step size */
	    makeScale("$starSymbol(bar).cp_low",	/* location of scale */
		      "$starSymbol(scale)",		/* name of scale */
		      "Step size",			/* identifier */
		      position,				/* initial position */
		      $starSymbol(setStep));		/* callback */

	    /* set the initial step size display */
	    displaySliderValue("$starSymbol(bar).cp_low",
			       "$starSymbol(scale)",
			       "$val(stepSize)");
	    }
	}
	codeblock (copyTaps) {
	{
	    int i;
            for (i=0; i<$val(tapSize); i++) {
		$starSymbol(backup)[i] = $ref(taps)[i];
	    }
	}
	}
	codeblock (procDefs) {
	    static int
	    $starSymbol(reset)(dummy, interp, argc, argv)
                ClientData dummy;                   /* Not used. */
                Tcl_Interp *interp;                 /* Current interpreter. */
                int argc;                           /* Number of arguments. */
                char **argv;                        /* Argument strings. */
            {
		int i;
                for (i=0; i<$val(tapSize); i++) {
		    $ref(taps)[i] = $starSymbol(backup)[i];
		}
                $starSymbol(invCount) = 0;
                setBarChart("$starSymbol(bar)",
			    $ref(taps),
                            $val(tapSize),
			    &$ref(fullScale),
			    $starSymbol(ids));
                return TCL_OK;
            }

	    static int
	    $starSymbol(setStep)(dummy, interp, argc, argv)
                ClientData dummy;                   /* Not used. */
                Tcl_Interp *interp;                 /* Current interpreter. */
                int argc;                           /* Number of arguments. */
                char **argv;                        /* Argument strings. */
            {
		int position;
		/* FIX ME: Is the following buffer big enough always? */
                static char* buf[20];

		if(sscanf(argv[1], "%d", &position) != 1) {
		    errorReport("Invalid value for step size");
                    return TCL_ERROR;
		}
		$ref(stepSize) = $val(stepSizeLow)
		    + ($val(stepSizeHigh) - $val(stepSizeLow))
		    * (position/100.0);

	        /* set the step size display */
		sprintf(buf, "%.4f", $ref(stepSize));
	        displaySliderValue("$starSymbol(bar).cp_low",
			           "$starSymbol(scale)",
			           buf);
                return TCL_OK;
            }
	}
}
