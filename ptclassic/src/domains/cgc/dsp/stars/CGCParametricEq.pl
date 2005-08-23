defstar {
	name {ParamBiquad}
	derivedFrom {Biquad}
	domain {CGC}
	version {@(#)CGCParamBiquad.pl	1.5 6/29/96 }
	desc {
A two-pole, two-zero parametric digital IIR filter (a biquad).
	}
	author { William Chen}
	copyright {
Copyright (c) 1990-1995 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { CGC dsp library }
	explanation {
The user supplies the parameters such as Bandwidth, Center Frequency,
and Gain.  The digital biquad coefficients are quickly calculated
based on the procedure defined by Shpak.
	}
	defstate {
		name {filtertype}
		type { int }
		default { "1" }
		desc { choices are Lowpass=0,Bandpass=1,and Hipass=2 }
	}
	defstate {
		name {sampleFreq}
		type { float }
		default { "44100" }
		desc { given in Hz }
	}
	defstate {
		name {centerFreq}
		type { float }
		default { "1000" }
		desc { given in Hz. Center frequency for bandpass and cutoff
			 frequency for low/highpass filters}
	}
	defstate {
		name {passFreq}
		type { float }
		default { "1000" }
		desc { given in Hz. Passband frequency for
		  low/highpass filters.  Not needed for bandpass
		    types. }
	}
	defstate {
		name {gain}
		type { float }
		default { "0" }
		desc { given in dB and ranges from [-10,10]. }
	}
	defstate {
		name {bandwidth}
		type { float }
		default { "1" }
		desc { given in Octave and ranges from [0,4]. 
			 Not needed for Lowpass and Hipass types. }
	}
	codeblock(setparams) {
	  static void setparams(parametric_t *parametric){
	    double gaintmp,t0,t1;

	    parametric->T = 1/$val(sampleFreq);
	    parametric->omegap = 2*PI*$val(passFreq)*parametric->T;
	    parametric->omegac = 2*PI*$val(centerFreq)*parametric->T;
	    t0 = log(2)/2;
	    t1 = exp($val(bandwidth)*t0);
	    parametric->omegabw = parametric->omegac*(t1-1/t1);

	    if ($val(gain)>=0){
	      parametric->gainflag = 1;
	      gaintmp=$val(gain)/20.0;
	    }
	    else{
	      parametric->gainflag = 0;
	      gaintmp=$val(gain)/-20.0;
	    }
	    parametric->lineargain = pow(10.0,gaintmp);
	  }
	}
        codeblock(constbw) {
          static double constbw(int niter,double tol,double
				bw,double wc,double initial_guess)
            {
	      double x0,x1;
	      double fval,fprimeval;
	      int i;

	      x0 = initial_guess;
	      fval = -bw/2+atan(x0*wc/2)-atan(wc/(2*x0));
	      fprimeval = (wc/2)/(1+(x0*wc/2)*(x0*wc/2)) +
		(wc/2)/(x0*x0 + wc*wc/4);
	      x1 = x0 -	fval/fprimeval;
 
	      i = 0;
	      while(fabs(x1-x0) > tol & i < niter){
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
          static void lowpass(parametric_t *parametric,double
			      *filtercoeff)
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
          static void bandpass(parametric_t *parametric,double *filtercoeff)
	    {
	      double omegacwarp,omegacornerwarp,omegacorner_guess,gamma;
	      double n0,d0,d1,d2,Qzsquared,Qpsquared,Qz,Qp,initial;
	      double a2,b2;

	      omegacwarp = 2*tan(parametric->omegac/2);
	      omegacorner_guess = parametric->omegac - (parametric->omegabw/2);
	      initial = parametric->omegac/omegacorner_guess;
	      gamma = constbw(10,0.001,parametric->omegabw,omegacwarp,initial);
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
          static void hipass(parametric_t *parametric,double *filtercoeff)
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
	  static void setfiltertaps(parametric_t *parametric,double
				    *filtercoeff,double *filtertaps){
	    if(parametric->gainflag == 1){
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
	codeblock(globalDecl){
	  typedef struct parametric_band
	  {
	    double omegac;
	    double omegap;
	    double omegabw;
	    double T;
	    double lineargain;
	    int gainflag;
	  } parametric_t;

#define LOW  (0)
#define BAND (1)
#define HI   (2)
#define PI (M_PI)
	}
	codeblock(mainDecl){
	  parametric_t parametric;
	  double $starSymbol(filtercoeff)[5];
	}
	codeblock(findparams){
	  setparams(&parametric);
	  if($val(filtertype) == LOW){
	    lowpass(&parametric,$starSymbol(filtercoeff));
	  }
	  else if($val(filtertype) == HI){
	    hipass(&parametric,$starSymbol(filtercoeff));
	  }
	  else if($val(filtertype) == BAND){
	    bandpass(&parametric,$starSymbol(filtercoeff));
          }
	  /*else
	    error;*/
	  setfiltertaps(&parametric,$starSymbol(filtercoeff),$starSymbol(filtertaps));
	}
	initCode {
	  CGCBiquad::initCode();
	  addGlobal(globalDecl,"global");
	  addProcedure(setparams,"param");
          addProcedure(constbw,"newtonapprox");
	  addProcedure(lowpass,"lp");
	  addProcedure(hipass,"hp");
	  addProcedure(bandpass,"bp");
	  addProcedure(setfiltertaps,"taps");
          addDeclaration(mainDecl);
          addCode(findparams);
        }
	go {
	  CGCBiquad::go();
	}
}
