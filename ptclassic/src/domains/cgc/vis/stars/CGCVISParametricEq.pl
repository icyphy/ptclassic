defstar {
	name {VISParametricEq}
	derivedFrom {VISBiquad}
	domain {CGC}
	version { $Id$ }
	desc {
A two-pole, two-zero parametric digital IIR filter (a biquad).
	}
	author { William Chen and John Reekie}
	copyright {
Copyright (c) 1996- The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { CGC dsp library }
	htmldoc {
The user supplies the parameters such as Bandwidth, Center Frequency,
and Gain.  The digital biquad coefficients are quickly calculated
based on the procedure defined by Shpak.
	}
	defstate {
		name {filtertype}
		type { string }
		default { "BAND" }
		desc { choices are LOWpass=0,BANDpass=1,and HIpass=2 }
		attributes { A_GLOBAL }
	}
	defstate {
		name {sampleFreq}
		type { float }
		default { "44100" }
		desc { given in Hz }
		attributes{ A_GLOBAL }
	}

	defstate {
		name {centerFreq}
		type { float }
		default { "1000" }
		desc {
Center frequency in Hz for bandpass and cutoff frequency for
low/highpass filters
		}
		attributes{ A_GLOBAL }
	}

	defstate {
		name {passFreq}
		type { float }
		default { "1000" }
		desc {
Passband frequency in Hz for low/highpass filters.
Not needed for bandpass types.
		}
		attributes{ A_GLOBAL }
	}
	defstate {
		name {gain}
		type { float }
		default { "0" }
		desc { given in dB and ranges from [-10,10]. }
		attributes { A_GLOBAL }
	}

	defstate {
		name {bandwidth}
		type { float }
		default { "1" }
		desc {
Given in Octave and ranges from [0,4]. Not needed for Lowpass 
and Hipass types.
		}
		attributes{ A_GLOBAL }
	}

	codeblock(setparams) {
	  static void $sharedSymbol(CGCVISParametricEq,setparams)
	    (parametric_t *parametric, double sampleFreq, double
	     passFreq, double centerFreq, double bandwidth, double gain) {
	    double gaintmp, t0, invf1prime;

	    parametric->T = 1/sampleFreq;
	    parametric->omegap = 2*PI*passFreq*parametric->T;
	    parametric->omegac = 2*PI*centerFreq*parametric->T;
	    t0 = log(2)/2;
	    invf1prime = exp(bandwidth*t0);
	    parametric->omegabw = parametric->omegac*(invf1prime-1/invf1prime);

	    if (gain>=0){
	      parametric->gainflag = 1;
	      gaintmp=gain/20.0;
	    }
	    else{
	      parametric->gainflag = 0;
	      gaintmp=gain/-20.0;
	    }
	    parametric->lineargain = pow(10.0,gaintmp);
	  }
	}

        codeblock(constbw) {
	  /* Newton approximation */
          static double $sharedSymbol(CGCVISParametricEq,constbw)(int niter,
	     double tol, double bw, double wc, double initial_guess) {
	      double x0,x1;
	      double fval,fprimeval;
	      int i;

	      x0 = initial_guess;
	      fval = -bw/2+atan(x0*wc/2)-atan(wc/(2*x0));
	      fprimeval = (wc/2)/(1+(x0*wc/2)*(x0*wc/2)) +
		(wc/2)/(x0*x0 + wc*wc/4);
	      x1 = x0 -	fval/fprimeval;
 
	      i = 0;
	      while((fabs(x1-x0) > tol) & (i < niter)){
		x0 = x1;
		fval = -bw/2+atan(x0*wc/2)-atan(wc/(2*x0));
		fprimeval = (wc/2)/(1+(x0*wc/2)*(x0*wc/2)) +
		  (wc/2)/(x0*x0 + wc*wc/4);
		x1 = x0 - fval/fprimeval;
		i++;
	      }
	      return x1;
            }        
	}
	codeblock(lowpass) {
          static void $sharedSymbol(CGCVISParametricEq,lowpass)(parametric_t *parametric, double *filtercoeff)
	    {
	      double omegapwarp,omegacwarp;
	      double n0,d0,d1,d2,Qzsquared,Qpsquared,Qz,Qp;
	      double a2,b2;

	      omegapwarp = 2*tan(parametric->omegap/2);
	      omegacwarp = 2*tan(parametric->omegac/2);
	      n0  = ((omegapwarp*omegacwarp)*(omegapwarp*omegacwarp));
	      d0  = (parametric->lineargain)*pow(omegacwarp,4.0);
	      d1  = (parametric->lineargain-1) * ((omegacwarp*omegapwarp) *
				(omegapwarp*omegacwarp));
	      d2  = (parametric->lineargain*parametric->lineargain) *
		pow(omegapwarp,4.0);
	      Qzsquared = (n0)/(d0+d1-d2);
	      Qz = sqrt(Qzsquared);
	      Qpsquared =
		(parametric->lineargain*parametric->lineargain) /
		((parametric->lineargain-1)*(parametric->lineargain-1) +
		 1/(Qz*Qz));
	      Qp = sqrt(Qpsquared);
	      a2 = Qp*(Qz*(4+parametric->lineargain *
			   (omegapwarp*omegapwarp)) + 2*omegapwarp);
	      filtercoeff[0] =
		(2*Qp*Qz/a2)*(-4+parametric->lineargain *
			      (omegapwarp*omegapwarp));
	      filtercoeff[1] = (Qp/a2) *
		(Qz*(4+parametric->lineargain*(omegapwarp*omegapwarp))
		 - 2*omegapwarp);
	      b2 = Qz*(Qp*(4+(omegapwarp*omegapwarp))+2*omegapwarp);
	      filtercoeff[2] = (2*Qz*Qp/b2)*(-4+(omegapwarp*omegapwarp));
	      filtercoeff[3] = (Qz/b2) *
		(Qp*(4+(omegapwarp*omegapwarp))-2*omegapwarp);
	      filtercoeff[4] = a2/b2;
	    }
	}
	codeblock(bandpass) {
          static void $sharedSymbol(CGCVISParametricEq,bandpass)(parametric_t *parametric, double *filtercoeff) {
	      double omegacwarp,omegacornerwarp,omegacorner_guess,gamma;
	      double Qz,Qp,initial;
	      double a2,b2;

	      omegacwarp = 2*tan(parametric->omegac/2);
	      omegacorner_guess = parametric->omegac - (parametric->omegabw/2);
	      initial = parametric->omegac/omegacorner_guess;
	      gamma = $sharedSymbol(CGCVISParametricEq,constbw)(5,0.001, parametric->omegabw, omegacwarp, initial);
	      omegacornerwarp = omegacwarp/gamma;
	      Qp = (sqrt(parametric->lineargain)*omegacwarp*omegacornerwarp) /
		(omegacwarp*omegacwarp-omegacornerwarp*omegacornerwarp);
	      Qz = Qp/parametric->lineargain;

	      a2 = Qp*(Qz*(4+(omegacwarp*omegacwarp))+2*omegacwarp);
	      filtercoeff[0] = (2*Qp*Qz/a2)*(-4+(omegacwarp*omegacwarp));
	      filtercoeff[1] =
		(Qp/a2)*(Qz*(4+(omegacwarp*omegacwarp))-2*omegacwarp);

	      b2 = Qz*(Qp*(4+(omegacwarp*omegacwarp))+2*omegacwarp);
	      filtercoeff[2] = (2*Qz*Qp/b2)*(-4+(omegacwarp*omegacwarp));
	      filtercoeff[3] =
		(Qz/b2)*(Qp*(4+(omegacwarp*omegacwarp))-2*omegacwarp);
	      filtercoeff[4] = a2/b2;
	    }
	}
	codeblock(hipass) {
          static void $sharedSymbol(CGCVISParametricEq,hipass)(parametric_t *parametric, double *filtercoeff)
	    {
	      double omegapwarp,omegacwarp;
	      double n0,d0,d1,d2,Qzsquared,Qpsquared,Qz,Qp;
	      double a2,b2;

	      omegapwarp = 2*tan(parametric->omegap/2);
	      omegacwarp = 2*tan(parametric->omegac/2);
	      n0  = ((omegapwarp*omegacwarp)*(omegapwarp*omegacwarp));
	      d0  =
		(parametric->lineargain*parametric->lineargain) *
		pow(omegacwarp,4.0);
	      d1  = (parametric->lineargain-1) * ((omegacwarp*omegapwarp) *
				(omegapwarp*omegacwarp));
	      d2  = (parametric->lineargain)*pow(omegapwarp,4.0);
	      Qzsquared = (n0)/(-d0+d1+d2);
	      Qz = sqrt(Qzsquared);
	      Qpsquared =
		(parametric->lineargain*parametric->lineargain) /
		((parametric->lineargain-1) * (parametric->lineargain-1)
		 + 1/(Qz*Qz));
	      Qp = sqrt(Qpsquared);
	      a2 =
		Qp*(Qz * (4*parametric->lineargain +
			  (omegapwarp*omegapwarp)) + 2*omegapwarp);
	      filtercoeff[0] = (2*Qp*Qz/a2) *
		(-4*parametric->lineargain + (omegapwarp*omegapwarp));
	      filtercoeff[1] = (Qp/a2)
		*(Qz*(4*parametric->lineargain+(omegapwarp*omegapwarp))
		  - 2*omegapwarp);
	      b2 = Qz*(Qp*(4+(omegapwarp*omegapwarp))+2*omegapwarp);
	      filtercoeff[2] = (2*Qz*Qp/b2)*(-4+(omegapwarp*omegapwarp));
	      filtercoeff[3] = (Qz/b2) *
		(Qp*(4+(omegapwarp*omegapwarp))-2*omegapwarp);
	      filtercoeff[4] = a2/b2;
	    }
	}

	codeblock(setfiltertaps){
	  static void $sharedSymbol(CGCVISParametricEq,setfiltertaps)
	    (parametric_t *parametric, double *filtercoeff, double
	     *filtertaps)
	    {
	    if (parametric->gainflag == 1){
	      filtertaps[0]=filtercoeff[2];
	      filtertaps[1]=filtercoeff[3];	      
	      filtertaps[2]=filtercoeff[4];
	      filtertaps[3]=filtercoeff[4]*filtercoeff[0];
	      filtertaps[4]=filtercoeff[4]*filtercoeff[1];
	    }
	    else{
	      filtertaps[0]=filtercoeff[0];
	      filtertaps[1]=filtercoeff[1];
	      filtertaps[2]=1/filtercoeff[4];
	      filtertaps[3]=filtercoeff[2]/filtercoeff[4];
	      filtertaps[4]=filtercoeff[3]/filtercoeff[4];
	    }
	  }
	}
	
	codeblock(selectFilter) {
	  /* This procedure is for the Tk star. Its a common	*/
	  /* procedure called to update changes in gain, center */
	  /* freq., bandwidth, and passband freq, in the Tk star*/

	  static void $sharedSymbol(CGCVISParametricEq,selectFilter)
	    (parametric_t *parametric, double *filtercoeff, 
	     double *filtertaps, char *filtername)
	    {
	      if (strcasecmp(filtername, "LOW") == 0){
		$sharedSymbol(CGCVISParametricEq,lowpass)
		  (parametric, filtercoeff);
	      }
	      else if (strcasecmp(filtername, "HI") == 0){
		$sharedSymbol(CGCVISParametricEq,hipass)
		  (parametric, filtercoeff);
	      }
	      else if (strcasecmp(filtername, "BAND") == 0){
		$sharedSymbol(CGCVISParametricEq,bandpass)
		  (parametric, filtercoeff);
	      }
	      $sharedSymbol(CGCVISParametricEq,setfiltertaps)
		(parametric, filtercoeff, filtertaps);
	    }
	}
	
	codeblock(globalDecl){
	  typedef struct parametric_band {
	    double omegac;
	    double omegap;
	    double omegabw;
	    double T;
	    double lineargain;
	    int gainflag;
	  } parametric_t;

#define PI (M_PI)
	}
	codeblock(declarations){
	  parametric_t $starSymbol(parametric);
	  double $starSymbol(filtercoeff)[5]; 
	}
 
	initCode {
	  addInclude("<string.h>");
	  addGlobal(globalDecl, "global");
	  addGlobal(declarations);
	  CGCVISBiquad::initCode();
	  addProcedure(setparams, "CGCVISParametricEq_setparams");
          addProcedure(constbw, "CGCVISParametricEq_constbw");
	  addProcedure(lowpass, "CGCVISParametricEq_lowpass");
	  addProcedure(hipass, "CGCVISParametricEq_hipass");
	  addProcedure(bandpass, "CGCVISParametricEq_bandpass");
	  addProcedure(setfiltertaps, "CGCVISParametricEq_setfiltertaps");
	  addProcedure(selectFilter, "CGCVISParametricEq_selectFilter");
	  addCode("$sharedSymbol(CGCVISParametricEq,setparams)(&$starSymbol(parametric),$ref(sampleFreq), $ref(passFreq), $ref(centerFreq), $ref(bandwidth), $ref(gain));");
	  if (strcasecmp(filtertype, "LOW") == 0){
	    addCode("$sharedSymbol(CGCVISParametricEq,lowpass)(&$starSymbol(parametric),$starSymbol(filtercoeff));");
	  }
	  else if (strcasecmp(filtertype, "HI") == 0){
	    addCode("$sharedSymbol(CGCVISParametricEq,hipass)(&$starSymbol(parametric),$starSymbol(filtercoeff));");
	  }
	  else if (strcasecmp(filtertype, "BAND") == 0){
	    addCode("$sharedSymbol(CGCVISParametricEq,bandpass)(&$starSymbol(parametric),$starSymbol(filtercoeff));");
          }
	  addCode("$sharedSymbol(CGCVISParametricEq,setfiltertaps)(&$starSymbol(parametric),$starSymbol(filtercoeff),$starSymbol(filtertaps));");
	  addCode("$sharedSymbol(CGCVISBiquad,settaps)($starSymbol(filtertaps),$starSymbol(tapmatrix),&$starSymbol(betatop),&$starSymbol(betabott),$starSymbol(scaledown));");
        }
	go {
	  CGCVISBiquad::go();
	}
}
