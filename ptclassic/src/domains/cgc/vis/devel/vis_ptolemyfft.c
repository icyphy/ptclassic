/*
 * User:      williamc Date:      Thu May 30 02:36:06 1996 Target:    CGCVIS
 * Universe:  256fft
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/times.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <math.h>
#include <vis_proto.h> 
#if !defined(NO_FIX_SUPPORT)
/* include definitions for the fix runtime library */
#include "CGCrtlib.h"
#endif
#define NUMTIMES (100)
#if !defined(COMPLEX_DATA)
#define COMPLEX_DATA 1
typedef struct complex_data {
  double          real;
  double          imag;
}	complex;
#endif

extern int      main(int argc, char *argv[]);

static void calcTwSinCos(double *Twsine, double *Twcosine, double ExpofW, int N2)
{
  double          scale = 32767.0;
  int             i;
  short          *indexcount0, *indexcount1;

  indexcount0 = (short *) Twcosine;
  indexcount1 = (short *) Twsine;
  for (i = 0; i < N2; i++) {
    *indexcount0++ = (short) scale *cos(ExpofW * (N2 - 1 - i));
    *indexcount1++ = (short) -scale * sin(ExpofW * (N2 - 1 - i));
  }
}
/*static double mult4x4(double mult1, double mult2)
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
}*/	
static float mult2x2(float mult1, float mult2)
{
  double          resultu, resultl, result;
  float           product;

  resultu = vis_fmuld8sux16(mult1, mult2);
  resultl = vis_fmuld8ulx16(mult1, mult2);
  result = vis_fpadd32(resultu, resultl);
  return product = vis_fpackfix(result);
}
#define SWAP(a, b) tempr=(a); (a)=(b); (b)=tempr

static void reorderfft(short *s1, short *s2, int nn)
{
  unsigned long   n, j, i, m;
  short           tempr;

  n = nn << 1;
  j = 1;
  for (i = 1; i < n; i += 2) {
    if (j > i) {
      SWAP(s1[nn - 1 - (j - 1) / 2], s1[nn - 1 - (i - 1) / 2]);
      SWAP(s2[nn - (j - 1) / 2 - 1], s2[nn - (i - 1) / 2 - 1]);
    }
    m = n >> 1;
    while (m >= 2 && j > m) {
      j -= m;
      m >>= 1;
    }
    j += m;
  }
}
/* main function */
int main(int argc, char *argv[])
{
  short          *packedout_17 =
    (short *) memalign(sizeof(double), sizeof(short) * 4);
  short          *packedout_19 =
    (short *) memalign(sizeof(double), sizeof(short) * 4);
  double         *rein_20 = (double *)
    memalign(sizeof(double), sizeof(double) * 256 / 4);
  double         *imin_21 = (double *)
    memalign(sizeof(double), sizeof(double) * 256 / 4);
  double         *Twcosine_22 = (double *)
    memalign(sizeof(double), sizeof(double) * 256 / 8);
  double         *Twsine_23 = (double *)
    memalign(sizeof(double), sizeof(double) * 256 / 8);
  double          twopi_24 = 6.28318530717959;
  double         *packedin_26 =
    (double *) memalign(sizeof(double), sizeof(double));
  double         *packedin_28 =
    (double *) memalign(sizeof(double), sizeof(double));
  FILE           *fp_30[1];
  FILE           *fp_33[1];
  double          value_37;
  double          output_0;
  complex         output_1;
  double          output_4;
  double          output_5;
  double          real_6[4];
  int             real_14;
  double          imag_7[4];
  int             imag_15;
  double          out_8[64];
  int             out_16;
  double          out_9[64];
  int             out_18;
  double          realOut_10[64];
  double          imagOut_11[64];
  int             in_25;
  double          out_12[4];
  int             in_27;
  double          out_13[4];
  int             count_40;
  double          index_41;
  int             input_1_29;
  int             count_43;
  double          index_44;
  int             input_1_32;
  hrtime_t start, end;
  float    time1, time2, time3, meansampletime, runningtotal;

  runningtotal=0.0;
  count_43 = 0;
  index_44 = 0.0;
  count_40 = 0;
  index_41 = 0.0;
  value_37 = 0.0;
  output_0 = 0.0;
  output_1.real = output_1.imag = 0.0;
  output_4 = 0.0;
  output_5 = 0.0;
  {
    int             i;
    for (i = 0; i < 4; i++)
      real_6[i] = 0.0;
  }
  real_14 = 0;
  {
    int             i;
    for (i = 0; i < 4; i++)
      imag_7[i] = 0.0;
  }
  imag_15 = 0;
  {
    int             i;
    for (i = 0; i < 64; i++)
      out_8[i] = 0.0;
  }
  out_16 = 0;
  {
    int             i;
    for (i = 0; i < 64; i++)
      out_9[i] = 0.0;
  }
  out_18 = 0;
  {
    int             i;
    for (i = 0; i < 64; i++)
      realOut_10[i] = 0.0;
  }
  {
    int             i;
    for (i = 0; i < 64; i++)
      imagOut_11[i] = 0.0;
  }
  vis_write_gsr(8);
  in_25 = 0;
  {
    int             i;
    for (i = 0; i < 4; i++)
      out_12[i] = 0.0;
  }
  in_27 = 0;
  {
    int             i;
    for (i = 0; i < 4; i++)
      out_13[i] = 0.0;
  }
  input_1_29 = 0;
  if (!(fp_30[0] = fopen("CGCVIS_temp_310", "w"))) {
    fprintf(stderr, "ERROR: cannot open output file for Xgraph star.\n");
    exit(1);
  }
  input_1_32 = 0;
  if (!(fp_33[0] = fopen("CGCVIS_temp_340", "w"))) {
    fprintf(stderr, "ERROR: cannot open output file for Xgraph star.\n");
    exit(1);
  } {
    int             sdfLoopCounter_45;
    for (sdfLoopCounter_45 = 0; sdfLoopCounter_45 < NUMTIMES;
	 sdfLoopCounter_45++)
 {
      {
	int             sdfLoopCounter_35;
	for (sdfLoopCounter_35 = 0; sdfLoopCounter_35 < 64;
	     sdfLoopCounter_35++) {
	  {
	    int             sdfLoopCounter_36;
	    for (sdfLoopCounter_36 = 0; sdfLoopCounter_36 < 4;
		 sdfLoopCounter_36++) {
	      {	/* star
		 * 256fft.expgen1.Ramp1
		 * (class CGCRamp) */
		output_0 = value_37;
		value_37 += 0.0628318530717958;
	      }
	      {	/* star
		 * 256fft.expgen1.expjx1.
		 * Fork.output=21 (class
		 * CGCFork) */
	      }
	      {	/* star
		 * 256fft.expgen1.expjx1.
		 * Cos1 (class CGCCos) */
		output_4 = cos(output_0);
	      }
	      {	/* star
		 * 256fft.expgen1.expjx1.
		 * Sin1 (class CGCSin) */
		output_5 = sin(output_0);
	      }
	      {	/* star
		 * 256fft.expgen1.expjx1.
		 * RectToCx1 (class
		 * CGCRectToCx) */
		output_1.real = output_4;
		output_1.imag = output_5;
	      }
	      {	/* star 256fft.CxToRect1
		 * (class CGCCxToRect) */
		real_6[real_14] = output_1.real;
		imag_7[imag_15] = output_1.imag;
		real_14 += 1;
		if (real_14 >= 4)
		  real_14 -= 4;
		imag_15 += 1;
		if (imag_15 >= 4)
		  imag_15 -= 4;
	      }
	    }
	  }	/* end repeat, depth 2 */
	  {	/* star 256fft.PackVis642
		 * (class CGCPackVis64) */
	    double         *outvalue;
	    /* scale,cast,and pack input */
	    packedout_19[0] = (short)
	      (127.99609375 * (double) imag_7[(3 - (0))]);
	    packedout_19[1] = (short)
	      (127.99609375 * (double) imag_7[(3 - (1))]);
	    packedout_19[2] = (short)
	      (127.99609375 * (double) imag_7[(3 - (2))]);
	    packedout_19[3] = (short)
	      (127.99609375 * (double) imag_7[(3 - (3))]);
	    /* output packed double */
	    outvalue = (double *) packedout_19;
	    out_9[out_18] = *outvalue;
	    out_18 += 1;
	    if (out_18 >= 64)
	      out_18 -= 64;
	  }
	  {	/* star 256fft.PackVis641
		 * (class CGCPackVis64) */
	    double         *outvalue;
	    /* scale,cast,and pack input */
	    packedout_17[0] = (short)
	      (127.99609375 * (double) real_6[(3 - (0))]);
	    packedout_17[1] = (short)
	      (127.99609375 * (double) real_6[(3 - (1))]);
	    packedout_17[2] = (short)
	      (127.99609375 * (double) real_6[(3 - (2))]);
	    packedout_17[3] = (short)
	      (127.99609375 * (double) real_6[(3 - (3))]);
	    /* output packed double */
	    outvalue = (double *) packedout_17;
	    out_8[out_16] = *outvalue;
	    out_16 += 1;
	    if (out_16 >= 64)
	      out_16 -= 64;
	  }
	}
      }	/* end repeat, depth 1 */
      {	/* star 256fft.QuadFFTCx1 (class
	 * CGCQuadFFTCx) */
	double          ExpofW, reindtmp, imagindtmp;
	double          xtcd, xtsd, ytsd, ytcd;
	float           xtcf, ytsf, xtsf, ytcf;
	float           reinftmp, imaginftmp;
	float          *splitf_rein, *splitf_imin;
	float          *Cf, *Sf;
	int             N, N1, N2, i, j, k, l;
	short          *splits_rein, *splits_imin,reinstmp, imaginstmp;
	double          prodhihi, prodhilo, prodlohi, prodlolo;
	double          prodhi, prodlo, product;
	float           mult1hi, mult1lo, mult2hi, mult2lo;
	float           packhi, packlo;


	splitf_rein = (float *) rein_20;
	splitf_imin = (float *) imin_21;
	splits_rein = (short *) rein_20;
	splits_imin = (short *) imin_21;
	/* read in the input */
	for (i = 0; i < 256 / 4; i++) {
	  rein_20[i] = (double) out_8[(63 - (i))];
	  imin_21[i] = (double) out_9[(63 - (i))];
	}
	/*
	 * first stages of fft (order of fft minus
	 * last two)
	 */
	start = gethrtime();
	N = 256;
	N2 = N;
	for (k = 0; k < 8 - 2; k++) {
	  N1 = N2;
	  N2 = N2 / 2;
	  ExpofW = twopi_24 / N1;
	    calcTwSinCos(Twsine_23, Twcosine_22, ExpofW, N2);
	  for (j = 0; j < N2 / 4; j++) {
	    for (i = j; i < N / 4; i += N1 / 4) {
	      l = i + N2 / 4;
	      reindtmp = vis_fpsub16(rein_20[l], rein_20[i]);
	      rein_20[l] =
		vis_fpadd16(rein_20[i], rein_20[l]);
	      imagindtmp = vis_fpsub16(imin_21[l], imin_21[i]);
	      imin_21[l] =
		vis_fpadd16(imin_21[i], imin_21[l]);
	      /*xtcd*/
	      mult1hi = vis_read_hi(reindtmp);
	      mult1lo = vis_read_lo(reindtmp);
	      mult2hi = vis_read_hi(Twcosine_22[j]);
	      mult2lo = vis_read_lo(Twcosine_22[j]);
	      prodhihi = vis_fmuld8sux16(mult1hi, mult2hi);
	      prodhilo = vis_fmuld8ulx16(mult1hi, mult2hi);
	      prodlohi = vis_fmuld8sux16(mult1lo, mult2lo);
	      prodlolo = vis_fmuld8ulx16(mult1lo, mult2lo);
	      prodhi = vis_fpadd32(prodhihi, prodhilo);
	      prodlo = vis_fpadd32(prodlohi, prodlolo);
	      packhi = vis_fpackfix(prodhi);
	      packlo = vis_fpackfix(prodlo);
	      xtcd = vis_freg_pair(packhi, packlo);
	      /*xtsd*/
	      mult1hi = vis_read_hi(reindtmp);
	      mult1lo = vis_read_lo(reindtmp);
	      mult2hi = vis_read_hi(Twsine_23[j]);
	      mult2lo = vis_read_lo(Twsine_23[j]);
	      prodhihi = vis_fmuld8sux16(mult1hi, mult2hi);
	      prodhilo = vis_fmuld8ulx16(mult1hi, mult2hi);
	      prodlohi = vis_fmuld8sux16(mult1lo, mult2lo);
	      prodlolo = vis_fmuld8ulx16(mult1lo, mult2lo);
	      prodhi = vis_fpadd32(prodhihi, prodhilo);
	      prodlo = vis_fpadd32(prodlohi, prodlolo);
	      packhi = vis_fpackfix(prodhi);
	      packlo = vis_fpackfix(prodlo);
	      xtsd = vis_freg_pair(packhi, packlo);
	      /*ytsd*/
	      mult1hi = vis_read_hi(imagindtmp);
	      mult1lo = vis_read_lo(imagindtmp);
	      mult2hi = vis_read_hi(Twsine_23[j]);
	      mult2lo = vis_read_lo(Twsine_23[j]);
	      prodhihi = vis_fmuld8sux16(mult1hi, mult2hi);
	      prodhilo = vis_fmuld8ulx16(mult1hi, mult2hi);
	      prodlohi = vis_fmuld8sux16(mult1lo, mult2lo);
	      prodlolo = vis_fmuld8ulx16(mult1lo, mult2lo);
	      prodhi = vis_fpadd32(prodhihi, prodhilo);
	      prodlo = vis_fpadd32(prodlohi, prodlolo);
	      packhi = vis_fpackfix(prodhi);
	      packlo = vis_fpackfix(prodlo);
	      ytsd = vis_freg_pair(packhi, packlo);
	      /*ytcd*/
	      mult1hi = vis_read_hi(imagindtmp);
	      mult1lo = vis_read_lo(imagindtmp);
	      mult2hi = vis_read_hi(Twcosine_22[j]);
	      mult2lo = vis_read_lo(Twcosine_22[j]);
	      prodhihi = vis_fmuld8sux16(mult1hi, mult2hi);
	      prodhilo = vis_fmuld8ulx16(mult1hi, mult2hi);
	      prodlohi = vis_fmuld8sux16(mult1lo, mult2lo);
	      prodlolo = vis_fmuld8ulx16(mult1lo, mult2lo);
	      prodhi = vis_fpadd32(prodhihi, prodhilo);
	      prodlo = vis_fpadd32(prodlohi, prodlolo);
	      packhi = vis_fpackfix(prodhi);
	      packlo = vis_fpackfix(prodlo);
	      ytcd = vis_freg_pair(packhi, packlo);
	      rein_20[i] = vis_fpsub16(xtcd, ytsd);
	      imin_21[i] = vis_fpadd16(xtsd, ytcd);
	    }
	  }
	}
	/* second to last stage of the fft */
	N1 = N2;
	N2 = N2 / 2;
	ExpofW = twopi_24 / N1;
	calcTwSinCos(Twsine_23, Twcosine_22, ExpofW, N2);
	Cf = (float *) Twcosine_22;
	Sf = (float *) Twsine_23;
	j = 0;
	for (i = 0; i < N / 4; i++) {
	  reinftmp = vis_fpsub16s(splitf_rein[j + 1], splitf_rein[j]);
	  imaginftmp = vis_fpsub16s(splitf_imin[j + 1], splitf_imin[j]);
	  splitf_rein[j + 1] = vis_fpadd16s(splitf_rein[j + 1],
					    splitf_rein[j]);
	  splitf_imin[j + 1] = vis_fpadd16s(splitf_imin[j + 1],
					    splitf_imin[j]);
	  xtcf = mult2x2(reinftmp, *Cf);
	  xtsf = mult2x2(reinftmp, *Sf);
	  ytsf = mult2x2(imaginftmp, *Sf);
	  ytcf = mult2x2(imaginftmp, *Cf);
	  splitf_rein[j] = vis_fpsub16s(xtcf, ytsf);
	  splitf_imin[j] = vis_fpadd16s(xtsf, ytcf);
	  j += 2;
	}
	/* last stage of the fft */
	i = 0;
	for (j = 0; j < N / 2; j++) {
	  reinstmp = splits_rein[i + 1] - splits_rein[i];
	  imaginstmp = splits_imin[i + 1] - splits_imin[i];
	  splits_rein[i + 1] = splits_rein[i] + splits_rein[i + 1];
	  splits_imin[i + 1] = splits_imin[i] + splits_imin[i + 1];
	  splits_rein[i] = reinstmp;
	  splits_imin[i] = imaginstmp;
	  i += 2;
	}
	end = gethrtime();
	time3 = (float) (end - start);
	runningtotal+=time3;
	/* output the results */
	for (i = 0; i < 256 / 4; i++) {
	  realOut_10[(63 - (i))] = rein_20[i];
	  imagOut_11[(63 - (i))] = imin_21[i];
	}
      }
      {
	int             sdfLoopCounter_38;
	for (sdfLoopCounter_38 = 0; sdfLoopCounter_38 < 64;
	     sdfLoopCounter_38++) {
	  {	/* star 256fft.UnpackVis641
		 * (class CGCUnpackVis64) */
	    double          outvalue;
	    short          *invalue;
	    
	    *packedin_26 = (double) realOut_10[in_25];
	    invalue = (short *) packedin_26;
	    /*
	     * scale down and unpack
	     * input
	     */
	    out_12[(3 - (0))] = (double) (0.00781273842585528 *
					  (double) invalue[0]);
	    out_12[(3 - (1))] = (double) (0.00781273842585528 *
					  (double) invalue[1]);
	    out_12[(3 - (2))] = (double) (0.00781273842585528 *
					  (double) invalue[2]);
	    out_12[(3 - (3))] = (double) (0.00781273842585528 *
					  (double) invalue[3]);
	    in_25 += 1;
	    if (in_25 >= 64)
	      in_25 -= 64;
	  }
	  {
	    int             sdfLoopCounter_39;
	    for (sdfLoopCounter_39 = 0; sdfLoopCounter_39 < 4;
		 sdfLoopCounter_39++) {
	      {	/* star 256fft.XMgraph1
		 * (class CGCXMgraph) */
		if (++count_40 >= 0) {
		  fprintf(fp_30[0], "%g %g\n", index_41, out_12[input_1_29]);
		}
		index_41 += 1.0;
		input_1_29 += 1;
		if (input_1_29 >= 4)
		  input_1_29 -= 4;
	      }
	    }
	  }	/* end repeat, depth 2 */
	  {	/* star 256fft.UnpackVis642
		 * (class CGCUnpackVis64) */
	    double          outvalue;
	    short          *invalue;
	    
	    *packedin_28 = (double) imagOut_11[in_27];
	    invalue = (short *) packedin_28;
	    /*
	     * scale down and unpack
	     * input
	     */
	    out_13[(3 - (0))] = (double) (0.00781273842585528 *
					  (double) invalue[0]);
	    out_13[(3 - (1))] = (double) (0.00781273842585528 *
					  (double) invalue[1]);
	    out_13[(3 - (2))] = (double) (0.00781273842585528 *
					  (double) invalue[2]);
	    out_13[(3 - (3))] = (double) (0.00781273842585528 *
					  (double) invalue[3]);
	    in_27 += 1;
	    if (in_27 >= 64)
	      in_27 -= 64;
	  }
	  {
	    int             sdfLoopCounter_42;
	    for (sdfLoopCounter_42 = 0; sdfLoopCounter_42 < 4;
		 sdfLoopCounter_42++) {
	      {	/* star 256fft.XMgraph2
		 * (class CGCXMgraph) */
		if (++count_43 >= 0) {
		  fprintf(fp_33[0], "%g %g\n", index_44, out_13[input_1_32]);
		}
		index_44 += 1.0;
		input_1_32 += 1;
		if (input_1_32 >= 4)
		  input_1_32 -= 4;
	      }
	    }
	  }	/* end repeat, depth 2 */
	}
      }	/* end repeat, depth 1 */
    }
  }			/* end repeat, depth 0 */
  meansampletime=(float) runningtotal/(NUMTIMES)/1000;
  printf("mean sample time =%f microsec\n",meansampletime);
  free(packedout_17);
  free(packedout_19);
  free(rein_20);
  free(imin_21);
  free(Twcosine_22);
  free(Twsine_23);
  free(packedin_26);
  free(packedin_28);
  {
    int             i;
    for (i = 0; i < 1; i++)
      fclose(fp_30[i]);
    system("( pxgraph -t 'real' -bb -tk =800x400 CGCVIS_temp_310;/bin/rm -f CGCVIS_temp_310) &");
  }
  {
    int             i;
    for (i = 0; i < 1; i++)
      fclose(fp_33[i]);
    system("( pxgraph -t 'imag' -bb -tk =800x400 CGCVIS_temp_340;/bin/rm -f CGCVIS_temp_340) &");
  }
  
  return 1;
}
