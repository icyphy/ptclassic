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
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
    }

    codeblock (tkSetup) {
      /* Establish the Tk window for setting the value    */
      /* "position" is  a local variable which scales the */
      /* volume from (0 - 10) to (0 - 100) for makeScale  */ 
      {
	int position;
	position = $val(volume)*10;
        makeScale(".high",
		  "$starSymbol(scale1)",
		  "Volume control",
		  position,
                  $starSymbol(setVolume));
	displaySliderValue(".high", "$starSymbol(scale1)",
			   "$val(volume)");
      }
      /* "tkbalance" is  a local variable which scales the    */
      /* balance from (-10 to 10) to (0 to 100) for makeScale */ 
      {
	int tkbalance;
	tkbalance = 5*$val(balance)+50;
	makeScale(".low",
		  "$starSymbol(scale2)",
		  "Balance control",
		  tkbalance,
                  $starSymbol(setBalance));
	displaySliderValue(".low", "$starSymbol(scale2)", "  $val(balance)");
      }
    }
    codeblock (setVolumeDef) {
        static int $starSymbol(setVolume)(dummy, interp, argc, argv)
            ClientData dummy;                   /* Not used. */
            Tcl_Interp *interp;                 /* Current interpreter. */
            int argc;                           /* Number of arguments. */
            char **argv;                        /* Argument strings. */
        {
	    audio_info_t info;
	    static char buf[20];
	    int position;
            if(sscanf(argv[1], "%d", &position) != 1) {
                errorReport("Invalid volume");
                return TCL_ERROR;
            }
	    /* conveying the value changes of volume to audioctl */
	    $ref(volume) = (int) position/10;
	    AUDIO_INITINFO(&info);
	    ioctl($starSymbol(ctlfile), AUDIO_GETINFO,(caddr_t)(&info));
	    info.play.gain = AUDIO_MAX_GAIN*$ref(volume)/10;
	    ioctl($starSymbol(ctlfile), AUDIO_SETINFO, (caddr_t)(&info));

	    sprintf(buf, "%d", $ref(volume));
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
	    audio_info_t info;
	    static char buf[20];
	    int tkbalance;
            if(sscanf(argv[1], "%d", &tkbalance) != 1) {
                errorReport("Invalid balance");
                return TCL_ERROR;
            }
	    $ref(balance) = ((int)tkbalance/5) - 10;
	    /* conveying the value changes of balance to audioctl */
	    AUDIO_INITINFO(&info);
	    ioctl($starSymbol(ctlfile), AUDIO_GETINFO, (caddr_t)(&info));
	    info.play.balance = AUDIO_MID_BALANCE*($ref(balance)+10)/10;
	    ioctl($starSymbol(ctlfile), AUDIO_SETINFO, (caddr_t)(&info));
	    
	    sprintf(buf, "%d", $ref(balance));
	    displaySliderValue(".low", "$starSymbol(scale2)", buf);
            return TCL_OK;
        }
    }

    initCode {
	CGCStereoOut :: initCode();
	addCode(tkSetup, "tkSetup");
        addCode(setVolumeDef, "procedure");
        addCode(setBalanceDef, "procedure");
    }
}
