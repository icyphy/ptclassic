defstar
{
    name { TkParamBiquad }
    domain { CGC }
    derivedFrom { ParamBiquad }
    desc {
Just like ParamBiquad, except that a Tk slider is put in the master
control panel to control the gain.
    }
    version { $Id$ }
    author { William Chen }
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
	tkgain = (int) 5*$val(gain)+50;
	makeScale(".low",
		  "$starSymbol(scale2)",
		  "Gain control",
		  tkgain,
                  $starSymbol(setGain));
	displaySliderValue(".low", "$starSymbol(scale2)", "  $val(gain)");
      }
      /* "position" is  a local variable which scales the    */
      /* bandwidth from (0 to 4) to (0 to 100) for makeScale */ 
      {
	int position;
	position = (int) $val(bandwidth)*25;
        makeScale(".high",
		  "$starSymbol(scale1)",
		  "Bandwidth control",
		  position,
                  $starSymbol(setBandwidth));
	displaySliderValue(".high", "$starSymbol(scale1)",
			   "$val(bandwidth)");
      }      
    }

    codeblock (setGainDef) {
        static int $starSymbol(setGain)(dummy, interp, argc, argv)
            ClientData dummy;                   /* Not used. */
            Tcl_Interp *interp;                 /* Current interpreter. */
            int argc;                           /* Number of arguments. */
            char **argv;                        /* Argument strings. */
        {
	    static char buf[20];
	    int tkgain;
            if(sscanf(argv[1], "%d", &tkgain) != 1) {
                errorReport("Invalid gain");
                return TCL_ERROR;
            }
	    /* set the new gain  */
	    $ref(gain) = tkgain/5.0 - 10.0;
	    
	    $sharedSymbol(CGCParamBiquad,setparams)(&$starSymbol(parametric));
	    if ($ref(filtertype) == LOW){
	      $sharedSymbol(CGCParamBiquad,lowpass)(&$starSymbol(parametric),$starSymbol(filtercoeff));
	    }
	    else if ($ref(filtertype) == HI){
	      $sharedSymbol(CGCParamBiquad,hipass)(&$starSymbol(parametric),$starSymbol(filtercoeff));
	    }
	    else if ($ref(filtertype) == BAND){
	      $sharedSymbol(CGCParamBiquad,bandpass)(&$starSymbol(parametric),$starSymbol(filtercoeff));
	    }
	    $sharedSymbol(CGCParamBiquad,setfiltertaps)(&$starSymbol(parametric),$starSymbol(filtercoeff),$starSymbol(filtertaps));
    
	    sprintf(buf, "%.2f", $ref(gain));
	    displaySliderValue(".low", "$starSymbol(scale2)", buf);
            return TCL_OK;
        }
    }
    
    codeblock (setBandwidthDef) {
        static int $starSymbol(setBandwidth)(dummy, interp, argc, argv)
            ClientData dummy;                   /* Not used. */
            Tcl_Interp *interp;                 /* Current interpreter. */
            int argc;                           /* Number of arguments. */
            char **argv;                        /* Argument strings. */
        {
	    static char buf[20];
	    int position;
            if(sscanf(argv[1], "%d", &position) != 1) {
                errorReport("Invalid bandwidth");
                return TCL_ERROR;
            }
	    /* conveying the value changes of bandwidth to audioctl */
	    $ref(bandwidth) = position/25.0;

	    $sharedSymbol(CGCParamBiquad,setparams)(&$starSymbol(parametric));
	    if ($ref(filtertype) == LOW){
	      $sharedSymbol(CGCParamBiquad,lowpass)(&$starSymbol(parametric),$starSymbol(filtercoeff));
	    }
	    else if ($ref(filtertype) == HI){
	      $sharedSymbol(CGCParamBiquad,hipass)(&$starSymbol(parametric),$starSymbol(filtercoeff));
	    }
	    else if ($ref(filtertype) == BAND){
	      $sharedSymbol(CGCParamBiquad,bandpass)(&$starSymbol(parametric),$starSymbol(filtercoeff));
	    }
	    $sharedSymbol(CGCParamBiquad,setfiltertaps)(&$starSymbol(parametric),$starSymbol(filtercoeff),$starSymbol(filtertaps));

	    sprintf(buf, "%.2f", $ref(bandwidth));
	    displaySliderValue(".high", "$starSymbol(scale1)", buf);
            return TCL_OK;
        }
    }

    initCode {
	CGCParamBiquad :: initCode();
	addCode(tkSetup, "tkSetup");
        addCode(setGainDef, "procedure");
        addCode(setBandwidthDef, "procedure");
    }
}

