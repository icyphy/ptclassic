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
	        int taplength,maxpast;
	        short *shift0_taparray;
		short *shift1_taparray;
	        short *shift2_taparray;
		short *shift3_taparray;	
	}
        setup {
	        int tapindex;
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
		signalIn.setSDFParams(NumIn,maxpast);

		// allocate shifted taparrays
		shift0_taparray = new short[NumPack*maxpast];
		shift1_taparray = new short[NumPack*maxpast];
		shift2_taparray = new short[NumPack*maxpast];
		shift3_taparray = new short[NumPack*maxpast];

		// initialize shifted taparrays to zero
		for(tapindex=0;tapindex<NumPack*maxpast;tapindex++){
		  shift0_taparray[tapindex] = 0;
		  shift1_taparray[tapindex] = 0;
	   	  shift2_taparray[tapindex] = 0;
		  shift3_taparray[tapindex] = 0;
		}

		// fill taparrays
       		for(tapindex=0;tapindex<taplength;tapindex++){
		  // scale taps, check for under/overflow,
		  // and cast to short
		  intmp = scale*floatarray[tapindex];
		  if (intmp <= double(LowerBound)){
		    shift0_taparray[tapindex] = double(LowerBound);
		    shift1_taparray[tapindex+1]= double(LowerBound);
		    shift2_taparray[tapindex+2]= double(LowerBound);
		    shift3_taparray[tapindex+3]= double(LowerBound);
		  }
		  else if (intmp >= double(UpperBound)){
		    shift0_taparray[tapindex] = double(UpperBound);
		    shift1_taparray[tapindex+1]= double(UpperBound);
		    shift2_taparray[tapindex+2]= double(UpperBound);
		    shift3_taparray[tapindex+3]= double(UpperBound);
		  }
		  else{ 
		    shift0_taparray[tapindex]  = short(intmp);
		    shift1_taparray[tapindex+1]= short(intmp);
		    shift2_taparray[tapindex+2]= short(intmp);
		    shift3_taparray[tapindex+3]= short(intmp);
		  }
		}	
	      }
	go {
	  int loopcount;
	  int numloop;
	  unsigned long 
	  double accum_lo, accum_hi;

	  // loop once for each set of filter taps
	  if (taplength == 0)
	    numloop = 0;
	  else
	    numloop = maxpast;

	  // initialize accumulator to zero
	  accum_lo = vis_fzero();
	  accum_hi = vis_fzero();

	  // filter data
          for(loopcount=0;loopcount<numloop;loopcount++){
	    // set up data
	    datahi = vis_read_hi(signalIn%(loopcount));
	    datalo = vis_read_lo(signalIn%(loopcount));

	    // set up tap pairs for each shifted taparray
	    // possible loop construct here
	    taphi = (shift0_taparray[NumPack*loopcount]<<16) |
	      (shift0_taparray[NumPack*loopcount+1] & 0xffff);
	    tappairhi = vis_to_float(taphi);
	    taplo = (shift0_taparray[NumPack*loopcount+2]<<16) |
	      (shift0_taparray[NumPack*loopcount+3] & 0xffff);
	    tappairlo = vis_to_float(taplo);

	    // take inner products
	    pairlolo = vis_fmuld8sux16(datalo,tappairlo);
	    pairlohi = vis_fmuld8ulx16(datalo,tappairlo);	    

	    pairhilo = vis_fmuld8sux16(datahi,tappairhi);
	    pairhihi = vis_fmuld8ulx16(datahi,tappairhi);

	    // accumulate results
	    accum_lo += vis_fpadd32(pairlolo,pairlohi);
	    accum_hi += vis_fpadd32(pairhilo,pairhihi);

	  }  
	    
	  // sum accumulators and pack outputs into a double
	  /*output packed double*/	  
	  Out%0 << packedout.dreg64;
	}
		
