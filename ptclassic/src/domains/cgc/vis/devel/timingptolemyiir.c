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
#define SCALEDATA 32767/16
/***************setup program arguments**************************/
main (int argc, char *argv[])
{
  extern  char *optarg;
  extern  int   optind;
  int           c;
  int      ntime, length;
  int      verbose,print;
  hrtime_t start, end;
  float    time1,time2,time3,time4;
  double sinarg,twopiover50;
  vis_d64  *dst1,*dst2,*src1;
  vis_d64  numtaps[2],dentaps[3],*taps1,taps2;
  vis_d64  t0,t1,t2,t3,quadn0;
  vis_d64  alphatop,alphabott,beta;
  vis_f32  *dst3;
  vis_u32  dbln0;
  int      i,j,scalefactor;
  short    scaledown;
  vis_s16  *dst0,*src0,*src2,n0,*taps0,*taps3,*splitsh,*splitshagain;

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
dst2 = (vis_d64 *) memalign(sizeof(double),sizeof(double)*length/4);
dst3 = (vis_f32 *) memalign(sizeof(double),sizeof(float)*length/2);
src0 = (vis_s16 *) memalign(sizeof(double),sizeof(short)*length);
src1 = (vis_d64 *) memalign(sizeof(double),sizeof(double)*length);
src2 = (vis_s16 *) memalign(sizeof(double),sizeof(short)*(length+2));
taps0 = (vis_s16 *) memalign(sizeof(double),sizeof(short)*5);
taps1 = (vis_d64 *) memalign(sizeof(double),sizeof(double)*5);
taps3 = (vis_s16 *) memalign(sizeof(double),sizeof(short)*12);
/**************setup input***********************************/
  src2[0]=src2[1]=0;
  for(i=0;i<length;i++){
    sinarg = twopiover50;
    twopiover50 += 0.628318530717958;
    src0[i] = src2[i+2] = (short)SCALEDATA * (double) sin(sinarg);
    src1[i] = sin(sinarg);
  }
/**************setup taps***********************************/
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

  taps1[0] = dentaps[0];
  taps1[1] = dentaps[1];
  taps1[2] = numtaps[0];
  taps1[3] = numtaps[1];
  taps1[4] = numtaps[2];
 
  vis_alignaddr(0,6);
  splitsh = (short *) taps0;
  taps2 = vis_fzero();
  n0 = taps0[2];
  t0 = vis_ld_u16(splitsh);
  t1 = vis_ld_u16(splitsh+3);
  t2 = vis_ld_u16(splitsh+1);
  t3 = vis_ld_u16(splitsh+4);
  taps2 = vis_faligndata(t3,taps2);
  taps2 = vis_faligndata(t2,taps2);
  taps2 = vis_faligndata(t1,taps2);
  taps2 = vis_faligndata(t0,taps2);

  dbln0 = (n0<<16)|(n0&0xffff);
  quadn0 = vis_to_double_dup(dbln0);

  taps3[0] = (short)SCALETAPS/scaledown*(-taps1[1]);
  taps3[1] = (short)SCALETAPS/scaledown*(-taps1[0]);
  taps3[2] = (short)SCALETAPS/scaledown*taps1[4];
  taps3[3] = (short)SCALETAPS/scaledown*taps1[3];
  taps3[4] = (short)SCALETAPS/scaledown*taps1[2];
  taps3[5] = 0;
  taps3[6] = (short)SCALETAPS/scaledown*(taps1[0]*taps1[1]);
  taps3[7] = (short)SCALETAPS/scaledown*((taps1[0]*taps1[0])-taps1[1]);
  taps3[8] = (short)SCALETAPS/scaledown*(-(taps1[4]*taps1[0]));
  taps3[9] = (short)SCALETAPS/scaledown*(taps1[4]-(taps1[3]*taps1[0]));
  taps3[10] = (short)SCALETAPS/scaledown*(taps1[3]-(taps1[2]*taps1[0]));
  taps3[11] = (short)SCALETAPS/scaledown*taps1[2];


  alphatop = vis_fzero();
  alphabott = vis_fzero();
  beta = vis_fzero();
  splitsh = (short *) taps3;
  t0 = vis_ld_u16(splitsh+2);
  t1 = vis_ld_u16(splitsh+3);
  t2 = vis_ld_u16(splitsh+4);
  t3 = vis_ld_u16(splitsh+5);
  alphatop = vis_faligndata(t3,alphatop);
  alphatop = vis_faligndata(t2,alphatop);
  alphatop = vis_faligndata(t1,alphatop);
  alphatop = vis_faligndata(t0,alphatop);
  t0 = vis_ld_u16(splitsh+8);
  t1 = vis_ld_u16(splitsh+9);
  t2 = vis_ld_u16(splitsh+10);
  t3 = vis_ld_u16(splitsh+11);
  alphabott = vis_faligndata(t3,alphabott);
  alphabott = vis_faligndata(t2,alphabott);
  alphabott = vis_faligndata(t1,alphabott);
  alphabott = vis_faligndata(t0,alphabott);
  t0 = vis_ld_u16(splitsh+0);
  t1 = vis_ld_u16(splitsh+1);
  t2 = vis_ld_u16(splitsh+6);
  t3 = vis_ld_u16(splitsh+7);
  beta = vis_faligndata(t3,beta);
  beta = vis_faligndata(t2,beta);
  beta = vis_faligndata(t1,beta);
  beta = vis_faligndata(t0,beta);
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
    vdk_vis_ptolemyiir((double*)src0, dst2, length/4, quadn0, taps2,
		       scalefactor);
  }
  end = gethrtime();
  time3 = (float) (end - start)/1000000/ntime;
  if (verbose) {
    printf("  VIS-GL Time = %f msec\n", time3);
  }
/***************measure vis unit/retiming iir*********************/
  start = gethrtime();
  for (j = 0; j < ntime; j ++) {
    vdk_visretimeby2_ptolemyiir((float*)src2, dst3, length/2, alphatop,
			     alphabott, beta, scalefactor);
  }
  end = gethrtime();
  time4 = (float) (end - start)/1000000/ntime;
  if (verbose) {
    printf("  VISRETIME-GL Time = %f msec\n", time4);
  }
/***************************************************************/
  if (print) {
    printf("compare outputs\n");
    splitsh=(short*)dst2;
    splitshagain=(short*)dst3;
    for(i=0;i<length;i++){
      printf("%i %i %f %i %i\n",i,dst0[i],dst1[i]*SCALEDATA,splitsh[i]<<1,splitshagain[i]);
    }
  }
  if (verbose) {
    printf("  CINT-GL/VIS-GL = %f\n", time1/time3);
    printf("  CFLOAT-GL/VIS-GL = %f\n", time2/time3);
    printf("  CFLOAT-GL/VISRETIME-GL = %f\n", time2/time4);
    printf("\n");
  } else {
    printf("%d\t%f\t%f\t%f\t%f\t%f\t%f\n", length, time1, time2, time3,
	   time1/time3, time2/time3, time2/time4);
  }
  exit(0); 
}
/***************************************************************/
