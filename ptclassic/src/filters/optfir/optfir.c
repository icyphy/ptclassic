/*******************************************************************
Version identification:
$Id$

Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.

Permission is hereby granted, without written agreement and without
license or royalty fees, to use, copy, modify, and distribute this
software and its documentation for any purpose, provided that the
above copyright notice and the following two paragraphs appear in all
copies of this software.

IN NO EVENT SHALL THE UNIVERSITY OF CALIFORNIA BE LIABLE TO ANY PARTY
FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES
ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF
THE UNIVERSITY OF CALIFORNIA HAS BEEN ADVISED OF THE POSSIBILITY OF
SUCH DAMAGE.

THE UNIVERSITY OF CALIFORNIA SPECIFICALLY DISCLAIMS ANY WARRANTIES,
INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. THE SOFTWARE
PROVIDED HEREUNDER IS ON AN "AS IS" BASIS, AND THE UNIVERSITY OF
CALIFORNIA HAS NO OBLIGATION TO PROVIDE MAINTENANCE, SUPPORT, UPDATES,
ENHANCEMENTS, OR MODIFICATIONS.

						PT_COPYRIGHT_VERSION_2
						COPYRIGHTENDKEY

Converted from Fortran by f2c, cleaned up by Christopher Hylands
********************************************************************/

#include <stdio.h>
#include <string.h>
#include <math.h>
#include "compat.h"

#define log10e 0.43429448190325182765

/* Common Block Declarations */

struct {
  float des[1600], wt[1600], alpha[100];
  int iext[100], nfcns, ngrid;
  double pi2, ad[100], dev, x[100], y[100];
  float grid[1600];
} _BLNK__;

#define _BLNK__1 _BLNK__

/* Table of constant values */

static int c__1 = 1;
/* Size of strings */
#define STRSIZE 100

/*<       Program Optfir > */
int 
main()
{
  FILE *fout=NULL, *fin=NULL;
  int dontwrite=0;
  /* System generated locals */
  double d__1, d__2;

  /* Local variables */
  static float edge[20], delf;
  static int nodd;
  extern double wate_();
  static float temp;
  static double attn;
  static int nrox;
  static float h[100];
  static int i, j, k, l, lband;
  static char input[STRSIZE];
  static char fname[STRSIZE];
  static int nfmax, nfilt, idist;
  extern /* Subroutine */ remez_();
  static int jtype;
  static double svrip;
  static float change;
  static double pi;
  static float fs, fx[10], deltaf;
  static int ncoeff, nbands, nz;
  static float deviat[10];
  static double ripple;
  static char answer[1];
  static int nm1;
  extern double eff_();
  static int neg;
  static float fup;
  static int kup;
  static float wtx[10];

/* @(#)optfir.f 1.1     11/2/90 */

/*   This program designs optimal equirriple FIR digital filters */

/*<       double precision attn,ripple,svrip > */
/*<       double precision pi2,pi,ad,dev,x,y > */
/*<       character fname*100, answer*1 > */
/*<       common des,wt,alpha,iext,nfcns,ngrid,pi2,ad,dev,x,y,grid > */
/*<       dimension iext(100),ad(100),alpha(100),x(100),y(100),h(100) > */
/*<       dimension des(1600),grid(1600),wt(1600) > */
/*<       dimension edge(20),fx(10),wtx(10),deviat(10) > */
/*<       pi=3.141592653589793 > */
  pi = 3.141592653589793;
/*<       pi2=6.283185307179586 > */
  _BLNK__1.pi2 = 6.283185307179586;
/* Maximum filter length */
/*<       nfmax=256 > */
  nfmax = 256;
  printf("PROVISIONAL EQUIRRIPLE FILTER DESIGN\n");
  printf(" USE AT YOUR OWN RISK ---------------\n");
/*<   100 continue > */
L100:
  printf(" Enter name of input command file(press <Enter> for manual entry,\n");
  printf(" Sorry, no tilde-expansion.  Give path relative to your\n");
  printf(" home or startup directory): ");
  fgets(input, STRSIZE, stdin);
  sscanf(input,"%s",fname);
  if (strlen(fname) == 0)
    fin=stdin;
  else {
    if ((fin = fopen(fname, "r"))==NULL) {
      perror("Can't open file for reading, defaulting to stdin:");
      fin=stdin;
    }
  }
  if (fin == stdin ) {
    printf(" Enter filter type (1=Bandpass, 2=Differentiator,\n");
    printf("                    3=Hilbert transformer, 4=Half-band): ");
  }
  fscanf(fin,"%d", &jtype);
  if (fin == stdin )
    printf(" Enter filter length (enter 0 for estimate): ");
  else
    printf(" Filter type: %d\n",jtype);
  fscanf(fin,"%d", &nfilt);
  if ((nfilt > nfmax) || (nfilt < 3 && nfilt != 0)) {
    goto L515;
  }
  if (fin == stdin )
    printf(" Enter sampling rate of filter: ");
  else
    printf(" Filter length: %d\n", nfilt);
  fscanf(fin,"%f", &fs);
  if (jtype == 4) {
    nfilt = (nfilt + 1) / 2;
    nbands = 1;
    edge[0] = 0.0;
    if (fin == stdin) 
      printf(" Enter passband edge frequency: ");
/*<          read (io,*) edge(2) > */
    fscanf(fin,"%f", &edge[1]);
    if (fin != stdin) 
      printf(" passband edge frequency: %f\n",edge[1]);
    edge[1] = edge[1] * 2.0 / fs;
    fx[0] = 0.5;
    wtx[0] = 1.0;
    goto L118;
  }
  if (fin == stdin )
    printf(" Enter number of filter bands: ");
  fscanf(fin,"%d", &nbands);
  if (nbands <= 0) {
    nbands = 1;
  }
/*<       do 115 i=1,nbands > */
  for (i = 1; i <= nbands; ++i) {
/*<       write (*,'(,a,i3,a)') ' Enter lower band edge for band', i, ': ' > */
    if (fin == stdin)
      printf(" Enter lower band edge for band %d: ", i);
    j = ((i - 1) << 1) + 1;
/*<       read (io,*) edge(j) > */
    fscanf(fin,"%f", &edge[j - 1]);
/*<       edge(j)=edge(j)/fs > */
    edge[j - 1] /= fs;
    if (fin == stdin)
      printf(" Enter upper band edge for band %d: ", i);
/*<       read (io,*) edge(j+1) > */
    fscanf(fin,"%f", &edge[j]);
/*<       edge(j+1)=edge(j+1)/fs > */
    edge[j] /= fs;
/*<       write (*,'(,a,i3,a)') ' Enter desired value for band', i, ': ' > */
    if (fin == stdin) 
      printf(" Enter desired value for band %d: ", i);
/*<       read (io,*) fx(i) > */
    fscanf(fin,"%f", &fx[i - 1]);
/*<       write (*,'(,a,i3,a)') ' Enter weight factor for band', i, ': ' > */
    if (fin == stdin)
      printf(" Enter weight factor for band %d: ", i);
/*<       read (io,*) wtx(i) > */
    fscanf(fin,"%f", &wtx[i - 1]);
    if (fin != stdin) {
      printf(" lower band edge for band %d: %f upper: %f\n",
	     i, edge[j-1], edge[j]);
      printf("\tdesired: %f weight: %f\n", fx[i-1], wtx[i-1]);
    }
/*<   115 continue > */
/* L115: */
  }
  if (fin == stdin)
    printf(" Do you want x/sin(x) predistortion? (y/n): ");
/*<       read (io,'(a1)') answer > */
  fscanf(fin,"%s", answer);
/*<       if (answer .eq. 'y' .or. answer .eq. 'Y') idist=1 > */
  if (*answer == 'y' || *answer == 'Y') {
    idist = 1;
  }
/*<   118 write (*,'(,a)') ' Enter name of coefficient output file ' > */
L118:
  fname[0]='\0';
  if (fin == stdin) {
    printf(" Enter name of coefficient output file ");
    printf(" (Sorry, no tilde-expansion.  Give path relative to your home directory): ");
  }
  fscanf(fin,"%s", fname);
/*<       open (1, file=fname) > */
  if (fin != stdin)
    printf("coefficient output file: %s\n",fname);
  if ((fout=fopen(fname,"w")) == NULL) {
    perror("Could not open output file:");
    dontwrite=1;
  }
/*<       if(nfilt.eq.0) go to 120 > */
  if (nfilt == 0) {
    goto L120;
  }
/*<       nrox=0 > */
  nrox = 0;
/*<       go to 130 > */
  goto L130;
/*<   120 continue > */
L120:
  printf(" Enter desired passband ripple in dB: ");
  fscanf(fin,"%lg", &ripple);
  printf(" Enter desired stopband attenuation in dB: ");
  fscanf(fin,"%lg", &attn);
/*<       ripple=10.**(ripple/20.) > */
  ripple = pow(10.0, ripple / 20.0);
/*<       attn=10.**(-attn/20.) > */
  attn = pow(10.0, -attn / 20.0);
/*<       ripple=(ripple-1)/(ripple+1) > */
  ripple = (ripple - 1) / (ripple + 1);
/*<       ripple=dlog10(ripple) > */
  ripple = log10e * log(ripple);
/*<       attn=dlog10(attn) > */
  attn = log10e *log(attn);
/*<       svrip=ripple > */
  svrip = ripple;
/*<    > */
/* Computing 2nd power */
  d__1 = ripple;
/* Computing 2nd power */
  d__2 = ripple;
  ripple = (d__1 * d__1 * .005309 + ripple * .07114 - .4761) * attn - (
			  d__2 * d__2 * .00266 + ripple * .5941 + .4278);
/*<       attn=11.01217+0.51244*(svrip-attn) > */
  attn = (svrip - attn) * .51244 + 11.01217;
/*<       deltaf=edge(3)-edge(2) > */
  deltaf = edge[2] - edge[1];
/*<       nfilt=ripple/deltaf-attn*deltaf+1 > */
  nfilt = (int) (ripple / deltaf - attn * deltaf + 1);
/*<       write (*,'(/,a,i4)') ' Estimated filter length = ', nfilt > */
  printf(" Estimated filter length = %d", nfilt);
/*<       if(nfilt.gt.nfmax.or.(nfilt.lt.3.and.nfilt.ne.0)) go to 515 > */
  if ((nfilt > nfmax) || (nfilt < 3 && nfilt != 0)) {
    goto L515;
  }
/*<       nrox=1 > */
  nrox = 1;
/*<   130 neg=1 > */
L130:
  neg = 1;
/*<       write (*,'(/,a,/)') ' Executing ...' > */
  printf(" Executing ...\n\n\n");
/*<       if(jtype .eq. 1 .or. jtype .eq. 4) neg = 0 > */
  if (jtype == 1 || jtype == 4) {
    neg = 0;
  }
/*<       nodd=nfilt/2 > */
  nodd = nfilt / 2;
/*<       nodd=nfilt-2*nodd > */
  nodd = nfilt - (nodd << 1);
/*<       nfcns=nfilt/2 > */
  _BLNK__1.nfcns = nfilt / 2;
/*<       if(nodd.eq.1.and.neg.eq.0) nfcns=nfcns+1 > */
  if (nodd == 1 && neg == 0) {
    ++_BLNK__1.nfcns;
  }
/*<       grid(1)=edge(1) > */
  _BLNK__1.grid[0] = edge[0];
/*<       delf=16*nfcns > */
  delf = (float) (_BLNK__1.nfcns << 4);
/*<       delf=0.5/delf > */
  delf = .5 / delf;
/*<       if(neg.eq.0) go to 135 > */
  if (neg == 0) {
    goto L135;
  }
/*<       if(edge(1).lt.delf)grid(1)=delf > */
  if (edge[0] < delf) {
    _BLNK__1.grid[0] = delf;
  }
/*<   135 continue > */
L135:
/*<       j=1 > */
  j = 1;
/*<       l=1 > */
  l = 1;
/*<       lband=1 > */
  lband = 1;
/*<   140 fup=edge(l+1) > */
L140:
  fup = edge[l];
/*<   145 temp=grid(j) > */
L145:
  temp = _BLNK__1.grid[j - 1];
/*<       des(j)=eff(temp,fx,lband,jtype,idist) > */
  _BLNK__1.des[j - 1] = eff_(&temp, fx, &lband, &jtype, &idist);
/*<       wt(j)=wate(temp,fx,wtx,lband,jtype) > */
  _BLNK__1.wt[j - 1] = wate_(&temp, fx, wtx, &lband, &jtype);
/*<       j=j+1 > */
  ++j;
/*<       grid(j)=temp+delf > */
  _BLNK__1.grid[j - 1] = temp + delf;
/*<       if(grid(j).gt.fup) go to 150 > */
  if (_BLNK__1.grid[j - 1] > fup) {
    goto L150;
  }
/*<       go to 145 > */
  goto L145;
/*<   150 grid(j-1)=fup > */
L150:
  _BLNK__1.grid[j - 2] = fup;
/*<       des(j-1)=eff(fup,fx,lband,jtype,idist) > */
  _BLNK__1.des[j - 2] = eff_(&fup, fx, &lband, &jtype, &idist);
/*<       wt(j-1)=wate(fup,fx,wtx,lband,jtype) > */
  _BLNK__1.wt[j - 2] = wate_(&fup, fx, wtx, &lband, &jtype);
/*<       lband=lband+1 > */
  ++lband;
/*<       l=l+2 > */
  l += 2;
/*<       if(lband.gt.nbands) go to 160 > */
  if (lband > nbands) {
    goto L160;
  }
/*<       grid(j)=edge(l) > */
  _BLNK__1.grid[j - 1] = edge[l - 1];
/*<       go to 140 > */
  goto L140;
/*<   160 ngrid=j-1 > */
L160:
  _BLNK__1.ngrid = j - 1;
/*<       if(neg.ne.nodd) go to 165 > */
  if (neg != nodd) {
    goto L165;
  }
/*<       if(grid(ngrid).gt.(0.5-delf)) ngrid=ngrid-1 > */
  if (_BLNK__1.grid[_BLNK__1.ngrid - 1] > .5 - delf) {
    --_BLNK__1.ngrid;
  }
/*<   165 continue > */
L165:
/*<       if(neg)170,170,180 > */
  if (neg <= 0) {
    goto L170;
  }
  else {
    goto L180;
  }
/*<   170 if(nodd.eq.1) go to 200 > */
L170:
  if (nodd == 1) {
    goto L200;
  }
/*<       do 175 j=1,ngrid > */
  for (j = 1; j <= _BLNK__1.ngrid; ++j) {
/*<       change=dcos(pi*grid(j)) > */
    change = cos(pi * _BLNK__1.grid[j - 1]);
/*<       des(j)=des(j)/change > */
    _BLNK__1.des[j - 1] /= change;
/*<   175 wt(j)=wt(j)*change > */
/* L175: */
    _BLNK__1.wt[j - 1] *= change;
  }
/*<       go to 200 > */
  goto L200;
/*<   180 if(nodd.eq.1) go to 190 > */
L180:
  if (nodd == 1) {
    goto L190;
  }
/*<       do 185 j=1,ngrid > */
  for (j = 1; j <= _BLNK__1.ngrid; ++j) {
/*<       change=dsin(pi*grid(j)) > */
    change = sin(pi * _BLNK__1.grid[j - 1]);
/*<       des(j)=des(j)/change > */
    _BLNK__1.des[j - 1] /= change;
/*<   185 wt(j)=wt(j)*change > */
/* L185: */
    _BLNK__1.wt[j - 1] *= change;
  }
/*<       go to 200 > */
  goto L200;
/*<   190 do 195 j=1,ngrid > */
L190:
  for (j = 1; j <= _BLNK__1.ngrid; ++j) {
/*<       change=dsin(pi2*grid(j)) > */
    change = sin(_BLNK__1.pi2 * _BLNK__1.grid[j - 1]);
/*<       des(j)=des(j)/change > */
    _BLNK__1.des[j - 1] /= change;
/*<   195 wt(j)=wt(j)*change > */
/* L195: */
    _BLNK__1.wt[j - 1] *= change;
  }
/*<   200 temp=float(ngrid-1)/float(nfcns) > */
L200:
  temp = (float) (_BLNK__1.ngrid - 1) / (float) _BLNK__1.nfcns;
/*<       do 210 j=1,nfcns > */
  for (j = 1; j <= _BLNK__1.nfcns; ++j) {
/*<   210 iext(j)=(j-1)*temp+1 > */
/* L210: */
    _BLNK__1.iext[j - 1] = (j - 1) * temp + 1;
  }
/*<       iext(nfcns+1)=ngrid > */
  _BLNK__1.iext[_BLNK__1.nfcns] = _BLNK__1.ngrid;
/*<       nm1=nfcns-1 > */
  nm1 = _BLNK__1.nfcns - 1;
/*<       nz=nfcns+1 > */
  nz = _BLNK__1.nfcns + 1;
/*<       call Remez(edge,nbands) > */
  remez_(edge, &nbands);
/*<       if(neg) 300,300,320 > */
  if (neg <= 0) {
    goto L300;
  }
  else {
    goto L320;
  }
/*<   300 if(nodd.eq.0) go to 310 > */
L300:
  if (nodd == 0) {
    goto L310;
  }
/*<       do 305 j=1,nm1 > */
  for (j = 1; j <= nm1; ++j) {
/*<   305 h(j)=0.5*alpha(nz-j) > */
/* L305: */
    h[j - 1] = _BLNK__1.alpha[nz - j - 1] * .5;
  }
/*<       h(nfcns)=alpha(1) > */
  h[_BLNK__1.nfcns - 1] = _BLNK__1.alpha[0];
/*<       go to 350 > */
  goto L350;
/*<   310 h(1)=0.25*alpha(nfcns) > */
L310:
  h[0] = _BLNK__1.alpha[_BLNK__1.nfcns - 1] * .25;
/*<       do 315 j=2,nm1 > */
  for (j = 2; j <= nm1; ++j) {
/*<   315 h(j)=0.25*(alpha(nz-j)+alpha(nfcns+2-j)) > */
/* L315: */
    h[j - 1] = (_BLNK__1.alpha[nz - j - 1] + _BLNK__1.alpha[
					_BLNK__1.nfcns + 2 - j - 1]) * .25;
  }
/*<       h(nfcns)=0.5*alpha(1)+0.25*alpha(2) > */
  h[_BLNK__1.nfcns - 1] = _BLNK__1.alpha[0] * .5 + _BLNK__1.alpha[1] *
    .25;
/*<       go to 350 > */
  goto L350;
/*<   320 if(nodd.eq.0) go to 330 > */
L320:
  if (nodd == 0) {
    goto L330;
  }
/*<       h(1)=0.25*alpha(nfcns) > */
  h[0] = _BLNK__1.alpha[_BLNK__1.nfcns - 1] * .25;
/*<       h(2)=0.25*alpha(nm1) > */
  h[1] = _BLNK__1.alpha[nm1 - 1] * .25;
/*<       do 325 j=3,nm1 > */
  for (j = 3; j <= nm1; ++j) {
/*<   325 h(j)=0.25*(alpha(nz-j)-alpha(nfcns+3-j)) > */
/* L325: */
    h[j - 1] = (_BLNK__1.alpha[nz - j - 1] - _BLNK__1.alpha[
					_BLNK__1.nfcns + 3 - j - 1]) * .25;
  }
/*<       h(nfcns)=0.5*alpha(1)-0.25*alpha(3) > */
  h[_BLNK__1.nfcns - 1] = _BLNK__1.alpha[0] * .5 - _BLNK__1.alpha[2] *
    .25;
/*<       h(nz)=0.0 > */
  h[nz - 1] = 0.0;
/*<       go to 350 > */
  goto L350;
/*<   330 h(1)=0.25*alpha(nfcns) > */
L330:
  h[0] = _BLNK__1.alpha[_BLNK__1.nfcns - 1] * .25;
/*<       do 335 j=2,nm1 > */
  for (j = 2; j <= nm1; ++j) {
/*<   335 h(j)=0.25*(alpha(nz-j)-alpha(nfcns+2-j)) > */
/* L335: */
    h[j - 1] = (_BLNK__1.alpha[nz - j - 1] - _BLNK__1.alpha[
					_BLNK__1.nfcns + 2 - j - 1]) * .25;
  }
/*<       h(nfcns)=0.5*alpha(1)-0.25*alpha(2) > */
  h[_BLNK__1.nfcns - 1] = _BLNK__1.alpha[0] * .5 - _BLNK__1.alpha[1] *
    .25;
/*<   350 write(6,360) > */
L350:
  printf(" Finite Impulse Response (FIR)\n Linear Phase Digital Filter Design\n Remez Exchange Algorithm\n");
  if (jtype == 1)
    printf("\n Bandpass Filter\n");
  if (jtype == 2)
    printf("\n Differentiator\n");
  if (jtype == 3)
    printf("\n Hilbert Transformer\n");
  if (jtype == 4)
    printf("\n Half-band Filter\n");
  printf("\n Filter length = %d\n\n", nfilt);
  if (nrox == 1)
    printf("Filter length determined by approximation\n");
  printf(" Impulse Response Coefficients:\n\n");
/*<       do 381 j=1,nfcns > */
  for (j = 1; j <= _BLNK__1.nfcns; ++j) {
/*<       k=nfilt+1-j > */
    k = nfilt + 1 - j;
/*<       if(neg.eq.0) write(6,382)j,h(j),k > */
    if (neg == 0)
      printf("         h(%3d) = % f = h(%3d)\n", j, h[j - 1], k);
/*<       if(neg.eq.1) write(6,383)j,h(j),k > */
    if (neg == 1)
      printf("         h(%3d) = % f = -h(%3d)\n", j, h[j - 1], k);
/*<   381 continue > */
/* L381: */
  }
/*<   382 format(10x,'h(',i3,') = ', e14.7,' = h(',i4,')') > */
/*<   383 format(10x,'h(',i3,') = ', e14.7,' = -h(',i4,')') > */
  if (neg == 1 && nodd == 1)
    printf("         h(%3d) = % f\n", nz, 0.0);
/*<   384 format(10x,'h(',i3,') = ', e14.7) > */
/*<       write (*,*) > */
/*      DEBUG */

/*     Pause 'Press <Enter> to continue ...' */
/*<       do 450 k=1,nbands,4 > */
  for (k = 1; k <= nbands; k += 4) {
/*<       kup=k+3 > */
    kup = k + 3;
/*<       if(kup.gt.nbands) kup=nbands > */
    if (kup > nbands) {
      kup = nbands;
    }
/*<       write(6,385)(j,j=k,kup) > */
    printf("                   ");
    for (j = k; j <= kup; ++j) {
      printf("       Band %d ", j);
    }
    printf("\n Lower band edge: ");
/*<   385 format(25x,4('Band',i3,8x)) > */
/*<       write(6,390)(edge(2*j-1),j=k,kup) > */
    for (j = k; j <= kup; ++j)
      printf("     % f", edge[(j << 1) - 2]);
/*<   390 format(/' Lower band edge:',515.7) > */
/*<       write(6,395)(edge(2*j),j=k,kup) > */
    printf("\n Upper band edge: ");
    for (j = k; j <= kup; ++j)
      printf("     % f", edge[(j << 1) - 1]);
/*<   395 format(' Upper band edge:',5f15.7) > */
/*<       if(jtype.ne.2) write(6,400) (fx(j),j=k,kup) > */
    printf("\n Desired value:   ");
    if (jtype != 2) {
      for (j = k; j <= kup; ++j)
	printf("     % f", fx[j - 1]);
    }
/*<   400 format(' Desired value:',2x,5f15.7) > */
/*<       if(jtype.eq.2) write(6,405) (fx(j),j=k,kup) > */
    if (jtype == 2) {
      printf("\n Desired slope:  ");
      for (j = k; j <= kup; ++j)
	printf("     % f", fx[j - 1]);
    }
/*<   405 format(' Desired slope:',2x,5f15.7) > */
/*<       write(6,410) (wtx(j),j=k,kup) > */
    printf("\n Weight factor:   ");
    for (j = k; j <= kup; ++j)
      printf("     % f", wtx[j - 1]);
/*<   410 format(' Weight factor:',2x,5f15.7) > */
/*<       do 420 j=k,kup > */
    for (j = k; j <= kup; ++j) {
/*<   420 deviat(j)=dev/wtx(j) > */
/* L420: */
      deviat[j - 1] = _BLNK__1.dev / wtx[j - 1];
    }
/*<       write(6,425) (deviat(j),j=k,kup) > */
    printf("\n Deviation:       ");
    for (j = k; j <= kup; ++j)
      printf("     % f", deviat[j - 1]);
/*<   425 format(' Deviation:',6x,5f15.7) > */
/*<       do 430 j=k,kup > */
    for (j = k; j <= kup; ++j) {
/*<       if(fx(j).eq.1.0) deviat(j)=(1.0+deviat(j))/(1.0-deviat(j)) > */
      if (fx[j - 1] == 1.0) {
	deviat[j - 1] = (deviat[j - 1] + 1.0) / (1.0 - deviat[j - 1]);
      }
/*<   430 deviat(j)=20.0*alog10(deviat(j)) > */
/* L430: */
      deviat[j - 1] = log(deviat[j - 1]) * log10e * 20;
/*      deviat[j - 1] = r_lg10(&deviat[j - 1]) * 20.0;*/
    }
/*<       write(6,435) (deviat(j),j=k,kup) > */
    printf("\n Deviation in dB: ");
    for (j = k; j <= kup; ++j)
      printf("     % f", deviat[j - 1]);
/*<   435 format(' Deviation in dB:',5f15.7) > */
/*<   450 continue > */
/* L450: */
  }
/*<       write(6,455) (grid(iext(j)),j=1,nz) > */
  printf("\n\n Extremal frequencies:\n\n");
  for (j = 1; j <= nz; ++j)
    printf("  % f%c", _BLNK__1.grid[_BLNK__1.iext[j - 1] - 1],
	   ((j-1)%5==4 || j==nz) ? '\n' : ' ');
    
/*<   455 format(/' Extremal frequencies:'//(2x,5f12.7)) > */
/*<       if (nodd .eq. 1) then > */
  if (nodd == 1) {
/*<          if (neg .eq. 1) nfcns = nfcns + 1 > */
    if (neg == 1) {
      ++_BLNK__1.nfcns;
    }
/*<          ncoeff = 2 * nfcns - 1 > */
    ncoeff = (_BLNK__1.nfcns << 1) - 1;
/*<       else > */
  }
  else {
/*<         ncoeff = 2 * nfcns > */
    ncoeff = _BLNK__1.nfcns << 1;
/*<       endif > */
  }
/*<       if (jtype .eq. 4) then > */
  if (jtype == 4) {

/*   Remove the writing of the filter length, for */
/*   Compatibility with Gabriel */
/*        write(1,500) 2 * ncoeff - 1 */

/*<   500    format(i4) > */
/* L500: */
/*<          do 502 i = 1, ncoeff - 1 > */
    for (i = 1; i <= ncoeff - 1; ++i) {
/*<             if (i .lt. nfcns) then > */
      if (i < _BLNK__1.nfcns) {
/*<                write (1,510) h(i) > */
	printf(" %f", h[i - 1]);
/*<                write (1,510) 0.0 > */
	printf(" %f", 0.0);
/*<             elseif (i .eq. nfcns) then > */
      }
      else if (i == _BLNK__1.nfcns) {
/*<                write (1,510) h(i) > */
	printf(" %f", h[i - 1]);
/*<                write (1,510) 0.5 > */
	printf(" %f", 0.5);
/*<                write (1,510) h(i) > */
	printf(" %f", h[i - 1]);
/*<             else > */
      }
      else {
/*<                write (1,510) 0.0 > */
	printf(" %f", 0.0);
/*<                write (1,510) h(ncoeff-i) > */
	printf(" %f", h[ncoeff - i - 1]);
/*<             endif > */
      }
/*<   502    continue > */
/* L502: */
    }
/*<       else > */
  }
  else {

    if (!dontwrite) {
/*   Remove the writing of the filter length, for */
/*   Compatibility with Gabriel */
/*        write(1,500) ncoeff */

/*<          do 505 i = 1, ncoeff > */
    for (i = 1; i <= ncoeff; ++i) {
/*<             if(i .le. nfcns) write(1,510) h(i) > */
      if (i <= _BLNK__1.nfcns)
	fprintf(fout," %f", h[i - 1]);
/*<             if(i .gt. nfcns) then > */
      if (i > _BLNK__1.nfcns) {
/*<                if(neg .eq. 0) then > */
	if (neg == 0) {
/*<                   write(1,510) h(ncoeff-i+1) > */
	  fprintf(fout, " %f", h[ncoeff - i]);
/*<                else > */
	}
	else {
/*<                   write(1,510) -h(ncoeff-i+1) > */
	  fprintf(fout, " %f", -(double) h[ncoeff - i]);
/*<                endif > */
	}
/*<             endif > */
      }
/*<   505    continue > */
/* L505: */
    }
/*<       endif > */
  }
  }
/*<   510 format(1pe14.6) > */
/*<       write (*,*) > */
/*<       go to 520 > */
  goto L520;

/*   Catch Errors */

/*<   515 print *, '*** FILTER TOO LONG OR SHORT FOR PROGRAM ***' > */
L515:
  printf("*** FILTER TOO LONG OR SHORT FOR PROGRAM ***\n");

/*   Find out whether to terminate program */

/*<   520 call flush(1) > */
L520:
  fflush(stdout);
/*<       close(1) > */
  fclose(fout);
  if (*answer == 'y' || *answer == 'Y') {
    goto L100;
  }
  printf("End of program\n");
  return 0;
}				/* MAIN__ */


/*<       Function Eff(freq,fx,lband,jtype,idist) > */
double 
eff_(freq, fx, lband, jtype, idist)
     float *freq;
     float *fx;
     int *lband;
     int *jtype;
     int *idist;
{
  /* System generated locals */
  float ret_val;

  /* Builtin functions */
  double sin();

  /* Local variables */
  static float x;

/*<       dimension fx(10) > */
/*<       if(jtype.eq.2) go to 1 > */
  /* Parameter adjustments */
  --fx;

  /* Function Body */
  if (*jtype == 2) {
    goto L1;
  }
/*<       eff=fx(lband) > */
  ret_val = fx[*lband];
/*<       if(idist.eq.0) return > */
  if (*idist == 0) {
    return ret_val;
  }
/*<       x=3.141593*freq > */
  x = *freq * 3.141593;
/*<       if(x.ne.0) eff=eff*x/sin(x) > */
  if (x != 0.0) {
    ret_val = ret_val * x / sin(x);
  }
/*<       return > */
  return ret_val;
/*<     1 eff=fx(lband)*freq > */
L1:
  ret_val = fx[*lband] * *freq;
/*<       return > */
  return ret_val;
/*<       end > */
}				/* eff_ */


/*<       Function Wate(freq,fx,wtx,lband,jtype) > */
double 
wate_(freq, fx, wtx, lband, jtype)
     float *freq;
     float *fx;
     float *wtx;
     int *lband;
     int *jtype;
{
  /* System generated locals */
  float ret_val;

/*<       dimension fx(10),wtx(10) > */
/*<       if(jtype.eq.2) go to 1 > */
  /* Parameter adjustments */
  --wtx;
  --fx;

  /* Function Body */
  if (*jtype == 2) {
    goto L1;
  }
/*<       wate=wtx(lband) > */
  ret_val = wtx[*lband];
/*<       return > */
  return ret_val;
/*<     1 if(fx(lband).lt.0.0001) go to 2 > */
L1:
  if (fx[*lband] < 1e-4) {
    goto L2;
  }
/*<       wate=wtx(lband)/freq > */
  ret_val = wtx[*lband] / *freq;
/*<       return > */
  return ret_val;
/*<     2 wate=wtx(lband) > */
L2:
  ret_val = wtx[*lband];
/*<       return > */
  return ret_val;
/*<       end > */
}				/* wate_ */


/*<       Subroutine Remez(edge,nbands) > */
/* Subroutine */ int 
remez_(edge, nbands)
     float *edge;
     int *nbands;
{
  /* Builtin functions */
  double cos(), acos();

  /* Local variables */
  static double dden;
  static float delf, devl;
  extern /* Subroutine */ int ouch_();
  static float comp;
  static int luck;
  static double dnum;
  static int klow;
  static double a[100];
  extern double d_();
  static int j, k, l;
  static double p[100], q[100], dtemp;
  static float gtemp;
  static int niter, k1;
  static float y1, aa, bb, cn;
  static int kn;
  static float ft, xe;
  static int jchnge, nu, nz;
  static float xt;
  static int itrmax, jm1, nm1, jp1;
  extern double gee_();
  static int kid;
  static float fsh;
  static int kkk, jet;
  static float err;
  static int kup, knz, nut;
  static float ynz;
  static int nzz, nut1;

/*<       double precision pi2,dnum,dden,dtemp,a,p,q > */
/*<       double precision d,ad,dev,x,y > */
/*<       common des,wt,alpha,iext,nfcns,ngrid,pi2,ad,dev,x,y,grid > */
/*<       dimension edge(20) > */
/*<       dimension iext(100),ad(100),alpha(100),x(100),y(100) > */
/*<       dimension des(1600),grid(1600),wt(1600) > */
/*<       dimension a(100),p(100),q(100) > */
/*<       itrmax=25 > */
  /* Parameter adjustments */
  --edge;

  /* Function Body */
  itrmax = 25;
/*<       devl=-1.0 > */
  devl = -1.0;
/*<       nz=nfcns+1 > */
  nz = _BLNK__1.nfcns + 1;
/*<       nzz=nfcns+2 > */
  nzz = _BLNK__1.nfcns + 2;
/*<       niter=0 > */
  niter = 0;
/*<   100 continue > */
L100:
/*<       iext(nzz)=ngrid+1 > */
  _BLNK__1.iext[nzz - 1] = _BLNK__1.ngrid + 1;
/*<       niter=niter+1 > */
  ++niter;
/*<       if(niter.gt.itrmax) go to 400 > */
  if (niter > itrmax) {
    goto L400;
  }
/*<       do 110 j=1,nz > */
  for (j = 1; j <= nz; ++j) {
/*<       dtemp=grid(iext(j)) > */
    dtemp = _BLNK__1.grid[_BLNK__1.iext[j - 1] - 1];
/*<       dtemp=dcos(dtemp*pi2) > */
    dtemp = cos(dtemp * _BLNK__1.pi2);
/*<   110 x(j)=dtemp > */
/* L110: */
    _BLNK__1.x[j - 1] = dtemp;
  }
/*<       jet=(nfcns-1)/15+1 > */
  jet = (_BLNK__1.nfcns - 1) / 15 + 1;
/*<       do 120 j=1,nz > */
  for (j = 1; j <= nz; ++j) {
/*<       kid=j > */
    kid = j;
/*<   120 ad(j)=d(kid,nz,jet) > */
/* L120: */
    _BLNK__1.ad[j - 1] = d_(&kid, &nz, &jet);
  }
/*<       dnum=0.0 > */
  dnum = 0.0;
/*<       dden=0.0 > */
  dden = 0.0;
/*<       k=1 > */
  k = 1;
/*<       do 130 j=1,nz > */
  for (j = 1; j <= nz; ++j) {
/*<       l=iext(j) > */
    l = _BLNK__1.iext[j - 1];
/*<       dtemp=ad(j)*des(l) > */
    dtemp = _BLNK__1.ad[j - 1] * _BLNK__1.des[l - 1];
/*<       dnum=dnum+dtemp > */
    dnum += dtemp;
/*<       dtemp=k*ad(j)/wt(l) > */
    dtemp = k * _BLNK__1.ad[j - 1] / _BLNK__1.wt[l - 1];
/*<       dden=dden+dtemp > */
    dden += dtemp;
/*<   130 k=-k > */
/* L130: */
    k = -k;
  }
/*<       dev=dnum/dden > */
  _BLNK__1.dev = dnum / dden;

/*      DEBUG */

/*     write(6,131) dev */
/* 131 format(1x,12hdeviation = ,f12.9) */

/*<       nu=1 > */
  nu = 1;
/*<       if(dev.gt.0.0) nu=-1 > */
  if (_BLNK__1.dev > 0.) {
    nu = -1;
  }
/*<       dev=-nu*dev > */
  _BLNK__1.dev = -nu * _BLNK__1.dev;
/*<       k=nu > */
  k = nu;
/*<       do 140 j=1,nz > */
  for (j = 1; j <= nz; ++j) {
/*<       l=iext(j) > */
    l = _BLNK__1.iext[j - 1];
/*<       dtemp=k*dev/wt(l) > */
    dtemp = k * _BLNK__1.dev / _BLNK__1.wt[l - 1];
/*<       y(j)=des(l)+dtemp > */
    _BLNK__1.y[j - 1] = _BLNK__1.des[l - 1] + dtemp;
/*<   140 k=-k > */
/* L140: */
    k = -k;
  }
/*<       if(dev.ge.devl) go to 150 > */
  if (_BLNK__1.dev >= devl) {
    goto L150;
  }
/*<       call Ouch > */
  ouch_();
/*<       go to 400 > */
  goto L400;
/*<   150 devl=dev > */
L150:
  devl = _BLNK__1.dev;
/*<       jchnge=0 > */
  jchnge = 0;
/*<       k1=iext(1) > */
  k1 = _BLNK__1.iext[0];
/*<       knz=iext(nz) > */
  knz = _BLNK__1.iext[nz - 1];
/*<       klow=0 > */
  klow = 0;
/*<       nut=-nu > */
  nut = -nu;
/*<       j=1 > */
  j = 1;
/*<   200 if(j.eq.nzz) ynz=comp > */
L200:
  if (j == nzz) {
    ynz = comp;
  }
/*<       if(j.ge.nzz) go to 300 > */
  if (j >= nzz) {
    goto L300;
  }
/*<       kup=iext(j+1) > */
  kup = _BLNK__1.iext[j];
/*<       l=iext(j)+1 > */
  l = _BLNK__1.iext[j - 1] + 1;
/*<       nut=-nut > */
  nut = -nut;
/*<       if(j.eq.2) y1=comp > */
  if (j == 2) {
    y1 = comp;
  }
/*<       comp=dev > */
  comp = _BLNK__1.dev;
/*<       if(l.ge.kup) go to 220 > */
  if (l >= kup) {
    goto L220;
  }
/*<       err=gee(l,nz) > */
  err = gee_(&l, &nz);
/*<       err=(err-des(l))*wt(l) > */
  err = (err - _BLNK__1.des[l - 1]) * _BLNK__1.wt[l - 1];
/*<       dtemp=nut*err-comp > */
  dtemp = nut * err - comp;
/*<       if(dtemp.le.0.0) go to 220 > */
  if (dtemp <= 0.) {
    goto L220;
  }
/*<       comp=nut*err > */
  comp = nut * err;
/*<   210 l=l+1 > */
L210:
  ++l;
/*<       if(l.ge.kup) go to 215 > */
  if (l >= kup) {
    goto L215;
  }
/*<       err=gee(l,nz) > */
  err = gee_(&l, &nz);
/*<       err=(err-des(l))*wt(l) > */
  err = (err - _BLNK__1.des[l - 1]) * _BLNK__1.wt[l - 1];
/*<       dtemp=nut*err-comp > */
  dtemp = nut * err - comp;
/*<       if(dtemp.le.0.0) go to 215 > */
  if (dtemp <= 0.) {
    goto L215;
  }
/*<       comp=nut*err > */
  comp = nut * err;
/*<       go to 210 > */
  goto L210;
/*<   215 iext(j)=l-1 > */
L215:
  _BLNK__1.iext[j - 1] = l - 1;
/*<       j=j+1 > */
  ++j;
/*<       klow=l-1 > */
  klow = l - 1;
/*<       jchnge=jchnge+1 > */
  ++jchnge;
/*<       go to 200 > */
  goto L200;
/*<   220 l=l-1 > */
L220:
  --l;
/*<   225 l=l-1 > */
L225:
  --l;
/*<       if(l.le.klow) go to 250 > */
  if (l <= klow) {
    goto L250;
  }
/*<       err=gee(l,nz) > */
  err = gee_(&l, &nz);
/*<       err=(err-des(l))*wt(l) > */
  err = (err - _BLNK__1.des[l - 1]) * _BLNK__1.wt[l - 1];
/*<       dtemp=nut*err-comp > */
  dtemp = nut * err - comp;
/*<       if(dtemp.gt.0.0) go to 230 > */
  if (dtemp > 0.) {
    goto L230;
  }
/*<       if(jchnge.le.0) go to 225 > */
  if (jchnge <= 0) {
    goto L225;
  }
/*<       go to 260 > */
  goto L260;
/*<   230 comp=nut*err > */
L230:
  comp = nut * err;
/*<   235 l=l-1 > */
L235:
  --l;
/*<       if(l.le.klow) go to 240 > */
  if (l <= klow) {
    goto L240;
  }
/*<       err=gee(l,nz) > */
  err = gee_(&l, &nz);
/*<       err=(err-des(l))*wt(l) > */
  err = (err - _BLNK__1.des[l - 1]) * _BLNK__1.wt[l - 1];
/*<       dtemp=nut*err-comp > */
  dtemp = nut * err - comp;
/*<       if(dtemp.le.0.0) go to 240 > */
  if (dtemp <= 0.) {
    goto L240;
  }
/*<       comp=nut*err > */
  comp = nut * err;
/*<       go to 235 > */
  goto L235;
/*<   240 klow=iext(j) > */
L240:
  klow = _BLNK__1.iext[j - 1];
/*<       iext(j)=l+1 > */
  _BLNK__1.iext[j - 1] = l + 1;
/*<       j=j+1 > */
  ++j;
/*<       jchnge=jchnge+1 > */
  ++jchnge;
/*<       go to 200 > */
  goto L200;
/*<   250 l=iext(j)+1 > */
L250:
  l = _BLNK__1.iext[j - 1] + 1;
/*<       if(jchnge.ge.0) go to 215 > */
  if (jchnge >= 0) {
    goto L215;
  }
/*<   255 l=l+1 > */
L255:
  ++l;
/*<       if(l.ge.kup) go to 260 > */
  if (l >= kup) {
    goto L260;
  }
/*<       err=gee(l,nz) > */
  err = gee_(&l, &nz);
/*<       err=(err-des(l))*wt(l) > */
  err = (err - _BLNK__1.des[l - 1]) * _BLNK__1.wt[l - 1];
/*<       dtemp=nut*err-comp > */
  dtemp = nut * err - comp;
/*<       if(dtemp.le.0.0) go to 255 > */
  if (dtemp <= 0.) {
    goto L255;
  }
/*<       comp=nut*err > */
  comp = nut * err;
/*<       go to 210 > */
  goto L210;
/*<   260 klow=iext(j) > */
L260:
  klow = _BLNK__1.iext[j - 1];
/*<       j=j+1 > */
  ++j;
/*<       go to 200 > */
  goto L200;
/*<   300 if(j.gt.nzz) go to 320 > */
L300:
  if (j > nzz) {
    goto L320;
  }
/*<       if(k1.gt.iext(1)) k1=iext(1) > */
  if (k1 > _BLNK__1.iext[0]) {
    k1 = _BLNK__1.iext[0];
  }
/*<       if(knz.lt.iext(nz)) knz=iext(nz) > */
  if (knz < _BLNK__1.iext[nz - 1]) {
    knz = _BLNK__1.iext[nz - 1];
  }
/*<       nut1=nut > */
  nut1 = nut;
/*<       nut=-nu > */
  nut = -nu;
/*<       l=0 > */
  l = 0;
/*<       kup=k1 > */
  kup = k1;
/*<       comp=ynz*(1.00001) > */
  comp = ynz * 1.00001;
/*<       luck=1 > */
  luck = 1;
/*<   310 l=l+1 > */
L310:
  ++l;
/*<       if(l.ge.kup) go to 315 > */
  if (l >= kup) {
    goto L315;
  }
/*<       err=gee(l,nz) > */
  err = gee_(&l, &nz);
/*<       err=(err-des(l))*wt(l) > */
  err = (err - _BLNK__1.des[l - 1]) * _BLNK__1.wt[l - 1];
/*<       dtemp=nut*err-comp > */
  dtemp = nut * err - comp;
/*<       if(dtemp.le.0.0) go to 310 > */
  if (dtemp <= 0.) {
    goto L310;
  }
/*<       comp=nut*err > */
  comp = nut * err;
/*<       j=nzz > */
  j = nzz;
/*<       go to 210 > */
  goto L210;
/*<   315 luck=6 > */
L315:
  luck = 6;
/*<       go to 325 > */
  goto L325;
/*<   320 if(luck.gt.9) go to 350 > */
L320:
  if (luck > 9) {
    goto L350;
  }
/*<       if(comp.gt.y1) y1=comp > */
  if (comp > y1) {
    y1 = comp;
  }
/*<       k1=iext(nzz) > */
  k1 = _BLNK__1.iext[nzz - 1];
/*<   325 l=ngrid+1 > */
L325:
  l = _BLNK__1.ngrid + 1;
/*<       klow=knz > */
  klow = knz;
/*<       nut=-nut1 > */
  nut = -nut1;
/*<       comp=y1*(1.00001) > */
  comp = y1 * 1.00001;
/*<   330 l=l-1 > */
L330:
  --l;
/*<       if(l.le.klow) go to 340 > */
  if (l <= klow) {
    goto L340;
  }
/*<       err=gee(l,nz) > */
  err = gee_(&l, &nz);
/*<       err=(err-des(l))*wt(l) > */
  err = (err - _BLNK__1.des[l - 1]) * _BLNK__1.wt[l - 1];
/*<       dtemp=nut*err-comp > */
  dtemp = nut * err - comp;
/*<       if(dtemp.le.0.0) go to 330 > */
  if (dtemp <= 0.) {
    goto L330;
  }
/*<       j=nzz > */
  j = nzz;
/*<       comp=nut*err > */
  comp = nut * err;
/*<       luck=luck+10 > */
  luck += 10;
/*<       go to 235 > */
  goto L235;
/*<   340 if(luck.eq.6) go to 370 > */
L340:
  if (luck == 6) {
    goto L370;
  }
/*<       do 345 j=1,nfcns > */
  for (j = 1; j <= _BLNK__1.nfcns; ++j) {
/*<   345 iext(nzz-j)=iext(nz-j) > */
/* L345: */
    _BLNK__1.iext[nzz - j - 1] = _BLNK__1.iext[nz - j - 1];
  }
/*<       iext(1)=k1 > */
  _BLNK__1.iext[0] = k1;
/*<       go to 100 > */
  goto L100;
/*<   350 kn=iext(nzz) > */
L350:
  kn = _BLNK__1.iext[nzz - 1];
/*<       do 360 j=1,nfcns > */
  for (j = 1; j <= _BLNK__1.nfcns; ++j) {
/*<   360 iext(j)=iext(j+1) > */
/* L360: */
    _BLNK__1.iext[j - 1] = _BLNK__1.iext[j];
  }
/*<       iext(nz)=kn > */
  _BLNK__1.iext[nz - 1] = kn;
/*<       go to 100 > */
  goto L100;
/*<   370 if(jchnge.gt.0) go to 100 > */
L370:
  if (jchnge > 0) {
    goto L100;
  }
/*<   400 continue > */
L400:
/*<       nm1=nfcns-1 > */
  nm1 = _BLNK__1.nfcns - 1;
/*<       fsh=1.0e-06 > */
  fsh = 1e-6;
/*<       gtemp=grid(1) > */
  gtemp = _BLNK__1.grid[0];
/*<       x(nzz)=-2.0 > */
  _BLNK__1.x[nzz - 1] = -2.0;
/*<       cn=2.*nfcns-1. > */
  cn = _BLNK__1.nfcns * 2.0 - 1.0;
/*<       delf=1.0/cn > */
  delf = 1.0 / cn;
/*<       l=1 > */
  l = 1;
/*<       kkk=0 > */
  kkk = 0;
/*<       if(edge(1).eq.0.0.and.edge(2*nbands).eq.0.5) kkk=1 > */
  if (edge[1] == 0.0 && edge[*nbands * 2] == .5) {
    kkk = 1;
  }
/*<       if(nfcns.le.3) kkk=1 > */
  if (_BLNK__1.nfcns <= 3) {
    kkk = 1;
  }
/*<       if(kkk.eq.1) go to 405 > */
  if (kkk == 1) {
    goto L405;
  }
/*<       dtemp=dcos(pi2*grid(1)) > */
  dtemp = cos(_BLNK__1.pi2 * _BLNK__1.grid[0]);
/*<       dnum=dcos(pi2*grid(ngrid)) > */
  dnum = cos(_BLNK__1.pi2 * _BLNK__1.grid[_BLNK__1.ngrid - 1]);
/*<       aa=2.0/(dtemp-dnum) > */
  aa = 2.0 / (dtemp - dnum);
/*<       bb=-(dtemp+dnum)/(dtemp-dnum) > */
  bb = -(dtemp + dnum) / (dtemp - dnum);
/*<   405 continue > */
L405:
/*<       do 430 j=1,nfcns > */
  for (j = 1; j <= _BLNK__1.nfcns; ++j) {
/*<       ft=(j-1)*delf > */
    ft = (j - 1) * delf;
/*<       xt=dcos(pi2*ft) > */
    xt = cos(_BLNK__1.pi2 * ft);
/*<       if(kkk.eq.1) go to 410 > */
    if (kkk == 1) {
      goto L410;
    }
/*<       xt=(xt-bb)/aa > */
    xt = (xt - bb) / aa;
/*<       ft=acos(xt)/pi2 > */
    ft = acos(xt) / _BLNK__1.pi2;
/*<   410 xe=x(l) > */
  L410:
    xe = _BLNK__1.x[l - 1];
/*<       if(xt.gt.xe) go to 420 > */
    if (xt > xe) {
      goto L420;
    }
/*<       if((xe-xt).lt.fsh) go to 415 > */
    if (xe - xt < fsh) {
      goto L415;
    }
/*<       l=l+1 > */
    ++l;
/*<       go to 410 > */
    goto L410;
/*<   415 a(j)=y(l) > */
  L415:
    a[j - 1] = _BLNK__1.y[l - 1];
/*<       go to 425 > */
    goto L425;
/*<   420 if((xt-xe).lt.fsh) go to 415 > */
  L420:
    if (xt - xe < fsh) {
      goto L415;
    }
/*<       grid(1)=ft > */
    _BLNK__1.grid[0] = ft;
/*<       a(j)=gee(1,nz) > */
    a[j - 1] = gee_(&c__1, &nz);
/*<   425 continue > */
  L425:
/*<       if(l.gt.1) l=l-1 > */
    if (l > 1) {
      --l;
    }
/*<   430 continue > */
/* L430: */
  }
/*<       grid(1)=gtemp > */
  _BLNK__1.grid[0] = gtemp;
/*<       dden=pi2/cn > */
  dden = _BLNK__1.pi2 / cn;
/*<       do 510 j=1,nfcns > */
  for (j = 1; j <= _BLNK__1.nfcns; ++j) {
/*<       dtemp=0.0 > */
    dtemp = 0.0;
/*<       dnum=(j-1)*dden > */
    dnum = (j - 1) * dden;
/*<       if(nm1.lt.1) go to 505 > */
    if (nm1 < 1) {
      goto L505;
    }
/*<       do 500 k=1,nm1 > */
    for (k = 1; k <= nm1; ++k) {
/*<   500 dtemp=dtemp+a(k+1)*dcos(dnum*k) > */
/* L500: */
      dtemp += a[k] * cos(dnum * k);
    }
/*<   505 dtemp=2.0*dtemp+a(1) > */
  L505:
    dtemp = dtemp * 2.0 + a[0];
/*<   510 alpha(j)=dtemp > */
/* L510: */
    _BLNK__1.alpha[j - 1] = dtemp;
  }
/*<       do 550 j=2,nfcns > */
  for (j = 2; j <= _BLNK__1.nfcns; ++j) {
/*<   550 alpha(j)=2.*alpha(j)/cn > */
/* L550: */
    _BLNK__1.alpha[j - 1] = _BLNK__1.alpha[j - 1] * 2.0 / cn;
  }
/*<       alpha(1)=alpha(1)/cn > */
  _BLNK__1.alpha[0] /= cn;
/*<       if(kkk.eq.1) go to 545 > */
  if (kkk == 1) {
    goto L545;
  }
/*<       p(1)=2.0*alpha(nfcns)*bb+alpha(nm1) > */
  p[0] = _BLNK__1.alpha[_BLNK__1.nfcns - 1] * 2.0 * bb + _BLNK__1.alpha[nm1
									- 1];
/*<       p(2)=2.0*aa*alpha(nfcns) > */
  p[1] = aa * 2.0 * _BLNK__1.alpha[_BLNK__1.nfcns - 1];
/*<       q(1)=alpha(nfcns-2)-alpha(nfcns) > */
  q[0] = _BLNK__1.alpha[_BLNK__1.nfcns - 3] - _BLNK__1.alpha[_BLNK__1.nfcns
							     - 1];
/*<       do 540 j=2,nm1 > */
  for (j = 2; j <= nm1; ++j) {
/*<       if(j.lt.nm1) go to 515 > */
    if (j < nm1) {
      goto L515;
    }
/*<       aa=0.5*aa > */
    aa *= .5;
/*<       bb=0.5*bb > */
    bb *= .5;
/*<   515 continue > */
  L515:
/*<       p(j+1)=0.0 > */
    p[j] = 0.0;
/*<       do 520 k=1,j > */
    for (k = 1; k <= j; ++k) {
/*<       a(k)=p(k) > */
      a[k - 1] = p[k - 1];
/*<   520 p(k)=2.0*bb*a(k) > */
/* L520: */
      p[k - 1] = bb * 2.0 * a[k - 1];
    }
/*<       p(2)=p(2)+a(1)*2.0*aa > */
    p[1] += a[0] * 2.0 * aa;
/*<       jm1=j-1 > */
    jm1 = j - 1;
/*<       do 525 k=1,jm1 > */
    for (k = 1; k <= jm1; ++k) {
/*<   525 p(k)=p(k)+q(k)+aa*a(k+1) > */
/* L525: */
      p[k - 1] = p[k - 1] + q[k - 1] + aa * a[k];
    }
/*<       jp1=j+1 > */
    jp1 = j + 1;
/*<       do 530 k=3,jp1 > */
    for (k = 3; k <= jp1; ++k) {
/*<   530 p(k)=p(k)+aa*a(k-1) > */
/* L530: */
      p[k - 1] += aa * a[k - 2];
    }
/*<       if(j.eq.nm1) go to 540 > */
    if (j == nm1) {
      goto L540;
    }
/*<       do 535 k=1,j > */
    for (k = 1; k <= j; ++k) {
/*<   535 q(k)=-a(k) > */
/* L535: */
      q[k - 1] = -a[k - 1];
    }
/*<       q(1)=q(1)+alpha(nfcns-1-j) > */
    q[0] += _BLNK__1.alpha[_BLNK__1.nfcns - 1 - j - 1];
/*<   540 continue > */
  L540:
    ;
  }
/*<       do 543 j=1,nfcns > */
  for (j = 1; j <= _BLNK__1.nfcns; ++j) {
/*<   543 alpha(j)=p(j) > */
/* L543: */
    _BLNK__1.alpha[j - 1] = p[j - 1];
  }
/*<   545 continue > */
L545:
/*<       if(nfcns.gt.3) return > */
  if (_BLNK__1.nfcns > 3) {
    return 0;
  }
/*<       alpha(nfcns+1)=0.0 > */
  _BLNK__1.alpha[_BLNK__1.nfcns] = 0.0;
/*<       alpha(nfcns+2)=0.0 > */
  _BLNK__1.alpha[_BLNK__1.nfcns + 1] = 0.0;
/*<       return > */
  return 0;
/*<       end > */
}				/* remez_ */


/*<       Double Precision Function D(k,n,m) > */
double 
d_(k, n, m)
     int *k;
     int *n;
     int *m;
{
  /* System generated locals */
  double ret_val;

  /* Local variables */
  static int j, l;
  static double q;

/*<       double precision ad,dev,x,y,q,pi2 > */
/*<       common des,wt,alpha,iext,nfcns,ngrid,pi2,ad,dev,x,y,grid > */
/*<       dimension iext(100),ad(100),alpha(100),x(100),y(100) > */
/*<       dimension des(1600),grid(1600),wt(1600) > */
/*<       d=1.0 > */
  ret_val = 1.0;
/*<       q=x(k) > */
  q = _BLNK__1.x[*k - 1];
/*<       do 3 l=1,m > */
  for (l = 1; l <= *m; ++l) {
/*<       do 2 j=l,n,m > */
    for (j = l; *m < 0 ? j >= *n : j <= *n; j += *m) {
/*<       if(j-k)1,2,1 > */
      if (j - *k != 0) {
	goto L1;
      }
      else {
	goto L2;
      }
/*<     1 d=2.0*d*(q-x(j)) > */
    L1:
      ret_val = ret_val * 2.0 * (q - _BLNK__1.x[j - 1]);
/*<     2 continue > */
    L2:
      ;
    }
/*<     3 continue > */
/* L3: */
  }
/*<       d=1.0/d > */
  ret_val = 1.0 / ret_val;
/*<       return > */
  return ret_val;
/*<       end > */
}				/* d_ */


/*<       Function Gee(k,n) > */
double 
gee_(k, n)
     int *k;
     int *n;
{
  /* System generated locals */
  float ret_val;

  /* Builtin functions */
  double cos();

  /* Local variables */
  static double c, d;
  static int j;
  static double p, xf;

/*<       double precision p,c,d,xf,pi2,ad,dev,x,y > */
/*<       common des,wt,alpha,iext,nfcns,ngrid,pi2,ad,dev,x,y,grid > */
/*<       dimension iext(100),ad(100),alpha(100),x(100),y(100) > */
/*<       dimension des(1600),grid(1600),wt(1600) > */
/*<       p=0.0 > */
  p = 0.0;
/*<       xf=grid(k) > */
  xf = _BLNK__1.grid[*k - 1];
/*<       xf=dcos(pi2*xf) > */
  xf = cos(_BLNK__1.pi2 * xf);
/*<       d=0.0 > */
  d = 0.0;
/*<       do 1 j=1,n > */
  for (j = 1; j <= *n; ++j) {
/*<       c=xf-x(j) > */
    c = xf - _BLNK__1.x[j - 1];
/*<       c=ad(j)/c > */
    c = _BLNK__1.ad[j - 1] / c;
/*<       d=d+c > */
    d += c;
/*<     1 p=p+c*y(j) > */
/* L1: */
    p += c * _BLNK__1.y[j - 1];
  }
/*<       gee=p/d > */
  ret_val = p / d;
/*<       return > */
  return ret_val;
/*<       end > */
}				/* gee_ */


/*<       Subroutine Ouch > */
/* Subroutine */ int 
ouch_()
{
printf("********** FAILURE TO CONVERGE **********\n");
printf("Probable cause is machine rounding error\n");
printf("The impulse response may still be correct\n");
/*<       return > */
  return 0;
/*<       end > */
}				/* ouch_ */
