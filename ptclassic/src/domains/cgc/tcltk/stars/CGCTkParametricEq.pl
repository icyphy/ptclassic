defstar
{
    name { TkParametricEq }
    domain { CGC }
    derivedFrom { ParametricEq }
    desc {
Just like ParametricEq, except that a Tk slider is put in the master
control panel to control the gain.
    }
    version { $Id$ }
    author { William Chen and Sunil Bhave }
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
      default { "parametric" }
      desc { The user-friendly name of this star.}
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
    
    codeblock (tkSetup) {
      /* Establish the Tk window for setting the values    */
      /* "tkgain" is  a local variable which scales the    */
      /* gain from (-10 to 10) to (0 to 100) for makeScale */ 
      {
	int tkgain;
	tkgain = (int) 5*$val(gain)+50;
	makeScale(".low",
		  "$starSymbol(scale1)",
		  "Gain control: $val(starName)",
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
		  "Center/Cutoff Freq crtl:$val(starName) ",
		  tkcenter,
                  $starSymbol(setcenterFreq));
	displaySliderValue(".middle", "$starSymbol(scale2)",
			   "$val(centerFreq)");
      }
 
    }
    codeblock(tkSetupband) {
      /* "tkband" is  a local variable which scales the    */
      /* bandwidth from (0.1 to 4) to (0 to 100) for makeScale */ 
      {
	int tkband;
	tkband = (int) ($val(bandwidth)-0.01)*100/3.99;
	makeScale(".high",
		  "$starSymbol(scale3)",
		  "Bandwidth control:$val(starName)",
		  tkband,
		  $starSymbol(setBandwidth));
	displaySliderValue(".high", "$starSymbol(scale3)",
			     "$val(bandwidth)");
      }      
    }
    codeblock(tkSetuppass) {
      /* "tkpass" is a local variable which scale the pass  */
      /* freq from (lowFreq to highFreq) to 0-100 */
      {	
	int tkpass;
	tkpass = (int)
	  100*($val(passFreq)-$val(lowFreq))/
	  ($val(highFreq) - $val(lowFreq));
	makeScale(".middle",
		  "$starSymbol(scale4)",
		  "Pass Frequency control:$val(starName)",
		  tkpass,
		  $starSymbol(setpassFreq));
	displaySliderValue(".middle", "$starSymbol(scale4)",
			   "$val(passFreq)");
      }
    }


    codeblock (tychoSetup) {
	connectControl(moduleName, "$val(starName)", "gain",
		$starSymbol(setGain));
	connectControl(moduleName, "$val(starName)", "freq",
		$starSymbol(setcenterFreq2));
    }
    codeblock(tychoSetupband) {
	connectControl(moduleName, "$val(starName)", "bw",
		$starSymbol(setBandwidth));
    }
    codeblock(tychoSetuppass) {
	connectControl(moduleName, "$val(starName)", "bw",
		$starSymbol(setpassFreq));
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
	    $sharedSymbol(CGCParametricEq,setparams)
	      (&$starSymbol(parametric),$ref(sampleFreq),
	       $ref(passFreq), $ref(centerFreq), $ref(bandwidth), $ref(gain));
	    $sharedSymbol(CGCParametricEq,selectFilter)
	      (&$starSymbol(parametric), $starSymbol(filtercoeff),
	       $starSymbol(filtertaps), $ref(filtertype));

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
	    $sharedSymbol(CGCParametricEq,setparams)
	      (&$starSymbol(parametric),$ref(sampleFreq),
	       $ref(passFreq), $ref(centerFreq), $ref(bandwidth), $ref(gain));
	    $sharedSymbol(CGCParametricEq,selectFilter)
	      (&$starSymbol(parametric), $starSymbol(filtercoeff),
	       $starSymbol(filtertaps), $ref(filtertype));

	    sprintf(buf, "%.2f", $ref(centerFreq));
	    displaySliderValue(".middle", "$starSymbol(scale2)", buf);
            return TCL_OK;
        }
    }
    
    codeblock (setcenterFreqDef2) {
        static int $starSymbol(setcenterFreq2)(dummy, interp, argc, argv)
            ClientData dummy;                   /* Not used. */
            Tcl_Interp *interp;                 /* Current interpreter. */
            int argc;                           /* Number of arguments. */
            char **argv;                        /* Argument strings. */
        {
	    static char buf[20];
            if(sscanf(argv[1], "%lf", &$ref(centerFreq)) != 1) {
                errorReport("Invalid center freq");
                return TCL_ERROR;
            }
	    /* procedure calls to update; defined in base class */	    
	    $sharedSymbol(CGCParametricEq,setparams)
	      (&$starSymbol(parametric),$ref(sampleFreq),
	       $ref(passFreq), $ref(centerFreq), $ref(bandwidth), $ref(gain));
	    $sharedSymbol(CGCParametricEq,selectFilter)
	      (&$starSymbol(parametric), $starSymbol(filtercoeff),
	       $starSymbol(filtertaps), $ref(filtertype));

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
	    $sharedSymbol(CGCParametricEq,setparams)
	      (&$starSymbol(parametric),$ref(sampleFreq), 
	       $ref(passFreq), $ref(centerFreq), $ref(bandwidth), $ref(gain));
	    $sharedSymbol(CGCParametricEq,selectFilter)
	      (&$starSymbol(parametric), $starSymbol(filtercoeff),
	       $starSymbol(filtertaps), $ref(filtertype));

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
	    $sharedSymbol(CGCParametricEq,setparams)
	      (&$starSymbol(parametric),$ref(sampleFreq), 
	       $ref(passFreq), $ref(centerFreq), $ref(bandwidth), $ref(gain));
	    $sharedSymbol(CGCParametricEq,selectFilter)
	      (&$starSymbol(parametric), $starSymbol(filtercoeff),
	       $starSymbol(filtertaps), $ref(filtertype));

	    sprintf(buf, "%.2f", $ref(passFreq));
	    displaySliderValue(".middle", "$starSymbol(scale4)", buf);
            return TCL_OK;
        }
    }
    
    initCode {
	CGCParametricEq :: initCode();
	addCode(tkSetup,    "tkSetup");
	addCode(tychoSetup, "tychoSetup");
	if (strcasecmp(filtertype, "BAND") == 0) {
	  addCode(tkSetupband,    "tkSetup");
	  addCode(tychoSetupband, "tychoSetup");
	  addCode(setBandwidthDef, "procedure");
	}
	else { 
	  addCode(tkSetuppass,    "tkSetup");
	  addCode(tychoSetuppass, "tychoSetup");
	  addCode(setpassFreqDef, "procedure");
	}
        addCode(setGainDef, "procedure");
	addCode(setcenterFreqDef,  "procedure");
	addCode(setcenterFreqDef2, "procedure");
    }
}
