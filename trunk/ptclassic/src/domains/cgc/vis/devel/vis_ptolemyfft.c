#include <stdlib.h>
#include<math.h>
#include "vis_types.h"
#include "vis_proto.h"
/***************************************************************/
void calcTwSinCos(double *Twsine, double *Twcosine, double ExpofW, int N2)
{
  double          scale = 32767.0;
  int             i;
  short          *indexcount0, *indexcount1,*tmp0,*tmp1;

  indexcount0 = (short *) Twcosine;
  indexcount1 = (short *) Twsine;
  for (i = 0; i < N2; i++) {
    *indexcount0++ = (short) scale *cos(ExpofW * (i));
    *indexcount1++ = (short) -scale * sin(ExpofW * (i));
  }
}
/***************************************************************/
double mult4x4(double mult1, double mult2)
{
  double          prodhihi, prodhilo, prodlohi, prodlolo;
  double          prodhi, prodlo, product;
  float           mult1hi, mult1lo, mult2hi, mult2lo;
  float           packhi, packlo;

  mult1hi = vis_read_hi(mult1);
  mult1lo = vis_read_lo(mult1);
  mult2hi = vis_read_hi(mult2);
  mult2lo = vis_read_lo(mult2);
  
  prodhihi = vis_fmuld8sux16(mult1hi, mult2hi);
  prodhilo = vis_fmuld8ulx16(mult1hi, mult2hi);
  prodlohi = vis_fmuld8sux16(mult1lo, mult2lo);
  prodlolo = vis_fmuld8ulx16(mult1lo, mult2lo);
  
  prodhi = vis_fpadd32(prodhihi, prodhilo);
  prodlo = vis_fpadd32(prodlohi, prodlolo);
  packhi = vis_fpackfix(prodhi);
  packlo = vis_fpackfix(prodlo);
  return product = vis_freg_pair(packhi, packlo);
}	
/***************************************************************/
float mult2x2(float mult1, float mult2)
{
  double          resultu, resultl, result;
  float           product;

  resultu = vis_fmuld8sux16(mult1, mult2);
  resultl = vis_fmuld8ulx16(mult1, mult2);
  result = vis_fpadd32(resultu, resultl);
  return product = vis_fpackfix(result);
}
/***************************************************************/
#define SWAP(a, b) tempr=(a); (a)=(b); (b)=tempr
/***************************************************************/
void reordershortfft(short *s1, short *s2, int nn)
{
  unsigned long   n, j, i, m;
  short           tempr;

  n = nn << 1;
  j = 1;
  for (i = 1; i < n; i += 2) {
    if (j > i) {
      SWAP(s1[(j - 1) / 2], s1[ (i - 1) / 2]);
      SWAP(s2[(j - 1) / 2 ], s2[ (i - 1) / 2 ]);
    }
    m = n >> 1;
    while (m >= 2 && j > m) {
      j -= m;
      m >>= 1;
    }
    j += m;
  }
}
/***************************************************************/
void vdk_vis_ptolemyfft(double* rein, double* imin, int length,
			double* twC, double* twS){
  double reindtmp, imagindtmp;
  double xtcd, xtsd, ytsd, ytcd;
  double t0,t1;
  float  xtcf, ytsf, xtsf, ytcf;
  float  reinftmp, imaginftmp;
  float  *splitf_rein, *splitf_imin;
  float  *Cf, *Sf, C0, S0;
  int    N, N1, N2, i, j, k, l;
  short  *splits_rein, *splits_imin, reinstmp, imaginstmp;

  splitf_rein = (float *) rein;
  splitf_imin = (float *) imin;
  splits_rein = (short *) rein;
  splits_imin = (short *) imin;
  /*
   * first stages of fft (order of fft minus
   * last two)
   */
  N = length;
  N2 = N;
  for (k = 0; k < 6; k++) {
    N1 = N2;
    N2 = N2 / 2;
    for (j = 0; j < N2 / 4; j++) {
      t0 = *twC++;
      t1 = *twS++;
      for (i = j; i < N / 4; i += N1 / 4) {
	l = i + N2 / 4;
	reindtmp = vis_fpsub16(rein[i], rein[l]);
	rein[i] = vis_fpadd16(rein[i], rein[l]);
	imagindtmp = vis_fpsub16(imin[i], imin[l]);
	imin[i] = vis_fpadd16(imin[i], imin[l]);
	xtcd = mult4x4(reindtmp,t0);
	xtsd = mult4x4(reindtmp,t1);
	ytsd = mult4x4(imagindtmp,t1);
	ytcd = mult4x4(imagindtmp,t0);
	rein[l] = vis_fpsub16(xtcd, ytsd);
	imin[l] = vis_fpadd16(xtsd, ytcd);
      }
    }
  }
  /* second to last stage of the fft */
  C0 = vis_to_float(0x7fff<<16|0x0000);
  S0 = vis_to_float(0x0000<<16|0x8000);
  j = 0;
  for (i = 0; i < N / 4; i++) {
    reinftmp = vis_fpsub16s(splitf_rein[j], splitf_rein[j+1]);
    imaginftmp = vis_fpsub16s(splitf_imin[j], splitf_imin[j+1]);
    splitf_rein[j] = vis_fpadd16s(splitf_rein[j + 1], splitf_rein[j]);
    splitf_imin[j] = vis_fpadd16s(splitf_imin[j + 1], splitf_imin[j]);
    xtcf = mult2x2(reinftmp, C0);
    xtsf = mult2x2(reinftmp, S0);
    ytsf = mult2x2(imaginftmp, S0);
    ytcf = mult2x2(imaginftmp, C0);
    splitf_rein[j+1] = vis_fpsub16s(xtcf, ytsf);
    splitf_imin[j+1] = vis_fpadd16s(xtsf, ytcf);
    j += 2;
  }

  /* last stage of the fft */
  i = 0;
  for (j = 0; j < N / 2; j++) {
    reinstmp = splits_rein[i] - splits_rein[i+1];
    imaginstmp = splits_imin[i] - splits_imin[i+1];
    splits_rein[i] = splits_rein[i] + splits_rein[i + 1];
    splits_imin[i] = splits_imin[i] + splits_imin[i + 1];
    splits_rein[i+1] = reinstmp;
    splits_imin[i+1] = imaginstmp;
    i += 2;
  }
}	
/***************************************************************/
