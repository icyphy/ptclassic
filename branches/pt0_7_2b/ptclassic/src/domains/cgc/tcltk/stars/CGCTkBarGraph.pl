defstar {
	name {TkBarGraph}
	domain {CGC}
	desc { 
Dynamically display the value of any number of input signals in bar-chart form.
The first 12 input signals will be assigned distinct colors. After that,
the colors are repeated. The colors can be controlled using X resources.
	}
	version { $Id$ }
	author { E. A. Lee, Jose Luis Pino }
	copyright {
Copyright (c) 1990-1994 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { CGC tcltk library }
	input {
		name {input}
		type {float}
		desc { Any number of inputs to plot }
	}
	defstate {
		name {label}
		type {string}
		default {"bar chart display"}
		desc {A label to put on the display}
	}
	defstate {
		name {top}
		type {float}
		default { "1.0" }
		attributes {A_GLOBAL}
		desc {High end of the scale}
	}
	state {
		name {bottom}
		type {float}
		default { "-1.0" }
		attributes {A_GLOBAL}
		desc {Low end of the scale}
	}
	defstate {
		name {number_of_bars}
		type {int}
		default { 16 }
		desc { Number of bars to display }
	}
	defstate {
		name {bar_graph_height}
		type {float}
		default { "5" }
		desc { Desired height of the bar graph in centimeters }
	}
	defstate {
		name {bar_graph_width}
		type {float}
		default { "10" }
		desc { Desired width of the bar graph in centimeters }
	}
	defstate {
		name {position}
		type {string}
		default { "+0+0" }
		desc { Width of the bar graph in centimeters }
	}
	setup {
	    if(float(top) <= float(bottom)) {
		Error::abortRun(*this, "invalid range for the scale");
	    }
        }
	initCode {
	    addGlobal("int $starSymbol(invCount);");
	    addGlobal("int $starSymbol(ids)[$val(number_of_bars)];");
	    addGlobal("double $starSymbol(buffer)[$val(number_of_bars)];");
	    addGlobal("double* $starSymbol(buffer_ptr);");
	    addDeclaration("int $starSymbol(count) = 0;");
	    addGlobal("int* $starSymbol(ids_ptr);");
	    addCode("$starSymbol(buffer_ptr)=$starSymbol(buffer);","tkSetup");
	    addCode("$starSymbol(ids_ptr) = $starSymbol(ids);","tkSetup");
	    addCode("$ref(top) = $val(top);\n","tkSetup");
	    addCode("$ref(bottom) = $val(bottom);\n","tkSetup");
	    addCode(makeGraph,"tkSetup");
	    addCode(procDefs,"procedure");
	}
	go {
	    addCode(updateDisplay);
	}
	codeblock (updateDisplay) {
	    $starSymbol(buffer)[$starSymbol(count)++] = $ref(input);
	    if ($starSymbol(count) == $val(number_of_bars))
		    $starSymbol(count) = 0;
	    if ($starSymbol(invCount)++ == $val(number_of_bars)) {
		$starSymbol(invCount) = 0;
		if(ptkSetBarGraph(interp, &w,
			    "$starSymbol(.bar)",
			    &$starSymbol(buffer_ptr),
			    1,
			    $val(number_of_bars),
			    $ref(top),
			    $ref(bottom),
			    &$starSymbol(ids_ptr)) == 0)
		    errorReport("Cannot update bar graph.");
	    }
        }
	codeblock (makeGraph) {
	    Tcl_CreateCommand(interp, "$starSymbol(.bar)rescale",
			$starSymbol(rescale),
			(ClientData) 0, (void (*)()) NULL);
	    if(ptkMakeBarGraph(interp, &w,
			 "$starSymbol(.bar)",	/* name of top level window */
			 "$val(label)",	        /* identifying string */
			 &$starSymbol(buffer_ptr),/* data to be plotted */
			 1,			/* one trace only */
			 $val(number_of_bars),	/* number of data points */
			 $ref(top),	        /* top of scale */
			 $ref(bottom),	        /* bottom of scale */
			 &$starSymbol(ids_ptr),	/* array to store item ids */
			 "$val(position)",	/* position window*/
			 $val(bar_graph_width),	/* width, in cm */
			 $val(bar_graph_height)) == 0)
		errorReport("Cannot create bar chart");
	    $starSymbol(invCount) = 0;
	    Tcl_CreateCommand(interp, "$starSymbol(.bar)redraw",
			$starSymbol(redraw),
			(ClientData) 0, (void (*)()) NULL);
	}
	codeblock (procDefs) {
	    static int
	    $starSymbol(redraw)(dummy, interp, argc, argv)
		ClientData dummy;		   /* Not used. */
		Tcl_Interp *interp;		 /* Current interpreter. */
		int argc;			   /* Number of arguments. */
		char **argv;			/* Argument strings. */
	    {
		if(ptkSetBarGraph(interp, &w,
			    "$starSymbol(.bar)",
			    &$starSymbol(buffer_ptr),
			    1,
			    $val(number_of_bars),
			    $ref(top),
			    $ref(bottom),
			    &$starSymbol(ids_ptr)) == 0)
		    errorReport("Cannot redraw bar graph");
		return TCL_OK;
	    }

	    static int
	    $starSymbol(rescale)(dummy, interp, argc, argv)
		ClientData dummy;		   /* Not used. */
		Tcl_Interp *interp;		 /* Current interpreter. */
		int argc;			   /* Number of arguments. */
		char **argv;			/* Argument strings. */
	    {
		float temp;
		if(sscanf(argv[1], "%4f", &temp) != 1) {
		    Tcl_AppendResult(interp,
			"Cannot reset full scale in bar graph",(char*) NULL);
		    return TCL_ERROR;
		}
		$ref(top) *=  temp;
		$ref(bottom) *=  temp;
	        /* Return new full scale value to tcl */
		sprintf(interp->result, "%f %f",
		    $ref(top), $ref(bottom));
		return TCL_OK;
	    }

    }
}
