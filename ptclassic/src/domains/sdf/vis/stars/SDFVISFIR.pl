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
	  default { "32767.0" }
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
	  float *result;
	  double *accumpair;
	}
	constructor {
	  shift_taparray = 0;
	  result = 0;
	  accumpair = 0;
	}
	destructor {
	  free(shift_taparray);
	  free(result);
	  free(accumpair);
	}
        setup {

	  // determine tap length
	       taplength = taps.size();
	  
	  // determine input buffer size
	       if (taplength == 0)
		 maxpast = 1;
	       else if ((taplength-1)%NumPack==0)
		 maxpast = (int)(taplength-1)/NumPack+1;
	       else
		 maxpast = (int)(taplength-1)/NumPack+2;
	  tappadlength = NumPack*maxpast;
	  signalIn.setSDFParams(NumIn,maxpast-1);
	}
	begin {

	  int taprowindex, tapcolindex;
	  short *indexcount;
	  double intmp;

  	  // allocate shifted tap and tmp arrays
	       free(shift_taparray);
	  free(result);
	  free(accumpair);

	  accumpair = (double *)
	    memalign(sizeof(double),sizeof(double)*NumPack);
	  result = (float *)
	    memalign(sizeof(double),sizeof(float)*NumPack);
	  shift_taparray = (short *)
	    memalign(sizeof(double),sizeof(short)*NumPack*tappadlength);

	  // initialize shifted taparrays to zero
	       indexcount = shift_taparray;
	  for(taprowindex=0;taprowindex<NumPack;taprowindex++){
	    for(tapcolindex=0;tapcolindex<tappadlength;tapcolindex++){
	      *indexcount++ = 0;
	    }
	  }
	  
	  // fill taparrays
	       for(taprowindex=0;taprowindex<NumPack;taprowindex++){
		 indexcount = shift_taparray + (tappadlength+1)*taprowindex;
		 for(tapcolindex=0;tapcolindex<taplength;tapcolindex++){
		   // scale taps, check under/overflow, and cast to short 
			intmp = scale*taps[tapcolindex];
		   if (intmp <= (double)(LowerBound)){
		     *indexcount++ = (short)(LowerBound);
		   }
		   else if (intmp >= (double)(UpperBound)){
		     *indexcount++ = (short)(UpperBound);
		   }
		   else{ 
		     *indexcount++ = (short)(intmp);
		   }
		 }
	       }
	}
	go {	

	  int outerloop, innerloop, numloop;
	  int genindex;
	  float datahi, datalo;
          float tappairhi, tappairlo;
          float splithi, splitlo;
          float packedhi, packedlo;
          double tapvalue;
	  double *tapptr[NumPack];
	  double pairlohi, pairlolo;
	  double pairhilo, pairhihi;
	  double pairlo, pairhi, pair;
	  double packedOut;
	  double *packedaccum;

	  vis_write_gsr(8);	  
	  
	  // loop once for each set of filter taps
	       if (taplength == 0)
		 numloop = 0;	
	       else		
		 numloop = maxpast;	
	  
	  // initialize accumulator to zero
	       for(genindex=0;genindex<NumPack;genindex++){
		 accumpair[genindex] = vis_fzero();
	       }

	  // initialize the tapptr to point at the beginning of
	       // each tap array
	       for(genindex=0;genindex<NumPack;genindex++){
		 tapptr[genindex] = (double *) (shift_taparray +
		   genindex*tappadlength);
	       }

	  // filter data
	       for(outerloop=0;outerloop<numloop;outerloop++){
		 // set up data
		      datahi = vis_read_hi(double(signalIn%(outerloop)));
		 datalo = vis_read_lo(double(signalIn%(outerloop)));

		 // calculate four outputs
		      for(innerloop=0;innerloop<NumPack;innerloop++){
			// set up tap pairs for each shifted taparray
			     tapvalue = *tapptr[innerloop]++;
			tappairhi = vis_read_hi(tapvalue);
			tappairlo = vis_read_lo(tapvalue);

			// take inner products
			     pairlolo = vis_fmuld8sux16(datalo,tappairlo);
			pairlohi = vis_fmuld8ulx16(datalo,tappairlo);	    

			pairhilo = vis_fmuld8sux16(datahi,tappairhi);
			pairhihi = vis_fmuld8ulx16(datahi,tappairhi);

			// accumulate results
			     pairlo = vis_fpadd32(pairlolo,pairlohi);
			pairhi = vis_fpadd32(pairhilo,pairhihi);
			pair = vis_fpadd32(pairhi,pairlo);
			accumpair[innerloop] = vis_fpadd32(accumpair[innerloop],pair);
		      }  
	       }

	  // sum accumulators and pack outputs into a double
	       for(genindex=0;genindex<NumPack;genindex++){
		 splithi = vis_read_hi(accumpair[genindex]);
		 splitlo = vis_read_lo(accumpair[genindex]);
		 result[genindex] = vis_fpadd32s(splithi,splitlo); 
	       }
	  
	  packedaccum = (double *) result;
	  packedhi = vis_fpackfix(*packedaccum);
	  packedaccum++;
	  packedlo = vis_fpackfix(*packedaccum);
	  packedOut = vis_freg_pair(packedhi,packedlo);
	  
	  signalOut%0 << packedOut;
	}
}	






