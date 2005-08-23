defstar
{
    name { TkParamBiquad }
    domain { CGC }
    derivedFrom { ParamBiquad }
    desc {
Just like ParamBiquad, except that a Tk slider is put in the master
control panel to control the gain.
    }
    version { @(#)CGCTkStereoIn.pl	1.4 07/14/96 }
    author { Sunil Bhave }
    location { CGC Tcl/Tk library }
    copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
    }

    codeblock (tkSetup) {
      /* Establish the Tk window for setting the value    */
      /* "tkgain" is  a local variable which scales the    */
      /* gain from (-10 to 10) to (0 to 100) for makeScale */ 
      {
	int tkgain;
	tkgain = 5*$val(gain)+50;
	makeScale(".low",
		  "$starSymbol(scale2)",
		  "Gain control",
		  tkgain,
                  $starSymbol(setGain));
	displaySliderValue(".low", "$starSymbol(scale2)", "  $val(gain)");
      }
    }

    codeblock (setGainDef) {
        static int $starSymbol(setGain)(dummy, interp, argc, argv)
            ClientData dummy;                   /* Not used. */
            Tcl_Interp *interp;                 /* Current interpreter. */
            int argc;                           /* Number of arguments. */
            char **argv;                        /* Argument strings. */
        {
	    audio_info_t info;
	    static char buf[20];
	    int tkgain;
            if(sscanf(argv[1], "%d", &tkgain) != 1) {
                errorReport("Invalid gain");
                return TCL_ERROR;
            }
	    /* set the new gain  */
	    $ref(gain) = ((int)tkgain/5) - 10;
	    
	    sprintf(buf, "%d", $ref(gain));
	    displaySliderValue(".low", "$starSymbol(scale2)", buf);
            return TCL_OK;
        }
    }

    initCode {
	CGCStereoOut :: initCode();
	addCode(tkSetup, "tkSetup");
        addCode(setGainDef, "procedure");
    }
}

