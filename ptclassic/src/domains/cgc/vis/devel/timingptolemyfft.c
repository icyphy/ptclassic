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
#include "vis_proto.h"

#define NTIME 1000
#define LENGTH 256
#define NPACK 4
#define SCALE 32767/64
/***************setup program arguments**************************/
main (int argc, char *argv[])
{
  extern  char *optarg;
  extern  int   optind;
  int           c;
  int      ntime, length;
  int      verbose,print;
  hrtime_t start, end;
  float    time1, time2;
  double sinarg,twopiover50,twopioverN2;
  vis_d64  *src0re,*src0im,*cosfarray,*sinfarray,*indexcountf0,*indexcountf1;
  int      i,j,N2;
  vis_s16  *src1re,*src1im,*cossarray,*sinsarray,*indexcounts0,*indexcounts1;

  ntime = NTIME;
  length = LENGTH;
  verbose = 0;
  print = 0;
  while ((c = getopt(argc, argv, "hvpn:l:")) != -1)
    switch (c) {
    case 'h':
      printf("usage: %s [-h] [-v] [-p] [-n ntimes]\n", argv[0]);
      printf("  Default Setting  \n");
      printf("===================\n");
      printf(" NTIMES = 1000 \n");
      printf(" LENGTH = 256 \n");
      exit(1);
    case 'v': verbose = 1;
      break;
    case 'p': print = 1;
      break;
    case 'n': ntime = atoi(optarg);
      break;
    };
/**************allocate memory************************************/
src0re = (vis_d64 *) memalign(sizeof(double),sizeof(double)*length);
src0im = (vis_d64 *) memalign(sizeof(double),sizeof(double)*length);
src1re = (vis_s16 *) memalign(sizeof(double),sizeof(short)*length);
src1im = (vis_s16 *) memalign(sizeof(double),sizeof(short)*length);
cosfarray = (vis_d64 *) memalign(sizeof(double),sizeof(double)*255);
sinfarray = (vis_d64 *) memalign(sizeof(double),sizeof(double)*255);
cossarray = (vis_s16 *) memalign(sizeof(double),sizeof(short)*252);
sinsarray = (vis_s16 *) memalign(sizeof(double),sizeof(short)*252);
/**************setup input***********************************/
  twopiover50 = 0.0;
  for(i=0;i<length;i++){
    sinarg = twopiover50;
    twopiover50 += 0.628318530717958;
    src0re[i] = sin(sinarg);
    src1re[i] = (short)SCALE * (double) sin(sinarg);
  }
  twopiover50 = 0.0;
  for(i=0;i<length;i++){
    sinarg = twopiover50;
    twopiover50 += 0.628318530717958;
    src0im[i] = cos(sinarg);
    src1im[i] = (short)SCALE * (double) cos(sinarg);
  }
  vis_write_gsr(8);
/**************setup twiddle***********************************/
  N2=256;
  indexcountf0=cosfarray;
  indexcountf1=sinfarray;
  for(i=0;i<8;i++){
    twopioverN2 = 6.28318/N2;
    N2 = N2/2;
    for(j=0;j<N2;j++){
      *indexcountf0++=cos(twopioverN2*j);
      *indexcountf1++=-sin(twopioverN2*j);
    }
  }
  N2=256;
  indexcounts0=cossarray;
  indexcounts1=sinsarray;
  for(i=0;i<6;i++){
    twopioverN2 = 6.28318/N2;
    N2 = N2/2;
    for(j=0;j<N2;j++){
      *indexcounts0++=(short)32767* cos(twopioverN2*j);
      *indexcounts1++=(short)-32767*sin(twopioverN2*j);
    }
  }
/***************************************************************/
  if (verbose) {
    printf("NTIME=%d\n", ntime);
    printf("\n");
    printf("LENGTH=%d\n", length);
    printf("\n");
  }
/****************measure fp unit fft************************/
  start = gethrtime();
  for (j = 0; j < ntime; j ++) {
    vdk_cfloat_ptolemyfft(src0re, src0im, length, cosfarray, sinfarray);
  }
  end = gethrtime();
  time1 = (float) (end - start)/1000000/ntime;

  reorderfloatfft(src0re,src0im,length);

  if (verbose) {
    printf("    CFLOAT-GL Time = %f msec\n", time1);
  }
/***************measure vis unit fft*****************************/
  start = gethrtime();
  for (j = 0; j < ntime; j ++) {
    vdk_vis_ptolemyfft((double*)src1re, (double*)src1im, length,
		       (double*)cossarray, (double*)sinsarray);
  }
  end = gethrtime();
  time2 = (float) (end - start)/1000000/ntime;

  reordershortfft(src1re,src1im,length);

  if (verbose) {
    printf("  VIS-GL Time = %f msec\n", time2);
  }
/***************************************************************/
  if (print) {
    printf("compare outputs\n");
    printf("real\n");
    for(i=0;i<length;i++){
      printf("%i %f %i\n",i,src0re[i]*SCALE,src1re[i]);
    }
    printf("imag\n");
    for(i=0;i<length;i++){
      printf("%i %f %i\n",i,src0im[i]*SCALE,src1im[i]);
    }
  }
  if (verbose) {
    printf("  CFLOAT-GL/VIS-GL = %f\n", time1/time2);
    printf("\n");
  } else {
    printf("%d\t%f\t%f\t%f\n", length, time1, time2, time1/time2);
  }

  exit(0); 
}
/***************************************************************/
