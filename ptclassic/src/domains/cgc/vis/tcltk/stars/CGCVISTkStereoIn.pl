defstar
{
    name { VISTkStereoIn }
    domain { CGC }
    derivedFrom { VISStereoIn }
    desc {
Just like StereoIn, except that a Tk slider is put in the master
control panel to control the volume.
    }
    version { $Id$ }
    author { Sunil Bhave }
    location { CGCVIS Tcl/Tk library }
    copyright {
Copyright (c) 1996-%Q% The Regents of the University of California.
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
      /* "tkvolume" is  a local variable which scale the */
      /* volume from (0 - 10) to (0 - 100) for makeScale */ 
      {	
	int tkvolume;
        tkvolume = (int)($val(volume)*100);
        makeScale(".high",
		  "$starSymbol(scale1)",
		  "Record volume control",
		  tkvolume,
		  $starSymbol(setVolume));
	displaySliderValue(".high", "$starSymbol(scale1)",
			   "$val(volume)");
      }

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
	  /* conveying the value changes of record volume to audio device */
	  $ref(volume) = (float) (tkvolume/100.0);
	  $sharedSymbol(CGCAudioBase, audio_gain)
	    ($starSymbol(file), $ref(volume), 1);
	  
	  sprintf(buf, "%f", $ref(volume));
	  displaySliderValue(".high", "$starSymbol(scale1)", buf);
	  
	  return TCL_OK;
        }
    }

    initCode {
	CGCVISStereoIn :: initCode();
	addCode(tkSetup, "tkSetup");
        addCode(setVolumeDef, "procedure");
    }
}
