defstar
{
    name { TkStereoOut }
    domain { CGC }
    derivedFrom { StereoOut }
    desc {
Just like StereoOut, except that a Tk slider is put in the master
control panel to control the volume.
    }
    version { $Date$ $Id$ }
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
        makeScale(".low",
		  "$starSymbol(scale)",
		  "Volume control",
		  $val(volume),
                  $starSymbol(setVolume));
	displaySliderValue(".low", "$starSymbol(scale)", "  $val(volume)%");
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
	    displaySliderValue(".low", "$starSymbol(scale)", buf);
            return TCL_OK;
        }
    }
    initCode {
	CGCStereoOut :: initCode();
	addCode(tkSetup, "tkSetup");
        addCode(setVolumeDef, "procedure");
    }
}
