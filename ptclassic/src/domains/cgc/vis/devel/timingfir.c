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
#define NTAPS 16

/***************************************************************/

main (int argc, char *argv[])
{
  extern  char *optarg;
  extern  int   optind;
  int           c;
  int      ntime, length;
  int      verbose;
  hrtime_t start, end;
  float    time1, time2;
  double sinarg,twopiover50;
  vis_s16  *dst,*src,*filtertaps;
  int      i,j;

  twopiover50 = 0.0;
  ntime = NTIME;
  length = LENGTH;
  verbose = 0;
  while ((c = getopt(argc, argv, "hvn:l:")) != -1)
    switch (c) {
      case 'h':
        printf("usage: %s [-h] [-v] [-n ntimes] [-l length]\n", argv[0]);
        printf("  Default Setting  \n");
        printf("===================\n");
        printf(" NTIMES = 1000 \n");
        printf(" LENGTH = 8192 \n");
        exit(1);
      case 'v': verbose = 1;
                break;
      case 'n': ntime = atoi(optarg);
                break;
      case 'l': length = atoi(optarg);
                break;
    };
/***************************************************************/
dst = (vis_s16 *) memalign(sizeof(double),sizeof(short)*LENGTH);
src = (vis_s16 *) memalign(sizeof(double),sizeof(short)*(LENGTH+NTAPS-1));
filtertaps = (vis_s16 *) memalign(sizeof(double),sizeof(short)*NTAPS);
/***************************************************************/
  for(i=0;i<LENGTH;i++){
    sinarg = twopiover50;
    twopiover50 += 0.628318530717958;
    dst[i] = (short)32767 * (double) sin(sinarg);
  }
    filtertaps[0]=(short)32767* -4.10485902015429e-05;
    filtertaps[1]=(short)32767*-6.27991234694974e-06;
    filtertaps[2]=(short)32767*-6.48044369959903e-06;
    filtertaps[3]=(short)32767*-6.43306093680702e-06;
    filtertaps[4]=(short)32767*-6.18549614648338e-06;
    filtertaps[5]=(short)32767*-5.65484231686277e-06;
    filtertaps[6]=(short)32767*-4.90690921899934e-06;
    filtertaps[7]=(short)32767*-3.87530545584888e-06;
    filtertaps[8]=(short)32767*-2.64796014539528e-06;
    filtertaps[9]=(short)32767*-1.1672977954277e-06;
    filtertaps[10]=(short)32767*4.65807896753724e-07;
    filtertaps[11]=(short)32767*2.31694868332586e-06;
    filtertaps[12]=(short)32767*4.26108402754801e-06;
    filtertaps[13]=(short)32767*6.34709045098423e-06;
    filtertaps[14]=(short)32767*8.32039368507842e-06;
    filtertaps[15]=(short)32767*1.03524342421933e-05;
/***************************************************************/

  if (verbose) {
    printf("NTIME=%d\n", ntime);
    printf("\n");
    printf("LENGTH=%d\n", length);
    printf("\n");
  }

  start = gethrtime();
  for (j = 0; j < ntime; j ++) {
    vdk_c_fir_16(src, dst, length, filtertaps, NTAPS);
  }
  end = gethrtime();
  time1 = (float) (end - start)/1000000/ntime;

  if (verbose) {
    printf("    C-GL Time = %f msec\n", time1);
  }

  start = gethrtime();
  for (j = 0; j < ntime; j ++) {
    vdk_vis_fir_16(src, dst, length, filtertaps, NTAPS);
  }
  end = gethrtime();
  time2 = (float) (end - start)/1000000/ntime;

  if (verbose) {
    printf("  VIS-GL Time = %f msec\n", time2);
    printf("  C-GL/VIS-GL = %f\n", time1/time2);
    printf("\n");
  } else {
    printf("%d\t%f\t%f\t%f\n", length, time1, time2, time1/time2);
  }

  exit(0); 
}

/***************************************************************/
