defstar {
	name {TkSlider}
	domain {CGC}
	desc { Output a value determined by a Tk scale slider.  }
	version { $Id$ }
	author { E. A. Lee }
	copyright {
Copyright (c) 1993 The Regents of the University of California.
All rights reserved.
See the file ~ptolemy/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { CGC tcltk library }
	output {
		name {output}
		type {float}
	}
	defstate {
		name {low}
		type{float}
		default {"0.0"}
		desc {The low end of the scale.}
	}
	defstate {
		name {high}
		type{float}
		default {"1.0"}
		desc {The high end of the scale.}
	}
	defstate {
		name {value}
		type{float}
		default {"0.0"}
		desc {The value to send to the output.}
		attributes {A_GLOBAL}
	}
	defstate {
		name {identifier}
		type{string}
		default {"Scale"}
		desc {The string to identify the slider in the control panel.}
	}
	initCode {
	    addCode(tkSetup,"tkSetup");
	    addCode(setValueDef, "procedure");
	}
	go {
	    addCode("\t$ref(output) = $ref(value);\n");
	}

	// Note that window names cannot start with upper case letters
	codeblock (tkSetup) {
	    /* Establish the Tk window for setting the value */
	    {
	    int position;

	    /* compute the initial position of the slider */
	    position = 0.5 + 100*($val(value)
			- $val(low))/($val(high)-$val(low));

	    makeScale("cp_low",			/* position in control panel */
	              "$starSymbol(scale)",	/* name of the scale pane */
		      "$val(identifier)",	/* text identifying the pane */
		      position,			/* initial position */
		      $starSymbol(setValue));	/* callback procedure */
	    }

	    /* set the initial value display */
	    displaySliderValue("cp_low", "$starSymbol(scale)", "$val(value)");
	}

	// Note that the argument to this function will an integer
	// between 0 and 100 indicating the position of the slider.
	// This function has to convert that position into a value.
	codeblock (setValueDef) {
	    static int
	    $starSymbol(setValue)(dummy, interp, argc, argv)
                ClientData dummy;                   /* Not used. */
                Tcl_Interp *interp;                 /* Current interpreter. */
                int argc;                           /* Number of arguments. */
                char **argv;                        /* Argument strings. */
            {
		int position;
		/* FIX ME: Is the following buffer big enough always? */
		static char* buf[20];
                if(sscanf(argv[1], "%d", &position) != 1) {
                    errorReport("Invalid value");
                    return TCL_ERROR;
                }
		$ref(value) = $val(low)+($val(high)-$val(low))*(position/100.0);
		/* set the value display */
		sprintf(buf, "%.4f", $ref(value));
		displaySliderValue("cp_low", "$starSymbol(scale)", buf);
                return TCL_OK;
            }
	}
}
