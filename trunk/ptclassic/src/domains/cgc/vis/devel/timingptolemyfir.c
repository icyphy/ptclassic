/* Copyright (C) 1995, Sun Microsystems, Inc. */

/***************************************************************/
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/uio.h>
#include "vis_types.h"

#define NTIME 1000
#define LENGTH 8192
#define NTAPS 32
#define NPACK 4
#define SCALE 32767
/***************setup program arguments**************************/

main (int argc, char *argv[])
{
  extern  char *optarg;
  extern  int   optind;
  int           c;
  int      ntime, length;
  int      verbose,print;
  hrtime_t start, end;
  float    time1, time2,time3;
  double sinarg,twopiover50;
  vis_s16  *dst0,*src0,*srcptolemy,*filtertaps,*shift_taparray,*indexcount;
  int      i,j,maxpast,tappadlength,taprowindex,tapcolindex;
  int	   scalefactor,scaledown,numloop,taplength;
  vis_d64  *dst1,*dst2,*src1,*filtertaps_d64;
  short    *tmp;

  twopiover50 = 0.0;
  ntime = NTIME;
  length = LENGTH;
  taplength = NTAPS;
  verbose = 0;
  print = 0;
  while ((c = getopt(argc, argv, "hvpn:l:t:")) != -1)
    switch (c) {
    case 'h':
      printf("usage: %s [-h] [-v] [-p] [-n ntimes] [-l length] [-t ntaps]\n", argv[0]);
      printf("  Default Setting  \n");
      printf("===================\n");
      printf(" NTIMES = 1000 \n");
      printf(" LENGTH = 8192 \n");
      printf(" NTAPS = 16 \n");
      exit(1);
    case 'v': verbose = 1;
      break;
    case 'p': print = 1;
      break;
    case 'n': ntime = atoi(optarg);
      break;
    case 'l': length = atoi(optarg);
      break;
    case 't': length = atoi(optarg);
      break;
    };
    scalefactor = 0;
/**************allocate memory************************************/
dst0 = (vis_s16 *) memalign(sizeof(double),sizeof(short)*LENGTH);
dst1 = (vis_d64 *) memalign(sizeof(double),sizeof(double)*LENGTH);
dst2 = (vis_d64 *) memalign(sizeof(double),sizeof(double)*LENGTH/4);
src0 = (vis_s16 *) memalign(sizeof(double),sizeof(short)*LENGTH);
src1 = (vis_d64 *) memalign(sizeof(double),sizeof(double)*LENGTH);
filtertaps = (vis_s16 *) memalign(sizeof(double),sizeof(short)*NTAPS);
filtertaps_d64 = (vis_d64 *) memalign(sizeof(double),sizeof(double)*NTAPS);
srcptolemy = (vis_s16 *) memalign(sizeof(double),sizeof(short)*length);
/**************setup filtertaps***********************************/
  for(i=0;i<LENGTH;i++){
    sinarg = twopiover50;
    twopiover50 += 0.628318530717958;
    src0[i] = (short)SCALE * (double) sin(sinarg);
    src1[i] = sin(sinarg);
  }
    filtertaps[0]=(short)SCALE*5.749331917706068e-02;
    filtertaps[1]=(short)SCALE*2.356117725608943e-01;
    filtertaps[2]=(short)SCALE*4.326064391182815e-01;
    filtertaps[3]=(short)SCALE*3.9298714665431733e-01;
    filtertaps[4]=(short)SCALE*6.9707352819524643e-02;
    filtertaps[5]=(short)SCALE*-1.9527235873539017e-01;
    filtertaps[6]=(short)SCALE*-1.1501073848243082e-01;
    filtertaps[7]=(short)SCALE*1.0384807075682692e-01;
    filtertaps[8]=(short)SCALE*1.0127325405350047e-01;
    filtertaps[9]=(short)SCALE*-6.2250082138045150e-02;
    filtertaps[10]=(short)SCALE*-8.0637756012925166e-02;
    filtertaps[11]=(short)SCALE*4.1872874030868444e-02;
    filtertaps[12]=(short)SCALE*6.1733323254462361e-02;
    filtertaps[13]=(short)SCALE*-3.0889724643843371e-02;
    filtertaps[14]=(short)SCALE*-4.5776126368330651e-02;
    filtertaps[15]=(short)SCALE*2.4137901399342250e-02;
    filtertaps[16]=(short)SCALE*5.749331917706068e-02;
    filtertaps[17]=(short)SCALE*2.356117725608943e-01;
    filtertaps[18]=(short)SCALE*4.326064391182815e-01;
    filtertaps[19]=(short)SCALE*3.9298714665431733e-01;
    filtertaps[20]=(short)SCALE*6.9707352819524643e-02;
    filtertaps[21]=(short)SCALE*-1.9527235873539017e-01;
    filtertaps[22]=(short)SCALE*-1.1501073848243082e-01;
    filtertaps[23]=(short)SCALE*1.0384807075682692e-01;
    filtertaps[24]=(short)SCALE*1.0127325405350047e-01;
    filtertaps[25]=(short)SCALE*-6.2250082138045150e-02;
    filtertaps[26]=(short)SCALE*-8.0637756012925166e-02;
    filtertaps[27]=(short)SCALE*4.1872874030868444e-02;
    filtertaps[28]=(short)SCALE*6.1733323254462361e-02;
    filtertaps[29]=(short)SCALE*-3.0889724643843371e-02;
    filtertaps[30]=(short)SCALE*-4.5776126368330651e-02;
    filtertaps[31]=(short)SCALE*2.4137901399342250e-02;

    filtertaps_d64[0]= 5.749331917706068e-02;
    filtertaps_d64[1]= 2.356117725608943e-01;
    filtertaps_d64[2]= 4.326064391182815e-01;
    filtertaps_d64[3]= 3.9298714665431733e-01;
    filtertaps_d64[4]= 6.9707352819524643e-02;
    filtertaps_d64[5]= -1.9527235873539017e-01;
    filtertaps_d64[6]= -1.1501073848243082e-01;
    filtertaps_d64[7]= 1.0384807075682692e-01;
    filtertaps_d64[8]= 1.0127325405350047e-01;
    filtertaps_d64[9]= -6.2250082138045150e-02;
    filtertaps_d64[10]= -8.0637756012925166e-02;
    filtertaps_d64[11]= 4.1872874030868444e-02;
    filtertaps_d64[12]= 6.1733323254462361e-02;
    filtertaps_d64[13]= -3.0889724643843371e-02;
    filtertaps_d64[14]= -4.5776126368330651e-02;
    filtertaps_d64[15]= 2.4137901399342250e-02;
    filtertaps_d64[16]= 5.749331917706068e-02;
    filtertaps_d64[17]= 2.356117725608943e-01;
    filtertaps_d64[18]= 4.326064391182815e-01;
    filtertaps_d64[19]= 3.9298714665431733e-01;
    filtertaps_d64[20]= 6.9707352819524643e-02;
    filtertaps_d64[21]= -1.9527235873539017e-01;
    filtertaps_d64[22]= -1.1501073848243082e-01;
    filtertaps_d64[23]= 1.0384807075682692e-01;
    filtertaps_d64[24]= 1.0127325405350047e-01;
    filtertaps_d64[25]= -6.2250082138045150e-02;
    filtertaps_d64[26]= -8.0637756012925166e-02;
    filtertaps_d64[27]= 4.1872874030868444e-02;
    filtertaps_d64[28]= 6.1733323254462361e-02;
    filtertaps_d64[29]= -3.0889724643843371e-02;
    filtertaps_d64[30]= -4.5776126368330651e-02;
    filtertaps_d64[31]= 2.4137901399342250e-02;
/***************************************************************/
  if (verbose) {
    printf("NTIME=%d\n", ntime);
    printf("\n");
    printf("LENGTH=%d\n", length);
    printf("\n");
    printf("NTAPS=%d\n", taplength);
    printf("\n");
  }
/****************measure integer unit fir************************/
  start = gethrtime();
  for (j = 0; j < ntime; j ++) {
    vdk_cint_ptolemyfir(src0, dst0, length, filtertaps, taplength);
  }
  end = gethrtime();
  time1 = (float) (end - start)/1000000/ntime;

  if (verbose) {
    printf("    CINT-GL Time = %f msec\n", time1);
  }
/****************measure fp unit fir************************/
  start = gethrtime();
  for (j = 0; j < ntime; j ++) {
    vdk_cfloat_ptolemyfir(src1, dst1, length, filtertaps_d64, taplength);
  }
  end = gethrtime();
  time2 = (float) (end - start)/1000000/ntime;

  if (verbose) {
    printf("    CFLOAT-GL Time = %f msec\n", time2);
  }
/***************measure vis unit fir*****************************/
  /*adjust the src*/
  for(i=0;i<length/4;i++){
   srcptolemy[NPACK*i+3]=src0[i*NPACK];
   srcptolemy[NPACK*i+2]=src0[i*NPACK+1];
   srcptolemy[NPACK*i+1]=src0[i*NPACK+2];
   srcptolemy[NPACK*i]=src0[i*NPACK+3];
  }
  /* determine padded tap buffer size */
  if (taplength == 0){
    maxpast = 0;
  }
  else if ((taplength-1)%NPACK==0){
    maxpast = (int)(taplength-1)/NPACK+1;
  }
  else{
    maxpast = (int)(taplength-1)/NPACK+2;
  }
  numloop = maxpast;
  tappadlength = NPACK*maxpast;  
  shift_taparray = (vis_s16 *)
    memalign(sizeof(double),sizeof(short)*NPACK*tappadlength);
  /* fill the tap buffer */
  indexcount = shift_taparray;
  for(taprowindex=0;taprowindex<NPACK;taprowindex++){
    for(tapcolindex=0;tapcolindex<tappadlength;tapcolindex++){
      *indexcount++ = 0;
    }
  }
  for(taprowindex=0;taprowindex<NPACK;taprowindex++){
    indexcount = shift_taparray + (tappadlength+1)*taprowindex;
    for(tapcolindex=0;tapcolindex<taplength;tapcolindex++){
      *indexcount++ = (short)(filtertaps[tapcolindex]>>scalefactor);
    }
  }
  vis_write_gsr((scalefactor+1)<<3);	  
	  
  start = gethrtime();
  for (j = 0; j < ntime; j ++) {
    vdk_vis_ptolemyfir((double*)srcptolemy, dst2, length/4,
		       (double*) shift_taparray, numloop);
  }
  end = gethrtime();
  time3 = (float) (end - start)/1000000/ntime;
/***************************************************************/
  if (print) {
    printf("compare outputs\n");
    tmp=(short*)dst2;
    for(i=0;i<length;i++){
      printf("%i %i %f %i\n",i,dst0[i],dst1[i]*SCALE,tmp[i]);
    }
  }
  if (verbose) {
    printf("  VIS-GL Time = %f msec\n", time3);
    printf("  CINT-GL/VIS-GL = %f\n", time1/time3);
    printf("  CFLOAT-GL/VIS-GL = %f\n", time2/time3);
    printf("\n");
  } else {
    printf("%d\t%f\t%f\t%f\t%f\t%f\n", length, time1, time2, time3,
	   time1/time3, time2/time3);
  }

  exit(0); 
}
/***************************************************************/
