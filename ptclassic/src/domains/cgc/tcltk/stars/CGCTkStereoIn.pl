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

    defstate {
      name { starName }
      type { string }
      default { "input" }
      desc { The user-friendly name of this star.}
    }

    codeblock (tkSetup) {
      /* Establish the Tk window for setting the value   */
      /* "position" is  a local variable which scale the */
      /* volume from (0 - 10) to (0 - 100) for makeScale */ 
      {	
	int position;
        position = $val(volume)*10;
        makeScale(".high",
		  "$starSymbol(scale1)",
		  "Record volume control",
		  position,
		  $starSymbol(setVolume));
	displaySliderValue(".high", "$starSymbol(scale1)",
			   "$val(volume)");
      }

      /* Test code for the new custom control panels */
      connectControl("$val(starName)",
		     "attenuate",
		     $starSymbol(setVolume));
    }

    codeblock (setVolumeDef) {
        static int $starSymbol(setVolume)(dummy, interp, argc, argv)
	    ClientData dummy;                   /* Not used. */
            Tcl_Interp *interp;                 /* Current interpreter. */
            int argc;                           /* Number of arguments. */
            char **argv;                        /* Argument strings. */
        {
	    static char buf[20];
	    int position;
            if(sscanf(argv[1], "%d", &position) != 1) {
                errorReport("Invalid volume");
                return TCL_ERROR;
            }
	    /* conveying the value changes of record volume to audio device */
	    $ref(volume) = (int) position/10;
	    $sharedSymbol(CGCStereoBase,set_parameters)
	      ($starSymbol(file), "$val(inputPort)", $ref(volume),
	       $ref(balance), 1);

	    sprintf(buf, "%d", $ref(volume));
	    displaySliderValue(".high", "$starSymbol(scale1)", buf);

            return TCL_OK;
        }
    }

    initCode {
	CGCStereoIn :: initCode();
	addCode(tkSetup, "tkSetup");
        addCode(setVolumeDef, "procedure");
    }
}

