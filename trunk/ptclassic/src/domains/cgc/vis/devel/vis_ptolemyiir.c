#include <stdlib.h>
#include <stdio.h>
#include "vis_types.h"
#include "vis_proto.h"

void vdk_vis_ptolemyfir(double* src, double* dst1, int dlen, double*
		   shift_taparray, int numloop)
{
  double	*tapptr[4], tapvalue;
  double	pairlohi, pairlolo, pairhilo,pairhihi;
  double 	pairlo, pairhi, pair, packedOut,*packedaccum;
  float  	packedhi, packedlo, datahi[1],datalo[1];
  float 	tappairhi, tappairlo, splithi,splitlo;
  int  		outerloop, innerloop,nminusk;
  int	     	n,i,j,k;
  double 	result[2];
  double	accumpair[4];
  short 	*t0,*t1,*t2;
  double	resultcheck[2];
  float		*f1,*r1,*r;
  float		resultprint[2];
  short		*s1;
  double	d[1];

  for(i=0;i<dlen;i++){
    for (j = 0; j < 4; j++) {
      accumpair[j] = vis_fzero();
      tapptr[j] = (double *) (shift_taparray + j * numloop);
    }
    nminusk = i;
    n = numloop;
    if(nminusk<numloop)
      n=nminusk+1;
    /* filter data */
    for (outerloop = 0; outerloop < n; outerloop++) {
      /* set up data */
      *datahi = vis_read_hi(src[nminusk]);
      *datalo = vis_read_lo(src[nminusk]);
      /* calculate four outputs */
      for (innerloop = 0; innerloop < 4; innerloop++) {
	/*
	 * set up tap pairs for each
	 * shifted taparray
	 */
	tapvalue = *tapptr[innerloop]++;
	tappairhi = vis_read_hi(tapvalue);
	tappairlo = vis_read_lo(tapvalue);
	/* take inner products */
	pairlolo = vis_fmuld8sux16(*datalo, tappairlo);
	pairlohi = vis_fmuld8ulx16(*datalo, tappairlo);
	pairhilo = vis_fmuld8sux16(*datahi, tappairhi);
	pairhihi = vis_fmuld8ulx16(*datahi, tappairhi);
	/* accumulate results */
	pairlo = vis_fpadd32(pairlolo, pairlohi);
	pairhi = vis_fpadd32(pairhilo, pairhihi);
	pair = vis_fpadd32(pairhi, pairlo);
	accumpair[innerloop] = vis_fpadd32(accumpair[innerloop],pair);

      }
      nminusk--;
    }
    /*
     * sum accumulators and pack outputs into a
     * double
     */
    for (k = 3; k >=0; k--) {
      splithi = vis_read_hi(accumpair[3-k]);
      splitlo = vis_read_lo(accumpair[3-k]);
      r=(float *) result;
      r[k] = vis_fpadd32s(splithi, splitlo);
    }
    packedaccum = (double *) result;
    packedhi = vis_fpackfix(*packedaccum);
    packedaccum++;
    packedlo = vis_fpackfix(*packedaccum);
    dst1[i] = vis_freg_pair(packedhi, packedlo);
  }	
}

