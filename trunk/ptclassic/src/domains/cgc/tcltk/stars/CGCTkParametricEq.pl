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
      name { highFreq }
      type { float }
      default { 20000.0 }
      desc { The highest possible center frequency. }
    }
    
    defstate {
      name { lowFreq }
      type { float }
      default { 1.0 }
      desc { The lowest possible center frequency. }
    }
    
    defstate {
      name { nameofStar }
      type { string }
      default { "left1" }
      desc { A token to distinguish between multiple instances of same
	       star in a universe.}
    }

    codeblock (tkSetup) {
      /* Establish the Tk window for setting the values    */
      /* "tkgain" is  a local variable which scales the    */
      /* gain from (-10 to 10) to (0 to 100) for makeScale */ 
      {
	int tkgain;
	tkgain = (int) 5*$val(gain)+50;
	makeScale(".low",
		  "$starSymbol(scale1)",
		  "Gain control: $val(nameofStar)",
		  tkgain,
                  $starSymbol(setGain));
	displaySliderValue(".low", "$starSymbol(scale1)", " $val(gain)");
      }

      /* "tkcenter" is a local variable which scale the center  */
      /* freq from (lowFreq to highFreq) to 0-100 */
      {
	int tkcenter;
	tkcenter = (int)
	  100*($val(centerFreq)-$val(lowFreq))/
	  ($val(highFreq) - $val(lowFreq));
	makeScale(".middle",
		  "$starSymbol(scale2)",
		  "Center/Cutoff Freq crtl:$val(nameofStar) ",
		  tkcenter,
                  $starSymbol(setcenterFreq));
	displaySliderValue(".middle", "$starSymbol(scale2)",
			   "$val(centerFreq)");
      }

      if (strcasecmp("$val(filtertype)", "BAND") == 0) {
	/* "tkband" is  a local variable which scales the    */
	/* bandwidth from (0.1 to 4) to (0 to 100) for makeScale */ 
	{
	  int tkband;
	  tkband = (int) ($val(bandwidth)-0.01)*100/3.99;
	  makeScale(".high",
		    "$starSymbol(scale3)",
		    "Bandwidth control:$val(nameofStar)",
		    tkband,
		    $starSymbol(setBandwidth));
	  displaySliderValue(".high", "$starSymbol(scale3)",
			     "$val(bandwidth)");
	}      
      }
      else {
	/* "tkpass" is a local variable which scale the pass  */
	/* freq from (lowFreq to highFreq) to 0-100 */
	{	
	  int tkpass;
	  tkpass = (int)
	    100*($val(passFreq)-$val(lowFreq))/
	    ($val(highFreq) - $val(lowFreq));
	  makeScale(".middle",
		    "$starSymbol(scale4)",
		    "Pass Frequency control:$val(nameofStar)",
		    tkpass,
		    $starSymbol(setpassFreq));
	  displaySliderValue(".middle", "$starSymbol(scale4)",
			     "$val(passFreq)");
	}
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
	    /* procedure calls to update; defined in base class */
	    $starSymbol(setparams)(&$starSymbol(parametric));
	    $sharedSymbol(CGCParamBiquad,selectFilter)
	      (&$starSymbol(parametric), $starSymbol(filtercoeff),
	       $starSymbol(filtertaps), "$val(filtertype)");

	    sprintf(buf, "%.2f", $ref(gain));
	    displaySliderValue(".low", "$starSymbol(scale1)", buf);
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
	    int tkcenter;
            if(sscanf(argv[1], "%d", &tkcenter) != 1) {
                errorReport("Invalid center freq");
                return TCL_ERROR;
            }
	    /* set the new center frequency  */
	    $ref(centerFreq) = 
	      tkcenter*($val(highFreq) - $val(lowFreq))/100 
	      + $val(lowFreq);
	    /* procedure calls to update; defined in base class */	    
	    $starSymbol(setparams)(&$starSymbol(parametric));
	    $sharedSymbol(CGCParamBiquad,selectFilter)
	      (&$starSymbol(parametric), $starSymbol(filtercoeff),
	       $starSymbol(filtertaps), "$val(filtertype)");

	    sprintf(buf, "%.2f", $ref(centerFreq));
	    displaySliderValue(".middle", "$starSymbol(scale2)", buf);
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
	    int tkband;
            if(sscanf(argv[1], "%d", &tkband) != 1) {
                errorReport("Invalid bandwidth");
                return TCL_ERROR;
            }
	    /* conveying the value changes of bandwidth to audio */
	    /* only for bandpass filters, else inactive          */
	    $ref(bandwidth) = tkband*0.0399 + 0.01;
	    /* procedure calls to update; defined in base class */
	    $starSymbol(setparams)(&$starSymbol(parametric));
	    $sharedSymbol(CGCParamBiquad,selectFilter)
	      (&$starSymbol(parametric), $starSymbol(filtercoeff),
	       $starSymbol(filtertaps), "$val(filtertype)");

	    sprintf(buf, "%.4f", $ref(bandwidth));
	    displaySliderValue(".high", "$starSymbol(scale3)", buf);
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
	    /* set the new pass frequency, only if its a high   */
	    /* or low pass filter.				*/
	    $ref(passFreq) = 
	      tkpass*($val(highFreq) - $val(lowFreq))/100 
	      + $val(lowFreq);
	    /* procedure calls to update; defined in base class */	    
	    $starSymbol(setparams)(&$starSymbol(parametric));
	    $sharedSymbol(CGCParamBiquad,selectFilter)
	      (&$starSymbol(parametric), $starSymbol(filtercoeff),
	       $starSymbol(filtertaps), "$val(filtertype)");

	    sprintf(buf, "%.2f", $ref(passFreq));
	    displaySliderValue(".middle", "$starSymbol(scale4)", buf);
            return TCL_OK;
        }
    }
    
    initCode {
	CGCParamBiquad :: initCode();
	addCode(tkSetup, "tkSetup");
        addCode(setGainDef, "procedure");
	addCode(setcenterFreqDef, "procedure");
	addCode(setBandwidthDef, "procedure");
	addCode(setpassFreqDef, "procedure");
    }
}
