defstar
{
    name { TkStereoOut }
    domain { CGC }
    derivedFrom { StereoOut }
    desc {
Just like StereoOut, except that a Tk slider is put in the master
control panel to control the volume and balance.
    }
    version { $Id$ }
    author { Sunil Bhave }
    location { CGC Tcl/Tk library }
    copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
    }

    defstate {
      name { starName }
      type { string }
      default { "output" }
      desc { The user-friendly name of this star.}
    }

    codeblock (tkSetup) {
      /* Establish the Tk window for setting the value    */
      /* "tkvolume" is  a local variable which scales the */
      /* volume from (0 - 10) to (0 - 100) for makeScale  */ 
      {
	int tkvolume;
	tkvolume = (int) ($val(volume)*100);
        makeScale(".high",
		  "$starSymbol(scale1)",
		  "Volume control",
		  tkvolume,
                  $starSymbol(setVolume));
	displaySliderValue(".high", "$starSymbol(scale1)",
			   "$val(volume)");
      }
      /* "tkbalance" is  a local variable which scales the    */
      /* balance from (-10 to 10) to (0 to 100) for makeScale */ 
      {
	int tkbalance;
	tkbalance = (int)(50*$val(balance)+50);
	makeScale(".low",
		  "$starSymbol(scale2)",
		  "Balance control",
		  tkbalance,
                  $starSymbol(setBalance));
	displaySliderValue(".low", "$starSymbol(scale2)", "  $val(balance)");
      }
    }

    codeblock (tychoSetup) {
	/* Call tycho to connect a control to the volume and
	   balance parameters */
	connectControl(moduleName, "$val(starName)", "volume",
		$starSymbol(setVolume));
	connectControl(moduleName, "$val(starName)", "balance",
		$starSymbol(setBalance));
    }

    codeblock (setVolumeDef) {
        static int $starSymbol(setVolume)(dummy, interp, argc, argv)
            ClientData dummy;                   /* Not used. */
            Tcl_Interp *interp;                 /* Current interpreter. */
            int argc;                           /* Number of arguments. */
            char **argv;                        /* Argument strings. */
        {
	    static char buf[20];
	    int tkvolume;
            if(sscanf(argv[1], "%d", &tkvolume) != 1) {
                errorReport("Invalid volume");
                return TCL_ERROR;
            }
	    /* conveying the value changes of volume to audio device */
	    $ref(volume) = (float) (tkvolume/100.0);
	    $sharedSymbol(CGCAudioBase, audio_gain)
	    ($starSymbol(file), $ref(volume), 0);

	    sprintf(buf, "%f", $ref(volume));
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
	    int tkbalance;
            if(sscanf(argv[1], "%d", &tkbalance) != 1) {
                errorReport("Invalid balance");
                return TCL_ERROR;
            }
	    $ref(balance) = (float) ((tkbalance/50.0) - 1.0);
	    /* conveying the value changes of balance to audio device */
	    $sharedSymbol(CGCAudioBase, audio_balance)
	      ($starSymbol(file), $ref(balance));

	    sprintf(buf, "%f", $ref(balance));
	    displaySliderValue(".low", "$starSymbol(scale2)", buf);

            return TCL_OK;
        }
    }

    initCode {
      CGCStereoOut :: initCode();
      addCode(tkSetup, "tkSetup");
      addCode(tychoSetup, "tychoSetup");
      addCode(setVolumeDef, "procedure");
      addCode(setBalanceDef, "procedure");
    }
}
