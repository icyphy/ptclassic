defstar {
	name {TkEntry}
	domain {CGC}
	desc { Output a constant signal with value determined by a Tk entry box (default 0.0).  }
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
		name {value}
		type{float}
		default {"0.0"}
		desc {The value to send to the output.}
		attributes { A_GLOBAL }
	}
	defstate {
		name {identifier}
		type{string}
		default {"Entry"}
		desc {The string to identify the entry in the control panel.}
	}
	initCode {
	    addCode(setup,"tkSetup");
	    addCode(setValueDef, "procedure");
	}
	go {
		addCode("\t$ref(output) = $ref(value);\n");
	}

	// Note that window names cannot start with upper case letters
	codeblock (setup) {
	    /* Establish the Tk window for setting the value */
	    makeEntry("cp_high",		/* position in control panel */
		      "$starSymbol(entry)",	/* name of entry pane */
		      "$val(identifier)",	/* text identifying the pane */
		      "$val(value)",		/* initial value of the entry */
		      $starSymbol(setValue));	/* callback procedure */
	}

	// Define the callback procedure invoked when the entry is changed.
	// A Return in the entry triggers this.
	codeblock (setValueDef) {
	    static int
            $starSymbol(setValue)(dummy, interp, argc, argv)
                ClientData dummy;                   /* Not used. */
                Tcl_Interp *interp;                 /* Current interpreter. */
                int argc;                           /* Number of arguments. */
                char **argv;                        /* Argument strings. */
            {
		float temp;
                if(sscanf(argv[1], "%f", &temp) != 1) {
                    errorReport("Invalid value");
                    return TCL_ERROR;
                }
		$ref(value) = temp;
                return TCL_OK;
            }
	}
}
