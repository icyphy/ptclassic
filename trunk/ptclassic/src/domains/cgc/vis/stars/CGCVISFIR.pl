defstar {
	name { QuadFir }
	domain { CGC }
	version { $Id$ }
	author { William Chen }
	copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { CGC vis library }
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
        defstate {
          name {scalefactor}
          type {int}
          default {"0"}
          desc { 2^scalefactor is used to scale down the magnitude
                   of the numerator coefficients between 0 and 1. }
          attributes{ A_CONSTANT|A_SETTABLE }
        }
        defstate {
                name {taplength}
                type {int}
                default {0}
                attributes { A_NONCONSTANT|A_NONSETTABLE }
	}
        defstate {
                name {tappadlength}
                type {int}
                default {0}
                attributes { A_NONCONSTANT|A_NONSETTABLE }
	}
        defstate {
                name {maxpast}
                type {int}
                default {0}
                attributes { A_NONCONSTANT|A_NONSETTABLE }
	}
	code {
#define NUMIN (1)
#define	NUMPACK (4)
	}
        setup {

	  // determine tap length
	       taplength = taps.size();
	  
	  // determine input buffer size
	       if (taplength == 0)
		 maxpast = 1;
	       else if ((taplength-1)%NUMPACK==0)
		 maxpast = (int)(taplength-1)/NUMPACK+1;
	       else
		 maxpast = (int)(taplength-1)/NUMPACK+2;
	  tappadlength = NUMPACK*maxpast;
	  signalIn.setSDFParams(NUMIN,maxpast-1);
	}
	codeblock(mainDecl) {
	  const int $starSymbol(NUMIN) = 1;
	  const int $starSymbol(NUMPACK) = 4;
	  double* $starSymbol(accumpair) = (double *)
	    memalign(sizeof(double),sizeof(double)*$starSymbol(NUMPACK));
	  float* $starSymbol(result) = (float *)
	    memalign(sizeof(double),sizeof(float)*$starSymbol(NUMPACK));
	  short* $starSymbol(shift_taparray) =
	    (short*)memalign(sizeof(double),sizeof(short)*
			     $starSymbol(NUMPACK)*$val(tappadlength));
	  int $starSymbol(taprowindex), $starSymbol(tapcolindex),$starSymbol(numloop);
	  short *$starSymbol(indexcount),$starSymbol(scaledown);
	}
	codeblock(initialize) {
	  vis_write_gsr(($val(scalefactor)+1)<<3);	  
	  $starSymbol(scaledown)=(short) 1<<$val(scalefactor);	  
	  /* initialize shifted taparrays to zero*/
	       $starSymbol(indexcount) = $starSymbol(shift_taparray);
	  for($starSymbol(taprowindex)=0;
	      $starSymbol(taprowindex)<$starSymbol(NUMPACK); $starSymbol(taprowindex)++){
	    for($starSymbol(tapcolindex)=0;
		$starSymbol(tapcolindex)<$val(tappadlength); $starSymbol(tapcolindex)++){
	      *$starSymbol(indexcount)++ = 0;
	    }
	  }
	  /* fill taparrays*/
	       for($starSymbol(taprowindex)=0;$starSymbol(taprowindex)<$starSymbol(NUMPACK);
		   $starSymbol(taprowindex)++){
		 $starSymbol(indexcount) = $starSymbol(shift_taparray) +
		   ($val(tappadlength)+1)*($starSymbol(taprowindex));
		 for($starSymbol(tapcolindex)=0
		       ;$starSymbol(tapcolindex)<$val(taplength); $starSymbol(tapcolindex)++){
		   /* scale and cast taps to short */
		     *$starSymbol(indexcount)++ = (short)
		       $val(scale)/$starSymbol(scaledown)*$ref2(taps,$starSymbol(tapcolindex));
		 }
	       }	  	
	       /* loop once for each set of filter taps*/
	       if ($val(taplength) == 0)
		 $starSymbol(numloop) = 0;	
	       else		
		 $starSymbol(numloop) = $val(maxpast);	
	}
	initCode {
          addInclude("<vis_proto.h>");
	  addDeclaration(mainDecl);
	  addCode(initialize);
	}
	codeblock(localDecl) {
          double *tapptr[4],tapvalue;
          double pairlohi,pairlolo,pairhilo,pairhihi;
          double pairlo,pairhi,pair,packedOut,*packedaccum;
          float packedhi,packedlo,datahi,datalo;
          float tappairhi,tappairlo,splithi,splitlo;
          int outerloop,innerloop,genindex;
	}
	codeblock(filter) {	  
	       /* initialize accumulator to zero*/
	       for(genindex=0;genindex<$starSymbol(NUMPACK);genindex++){
		 $starSymbol(accumpair)[genindex] = vis_fzero();
	       }
	       /*initialize the tapptr to point at the beginning of
		 each tap array*/
	       for(genindex=0;genindex<$starSymbol(NUMPACK);genindex++){
		 tapptr[genindex] = (double *) ($starSymbol(shift_taparray) +
		   genindex*$val(tappadlength));
	       }
	       /* filter data*/
	       for(outerloop=0;outerloop<$starSymbol(numloop);outerloop++){
		 /* set up data*/
		      datahi = vis_read_hi((double) $ref2(signalIn,outerloop));
		 datalo = vis_read_lo((double) $ref2(signalIn,outerloop));
		 /* calculate four outputs*/
		      for(innerloop=0;innerloop<$starSymbol(NUMPACK);
			  innerloop++){
			/* set up tap pairs for each shifted taparray*/
			     tapvalue = *tapptr[innerloop]++;
			tappairhi = vis_read_hi(tapvalue);
			tappairlo = vis_read_lo(tapvalue);
			/* take inner products*/
			     pairlolo = vis_fmuld8sux16(datalo,tappairlo);
			pairlohi = vis_fmuld8ulx16(datalo,tappairlo);	    
			pairhilo = vis_fmuld8sux16(datahi,tappairhi);
			pairhihi = vis_fmuld8ulx16(datahi,tappairhi);
			/* accumulate results*/
			     pairlo = vis_fpadd32(pairlolo,pairlohi);
			pairhi = vis_fpadd32(pairhilo,pairhihi);
			pair = vis_fpadd32(pairhi,pairlo);
			$starSymbol(accumpair)[innerloop] =
			  vis_fpadd32($starSymbol(accumpair)[innerloop],pair);
		      }  
	       }
	       /* sum accumulators and pack outputs into a double*/
	       for(genindex=0;genindex<$starSymbol(NUMPACK);genindex++){
		 splithi = vis_read_hi($starSymbol(accumpair)[genindex]);
		 splitlo = vis_read_lo($starSymbol(accumpair)[genindex]);
		 $starSymbol(result)[genindex] =
		   vis_fpadd32s(splithi,splitlo); 
	       }	  
	  packedaccum = (double *) $starSymbol(result);
	  packedhi = vis_fpackfix(*packedaccum);
	  packedaccum++;
	  packedlo = vis_fpackfix(*packedaccum);
	  packedOut = vis_freg_pair(packedhi,packedlo);	  
	  $ref(signalOut) = packedOut;
	}
	go {   
	  addCode(localDecl);
	  addCode(filter);
	}
	codeblock(freeit){	  
	  free($starSymbol(shift_taparray));
	  free($starSymbol(result));
	  free($starSymbol(accumpair));
	}
	wrapup {
	  addCode(freeit);
	}
}

