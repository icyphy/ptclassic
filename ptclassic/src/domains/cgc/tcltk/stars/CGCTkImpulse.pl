defstar {
	name {TkImpulse}
	domain {CGC}
	desc {
Output a specified value when a button is pushed.
Optionally synchronize by halting until the button is pushed.
        }
	version { $Id$ }
	author { E. A. Lee }
	copyright {
Copyright (c) 1990-1994 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { CGC tcltk library }
	output {
		name {output}
		type {float}
	}
	defstate {
		name { level }
		type { float }
		default { 1.0 }
		desc { The height of the impulse. }
		attributes { A_GLOBAL }
	}
	defstate {
		name {identifier}
		type{string}
		default {"Impulse"}
		desc {The string to identify the slider in the control panel.}
	}
	defstate {
	        name {synchronize}
		type {int}
		default {"NO"}
		desc {Halt until the user presses the button.}
	}

	initCode {
	    addCode(tkSetup,"tkSetup");
	    addCode(pulseDef, "procedure");
	    addCode("$ref(level) = 0.0;");
	}
	go {
	  if (int(synchronize)) addCode(sync);
	  else addCode(nosync);
	}
	codeblock (nosync) {
	    $ref(output) = $ref(level);
	    $ref(level) = 0.0;
        }
	codeblock (sync) {
	    while ($ref(level) != $val(level)) {
	      /* The following will sleep until there is an event to process */
	      Tk_DoOneEvent(0);
	    }
	    $ref(output) = $ref(level);
	    /* Use level as a flag to */
	    $ref(level) = $val(level) - 1.0;
	}
	// Note that window names cannot start with upper case letters
	codeblock (tkSetup) {
	    /* Establish the Tk button */
	    makeButton(".middle",	/* location in the control panel */
		       "$starSymbol(button)",	/* name of the button */
		       "$val(identifier)",	/* label on the button */
		       $starSymbol(pulse));	/* callback procedure */
	}
	codeblock (pulseDef) {
	    static int
	    $starSymbol(pulse)(dummy, interp, argc, argv)
                ClientData dummy;                   /* Not used. */
                Tcl_Interp *interp;                 /* Current interpreter. */
                int argc;                           /* Number of arguments. */
                char **argv;                        /* Argument strings. */
            {
                $ref(level) = $val(level);
                return TCL_OK;
            }
	}
}
