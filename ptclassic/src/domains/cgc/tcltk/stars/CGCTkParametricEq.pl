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
    author { William Chen and Sunil Bhave }
    location { CGC Tcl/Tk library }
    copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
    }

    defstate {
      name { highcenterFreq }
      type { float }
      default { "20000" }
      desc { The highest possible center frequency. }
    }
    
    defstate {
      name {lowcenterFreq}
      type { float }
      default { "1" }
      desc { the lowest possible center frequency. }
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
      /* bandwidth from (0.1 to 4) to (0 to 100) for makeScale */ 
      {
	int position;
	position = (int) ($val(bandwidth)-0.01)*100/3.99;
        makeScale(".high",
		  "$starSymbol(scale1)",
		  "Bandwidth control",
		  position,
                  $starSymbol(setBandwidth));
	displaySliderValue(".high", "$starSymbol(scale1)",
			   "$val(bandwidth)");
      }      
      /* "tkfreq" is a local variable which scale the center  */
      /* freq from (lowcenterFreq to highcenterFreq) to 0-100 */
      {
	int tkfreq;
	tkfreq = (int)
	  100*($val(centerFreq)-$val(lowcenterFreq))/
	  ($val(highcenterFreq) - $val(lowcenterFreq));
	makeScale(".middle",
		  "$starSymbol(scale3)",
		  "Center Frequency control",
		  tkfreq,
                  $starSymbol(setcenterFreq));
	displaySliderValue(".middle", "$starSymbol(scale3)",
			   "$val(centerFreq)");
      }
      {	
	int tkpass;
	tkpass = (int)
	  100*($val(passFreq)-$val(lowcenterFreq))/
	  ($val(highcenterFreq) - $val(lowcenterFreq));
	makeScale(".middle",
		  "$starSymbol(scale4)",
		  "Pass Frequency control",
		  tkpass,
		  $starSymbol(setpassFreq));
	displaySliderValue(".middle", "$starSymbol(scale4)",
			   "$val(passFreq)");
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
	    
	    $starSymbol(setparams)(&$starSymbol(parametric));
	    if (strcasecmp("$val(filtertype)", "LOW") == 0){
	      $sharedSymbol(CGCParamBiquad,lowpass)(&$starSymbol(parametric),
				   $starSymbol(filtercoeff));
	    }
	    else if (strcasecmp("$val(filtertype)", "HI") == 0){
	      $sharedSymbol(CGCParamBiquad,hipass)(&$starSymbol(parametric),
				  $starSymbol(filtercoeff));
	    }
	    else if (strcasecmp("$val(filtertype)", "BAND") == 0){
	      $sharedSymbol(CGCParamBiquad,bandpass)(&$starSymbol(parametric),
				    $starSymbol(filtercoeff));
	    }
	    $sharedSymbol(CGCParamBiquad,setfiltertaps)
	      (&$starSymbol(parametric), $starSymbol(filtercoeff),
	       $starSymbol(filtertaps));  

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
	    /* conveying the value changes of bandwidth to audio */
	    /* only for bandpass filters, else inactive          */
	    $ref(bandwidth) = position*0.0399 + 0.01;
	    	    
	    if (strcasecmp("$val(filtertype)", "BAND") == 0){
	    $starSymbol(setparams)(&$starSymbol(parametric));
	    $sharedSymbol(CGCParamBiquad,bandpass)(&$starSymbol(parametric),
				    $starSymbol(filtercoeff));
	    $sharedSymbol(CGCParamBiquad,setfiltertaps)
	      (&$starSymbol(parametric), $starSymbol(filtercoeff),
	       $starSymbol(filtertaps));  	    
	    }

	    sprintf(buf, "%.4f", $ref(bandwidth));
	    displaySliderValue(".high", "$starSymbol(scale1)", buf);
            return TCL_OK;
        }
    }
    codeblock (setcenterFreqDef) {
        static int $starSymbol(setcenterFreq)(dummy, interp, argc, argv)
            ClientData dummy;                   /* Not used. */
            Tcl_Interp *interp;                 /* Current interpreter. */
            int argc;                           /* Number of arguments. */
            char **argv;                        /* Argument strings. */
        {
	    static char buf[20];
	    int tkfreq;
            if(sscanf(argv[1], "%d", &tkfreq) != 1) {
                errorReport("Invalid center freq");
                return TCL_ERROR;
            }
	    /* set the new center frequency  */
	    $ref(centerFreq) = 
	      tkfreq*($val(highcenterFreq) - $val(lowcenterFreq))/100 
	      + $val(lowcenterFreq);
	    
	    $starSymbol(setparams)(&$starSymbol(parametric));
	    if (strcasecmp("$val(filtertype)", "LOW") == 0){
	      $sharedSymbol(CGCParamBiquad,lowpass)(&$starSymbol(parametric),
				   $starSymbol(filtercoeff));
	    }
	    else if (strcasecmp("$val(filtertype)", "HI") == 0){
	      $sharedSymbol(CGCParamBiquad,hipass)(&$starSymbol(parametric),
				  $starSymbol(filtercoeff));
	    }
	    else if (strcasecmp("$val(filtertype)", "BAND") == 0){
	      $sharedSymbol(CGCParamBiquad,bandpass)(&$starSymbol(parametric),
				    $starSymbol(filtercoeff));
	    }
	    $sharedSymbol(CGCParamBiquad,setfiltertaps)
	      (&$starSymbol(parametric), $starSymbol(filtercoeff),
	       $starSymbol(filtertaps));  

	    sprintf(buf, "%.2f", $ref(centerFreq));
	    displaySliderValue(".middle", "$starSymbol(scale3)", buf);
            return TCL_OK;
        }
    }
    codeblock (setpassFreqDef) {
        static int $starSymbol(setpassFreq)(dummy, interp, argc, argv)
            ClientData dummy;                   /* Not used. */
            Tcl_Interp *interp;                 /* Current interpreter. */
            int argc;                           /* Number of arguments. */
            char **argv;                        /* Argument strings. */
        {
	    static char buf[20];
	    int tkpass;
            if(sscanf(argv[1], "%d", &tkpass) != 1) {
                errorReport("Invalid pass freq");
                return TCL_ERROR;
            }
	    /* set the new pass frequency  */
	    $ref(passFreq) = 
	      tkpass*($val(highcenterFreq) - $val(lowcenterFreq))/100 
	      + $val(lowcenterFreq);
	    
	    $starSymbol(setparams)(&$starSymbol(parametric));
	    if (strcasecmp("$val(filtertype)", "LOW") == 0){
	      $sharedSymbol(CGCParamBiquad,lowpass)(&$starSymbol(parametric),
				   $starSymbol(filtercoeff));
	    }
	    else if (strcasecmp("$val(filtertype)", "HI") == 0){
	      $sharedSymbol(CGCParamBiquad,hipass)(&$starSymbol(parametric),
				  $starSymbol(filtercoeff));
	    }
	    $sharedSymbol(CGCParamBiquad,setfiltertaps)
	      (&$starSymbol(parametric), $starSymbol(filtercoeff),
	       $starSymbol(filtertaps));  

	    sprintf(buf, "%.2f", $ref(passFreq));
	    displaySliderValue(".middle", "$starSymbol(scale4)", buf);
            return TCL_OK;
        }
    }
    
    initCode {
	CGCParamBiquad :: initCode();
	addCode(tkSetup, "tkSetup");
        addCode(setGainDef, "procedure");
        addCode(setBandwidthDef, "procedure");
	addCode(setcenterFreqDef, "procedure");
	addCode(setpassFreqDef, "procedure");

    }
}
