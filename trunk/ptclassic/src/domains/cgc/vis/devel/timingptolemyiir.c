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
#define LENGTH 8192
#define NPACK 4
#define SCALETAPS 32767
#define SCALEDATA 32767/8
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
  vis_d64  *dst1,*src1,numtaps[2],dentaps[3],*taps1,taps2;
  vis_d64  t0,t1,t2,t3;
  int      i,j;
  int	   scalefactor;
  short    scaledown;
  vis_s16  *dst0,*dst2,*src0,n0,*taps0;
  short    *tmp;

  twopiover50 = 0.0;
  ntime = NTIME;
  length = LENGTH;
  verbose = 0;
  print = 0;
  while ((c = getopt(argc, argv, "hvpn:l:")) != -1)
    switch (c) {
    case 'h':
      printf("usage: %s [-h] [-v] [-p] [-n ntimes] [-l length]\n", argv[0]);
      printf("  Default Setting  \n");
      printf("===================\n");
      printf(" NTIMES = 1000 \n");
      printf(" LENGTH = 8192 \n");
      exit(1);
    case 'v': verbose = 1;
      break;
    case 'p': print = 1;
      break;
    case 'n': ntime = atoi(optarg);
      break;
    case 'l': length = atoi(optarg);
      break;
    };
    scalefactor = 1;
/**************allocate memory************************************/
dst0 = (vis_s16 *) memalign(sizeof(double),sizeof(short)*length);
dst1 = (vis_d64 *) memalign(sizeof(double),sizeof(double)*length);
dst2 = (vis_s16 *) memalign(sizeof(double),sizeof(short)*length);
src0 = (vis_s16 *) memalign(sizeof(double),sizeof(short)*length);
src1 = (vis_d64 *) memalign(sizeof(double),sizeof(double)*length);
taps0 = (vis_s16 *) memalign(sizeof(double),sizeof(short)*5);
taps1 = (vis_d64 *) memalign(sizeof(double),sizeof(double)*5);
/**************setup input***********************************/
  for(i=0;i<length;i++){
    sinarg = twopiover50;
    twopiover50 += 0.628318530717958;
    src0[i] = (short)SCALEDATA * (double) sin(sinarg);
    src1[i] = sin(sinarg);
  }
  numtaps[0] = 0.067455;
  numtaps[1] = 0.135;
  numtaps[2] = 0.067455;
  dentaps[0] = -1.143;
  dentaps[1] = 0.4128;

  scaledown = (short) 1 << scalefactor;
  taps0[0] = (short) SCALETAPS/scaledown*dentaps[0];
  taps0[1] = (short) SCALETAPS/scaledown*dentaps[1];
  taps0[2] = (short) SCALETAPS/scaledown*numtaps[0];
  taps0[3] = (short) SCALETAPS/scaledown*numtaps[1];
  taps0[4] = (short) SCALETAPS/scaledown*numtaps[2];
  printf("%i %i %i %i\n",taps0[0],taps0[3],taps0[1],taps0[4]);

  taps1[0] = dentaps[0];
  taps1[1] = dentaps[1];
  taps1[2] = numtaps[0];
  taps1[3] = numtaps[1];
  taps1[4] = numtaps[2];
 
  vis_alignaddr(0,6);
  tmp = (short *) taps0;
  taps2 = vis_fzero();
  n0 = taps0[2];
  t0 = vis_ld_u16(tmp);
  t1 = vis_ld_u16(tmp+3);
  t2 = vis_ld_u16(tmp+1);
  t3 = vis_ld_u16(tmp+4);
  taps2 = vis_faligndata(t3,taps2);
  taps2 = vis_faligndata(t2,taps2);
  taps2 = vis_faligndata(t1,taps2);
  taps2 = vis_faligndata(t0,taps2);
/***************************************************************/
  if (verbose) {
    printf("NTIME=%d\n", ntime);
    printf("\n");
    printf("LENGTH=%d\n", length);
    printf("\n");
  }
/****************measure integer unit iir************************/
  start = gethrtime();
  for (j = 0; j < ntime; j ++) {
    vdk_cint_ptolemyiir(src0, dst0, length, taps0, scalefactor);
  }
  end = gethrtime();
  time1 = (float) (end - start)/1000000/ntime;

  if (verbose) {
    printf("    CINT-GL Time = %f msec\n", time1);
  }
/****************measure fp unit iir************************/
  start = gethrtime();
  for (j = 0; j < ntime; j ++) {
    vdk_cfloat_ptolemyiir(src1, dst1, length, taps1);
  }
  end = gethrtime();
  time2 = (float) (end - start)/1000000/ntime;

  if (verbose) {
    printf("    CFLOAT-GL Time = %f msec\n", time2);
  }
/***************measure vis unit iir*****************************/
  start = gethrtime();
  for (j = 0; j < ntime; j ++) {
    vdk_vis_ptolemyiir(src0, dst2, length, n0, taps2, scalefactor);
  }
  end = gethrtime();
  time3 = (float) (end - start)/1000000/ntime;
/***************************************************************/
  if (print) {
    printf("compare outputs\n");
    for(i=0;i<length;i++){
      printf("%i %i %f %i\n",i,dst0[i],dst1[i]*SCALEDATA,dst2[i]);
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
