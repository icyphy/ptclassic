defstar {
	name { QuadFir }
	domain { SDF }
	version { $Id$ }
	author { William Chen }
	copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { SDF vis library }
	desc { 
	  A finite impulse response (FIR) filter.
	}
	input {
	  name { signalIn }
	  type { float }
	  desc { Input float type }
	}	
	output {	
	  name { signalOut }
	  type { float }
	  desc { Output float type }
	}
        ccinclude {<vis_proto.h>}
	defstate {
	  name {taps}
	  type {floatarray}
	  default {
	"-.040609 -.001628 .17853 .37665 .37665 .17853 -.001628 -.040609"
      }
	  desc { Filter tap values. }
	}		
      	defstate {
	  name { scale }
	  type { float }
	  default { "10000.0" }
	  desc { Filter tap scale }
	  attributes { A_CONSTANT|A_SETTABLE }
	}
	code {
#define NumIn (1)
#define	NumPack (4)
#define UpperBound (32767) 
#define LowerBound (-32768)
	}
	protected {
	  int taplength,tappadlength,maxpast;
	  short *shift_taparray;
	}
	constructor {
	  shift_taparray = 0;
	}
	destructor {
	  if (shift_taparray)
	    free(shift_taparray);
	}
        setup {
	  int taprowindex, tapcolindex;
	  double intmp;
	  
	  // determine tap length
	    taplength = taps.size();
	  
	  // determine input buffer size
	    if (taplength == 0)
	      maxpast = 1;
	    else if ((taplength-1)%NumPack==0)
	      maxpast = int((taplength-1)/NumPack)+1;
	    else
	      maxpast = int((taplength-1)/NumPack)+2;
	  tappadlength = NumPack*maxpast;
	  signalIn.setSDFParams(NumIn,maxpast);
	  
	  // allocate shifted taparrays
	    if (shift_taparray)
	      free(shift_taparray);
	    shift_taparray = (short*) malloc(NumPack*tappadlength);
	  
	  // initialize shifted taparrays to zero
	    for(taprowindex=0;taprowindex<NumPack;taprowindex++){
	      for(tapcolindex=0;tapcolindex<tappadlength;tapcolindex++){
		shift_taparray[taprowindex*tappadlength+tapcolindex] = 0;
	      }
	    }
	  
	  // fill taparrays
	    for(taprowindex=0;taprowindex<NumPack;taprowindex++){
	      for(tapcolindex=0;tapcolindex<taplength;tapcolindex++){
		// scale taps, check for under/overflow,
		// and cast to short
		  intmp = scale*taps[tapcolindex];
		if (intmp <= double(LowerBound)){
		  shift_taparray[taprowindex*(tappadlength+1)+tapcolindex]=\
		    double(LowerBound);
		}
		else if (intmp >= double(UpperBound)){
		  shift_taparray[taprowindex*(tappadlength+1)+tapcolindex]=\
		    double(UpperBound);
		}
		else{ 
		  shift_taparray[taprowindex*(tappadlength+1)+tapcolindex]=\
		    short(intmp);
		}
	      }	
	    }
	}
	go {	
	  
          union vis_dreg {
            double dreg64;
            short  sreg16[NumPack];
          };		
	  
	  union vis_dreg packedOut;
	  int outerloop, innerloop, numloop;
	  int genindex, tapindex;
	  float datahi, datalo;
	  unsigned long taphi[NumPack], taplo[NumPack];
	  float tappairhi[NumPack], tappairlo[NumPack];
	  float t0,t1;
	  float accumfinal[NumPack];
	  double pairlohi[NumPack], pairlolo[NumPack];
	  double pairhilo[NumPack], pairhihi[NumPack];
	  double accum_pairlo[NumPack], accum_pairhi[NumPack];
	  double accum_pair[NumPack];
	  
	  // loop once for each set of filter taps
	    if (taplength == 0)
	      numloop = 0;
	    else	
	      numloop = maxpast;
	  
	  // initialize accumulator to zero
	    for(genindex=0;genindex<NumPack;genindex++){
	      accum_pairlo[genindex] = vis_fzero();
	      accum_pairhi[genindex] = vis_fzero();
	      accum_pair[genindex] = vis_fzero();
	    }
	  // filter data
	    for(outerloop=0;outerloop<numloop;outerloop++){
	      // set up data
		datahi = vis_read_hi(double(signalIn%(outerloop)));
	      datalo = vis_read_lo(double(signalIn%(outerloop)));

	      // calculate four outputs
		for(innerloop=0;innerloop<NumPack;innerloop++){
		  // set up tap pairs for each shifted taparray
		  tapindex = innerloop*tappadlength+NumPack*outerloop;
		  taphi[innerloop] =
		    (shift_taparray[tapindex]<<16)|\
		      (shift_taparray[tapindex+1] & 0xffff);
		  tappairhi[innerloop] = vis_to_float(taphi[innerloop]);
		  taplo[innerloop] = (shift_taparray[tapindex+2]<<16)|\
		    (shift_taparray[tapindex+3] & 0xffff);
		  tappairlo[innerloop] = vis_to_float(taplo[innerloop]);

		  // take inner products
		    pairlolo[innerloop] =\
		      vis_fmuld8sux16(datalo,tappairlo[innerloop]);
		  pairlohi[innerloop] =\
		    vis_fmuld8ulx16(datalo,tappairlo[innerloop]);	    

		  pairhilo[innerloop] =\
		    vis_fmuld8sux16(datahi,tappairhi[innerloop]);
		  pairhihi[innerloop] =\
		    vis_fmuld8ulx16(datahi,tappairhi[innerloop]);

		  // accumulate results
		    accum_pairlo[innerloop] +=\
		      vis_fpadd32(pairlolo[innerloop],pairlohi[innerloop]);
		  accum_pairhi[innerloop] +=\
		    vis_fpadd32(pairhilo[innerloop],pairhihi[innerloop]);
		  accum_pair[innerloop] +=\
		    vis_fpadd32(accum_pairhi[innerloop],accum_pairlo[innerloop]);
		}  
	    }
	  // sum accumulators and pack outputs into a double
	    for(genindex=0;genindex<NumPack;genindex++){
	      t0 = vis_read_hi(accum_pair[genindex]);
	      t1 = vis_read_lo(accum_pair[genindex]);
	      accumfinal[genindex] = vis_fpadd32s(t0,t1); 
	    }

	  for(genindex=0;genindex<NumPack;genindex++)
	    packedOut.sreg16[genindex] = short(accumfinal[genindex]);
	  
	  signalOut%0 << packedOut.dreg64;
	}
      }
