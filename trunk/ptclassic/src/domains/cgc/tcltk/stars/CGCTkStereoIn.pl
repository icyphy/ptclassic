defstar
{
    name { TkStereoIn }
    domain { CGC }
    derivedFrom { StereoIn }
    desc {
Just like StereoIn, except that a Tk slider is put in the master
control panel to control the volume.
    }
    version { $Id$ }
    author { Sunil Bhave }
    location { CGC Tcl/Tk library }
    copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
    }

    codeblock (tkSetup) {
        /* Establish the Tk window for setting the value */
        makeScale(".high",
		  "$starSymbol(scale1)",
		  "Volume control",
		  $val(volume),
                  $starSymbol(setVolume));
	displaySliderValue(".high", "$starSymbol(scale1)",
			   "$val(volume)%");
	makeScale(".low",
		  "$starSymbol(scale2)",
		  "Balance control",
		  $val(balance),
                  $starSymbol(setBalance));
	displaySliderValue(".low", "$starSymbol(scale2)", "  $val(balance)%");

    }
    codeblock (setVolumeDef) {
        static int $starSymbol(setVolume)(dummy, interp, argc, argv)
            ClientData dummy;                   /* Not used. */
            Tcl_Interp *interp;                 /* Current interpreter. */
            int argc;                           /* Number of arguments. */
            char **argv;                        /* Argument strings. */
        {
	    static char buf[20];
            if(sscanf(argv[1], "%d", &$ref(volume)) != 1) {
                errorReport("Invalid volume");
                return TCL_ERROR;
            }
            $starSymbol(set_parameters) ();
	    sprintf(buf, "%5d%%", $ref(volume));
	    displaySliderValue(".high", "$starSymbol(scale1)", buf);
            return TCL_OK;
        }
    }

    codeblock (setBalanceDef) {
        static int $starSymbol(setBalance)(dummy, interp, argc, argv)
            ClientData dummy;                   /* Not used. */
            Tcl_Interp *interp;                 /* Current interpreter. */
            int argc;                           /* Number of arguments. */
            char **argv;                        /* Argument strings. */
        {
	    static char buf[20];
            if(sscanf(argv[1], "%d", &$ref(balance)) != 1) {
                errorReport("Invalid balance");
                return TCL_ERROR;
            }
            $starSymbol(set_parameters) ();
	    sprintf(buf, "%5d%%", $ref(balance));
	    displaySliderValue(".low", "$starSymbol(scale2)", buf);
            return TCL_OK;
        }
    }




    initCode {
	CGCStereoIn :: initCode();
	addCode(tkSetup, "tkSetup");
        addCode(setVolumeDef, "procedure");
        addCode(setBalanceDef, "procedure");
    }
}
