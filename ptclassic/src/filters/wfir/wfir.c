/*******************************************************************
Version identification:
$Id$

/*
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
*/

Converted from Fortran by f2c, cleaned up by Christopher Hylands
********************************************************************/

#include <stdio.h>
#include <string.h>
#include <math.h>
#include "compat.h"

#define log10e 0.43429448190325182765
#define max(a,b) ((a) >= (b) ? (a) : (b))
#define dabs(x) ((double)((x) >= 0 ? (x) : -(x)))
#define STRSIZE 100
/* Table of constant values */

static float c_b233 = (float) 1.;
static float c_b234 = (float) 0.;
static float c_b240 = (float) .5;

/* @(#)wfir.f   1.2     1/11/91 */
/* main program: window design of linear phase, lowpass, highpass */
/*               bandpass, and bandstop fir digital filters */
/* author:       lawrence r. rabiner and carol a. mcgonegal */
/*               bell laboratories, murray hill, new jersey, 07974 */
/* modified jan. 1978 by doug paul, mit lincoln laboratories */
/* to include subroutines for obtaining filter band edges and ripples */

/* modified july. 1990 by Edward Lee, UCB, for Gabriel compatibility */

/* input:        nf is the filter length in samples */
/*                   3 <= nf <= 1024 */

/*               itype is the window type */
/*                   itype = 1     rectangular window */
/*                   itype = 2     triangular window */
/*                   itype = 3     hamming window */
/*                   itype = 4     generalized hamming window */
/*                   itype = 5     hanning window */
/*                   itype = 6     kaiser (i0-sinh) window */
/*                   itype = 7     chebyshev window */

/*               jtype is the filter type */
/*                   jtype = 1     lowpass filter */
/*                   jtype = 2     highpass filter */
/*                   jtype = 3     bandpass filter */
/*                   jtype = 4     bandstop filter */

/*               fc is the normalized cutoff frequency */
/*                   0 <= fc <= 0.5 */
/*               fl and fh are the normalized filter cutoff frequencies */
/*                   0 <= fl <= fh <= 0.5 */
/*               iwp optionally prints out the window values */
/*                   iwp = 0  do not print */
/*                   iwp = 1  print */
/*               imd requests additional runs */
/*                   imd = 1   new run */
/*                   imd = 0   terminates program */
/* ----------------------------------------------------------------------- */

int main()
{
    FILE *fin, *fout;

    /* System generated locals */
    int i__1;

    /* Builtin functions */
    double atan();
    double sin(), cos();

    /* Local variables */
    static float beta;
    static int otcd1, otcd2;
    static float c;
    extern /* Subroutine */ int chebc_();
    static float g[512];
    static int i, j, k, n;
    static float alpha;
    static char input[STRSIZE];
    static char fname[STRSIZE];
    static float w[512];
    extern /* Subroutine */ int cheby_();
    static float dplog;
    static float c1;
    static float c3;
    static int itype, jtype, i1;
    static float twopi, x0, fc, df, fh, fl;
    static int nf;
    static float dp, pi;
    extern /* Subroutine */ int flchar_();
    static float xn;
    extern /* Subroutine */ int hammin_(), kaiser_(), triang_();
    static char answer[1];
    extern /* Subroutine */ int outimp_();
    static int ieo;
    static float att;


    pi = atan((float) 1.) * (float) 4.;
    twopi = pi * (float) 2.;

/* define i/o device codes */
/* input: input to this program is user-interactive */
/*        that is - a question is written on the user */
/*        terminal (otcd1) and the user types in the answer. */

/* output: standard output is otcd1 and otcd2 */

/*      otcd1 = i1mach(4) */
    otcd1 = 6;
/*      otcd2 = i1mach(2) */
    otcd2 = 6;

    printf(" PROVISIONAL WINDOW FIR FILTER DESIGN\n");
    printf(" USE AT YOUR OWN RISK ---------------\n");

/* input the filter length(nf));
 */

  L10:
    printf(" Enter name of input command file (press <Enter> for manual entry,\n");
    printf(" Sorry, no tilde-expansion.  Give path relative to your\n");
    printf(" home or startup directory): ");
    fgets(input, STRSIZE, stdin);
    sscanf(input,"%s",fname);
    if (strlen(fname) == 0)
      fin = stdin;
    else {
      if ((fin = fopen(fname, "r")) == NULL) {
	perror("Can't open file for reading, defaulting to stdin:");
	fin = stdin;
      }
    }

  L15:
    if (fin == stdin)
      printf(" Enter filter length: ");
    else
      printf(" Filter type: %d\n", jtype);
    fscanf(fin, "%d", &nf);
    if (nf <= 1024) {
      goto L30;
    }
  L20:
    printf("%d", nf);
    goto L15;
  L30:
    if (fin == stdin) {
      printf(" Enter window type (1=Rectangular, 2=Triangular,\n");
      printf("          3=Hamming, 4=Gen. Hamming, 5=Hanning, 6=Kaiser, 7=Chebyshev): ");
    }
    fscanf(fin, "%d", &itype);

/* L35: */
    if (itype != 7 && nf < 3) {
      goto L20;
    }
    if (itype == 7 && (nf == 1 || nf == 2)) {
      goto L20;
    }
    if (fin == stdin) {
      printf(" Enter filter type (1=Lowpass, 2=Highpass,\n");
      printf("                    3=Bandpass, 4=Bandstop): ");
    }
    fscanf(fin, "%d", &jtype);

/* n is half the length of the symmetric filter */

    n = (nf + 1) / 2;
    if (jtype != 1 && jtype != 2) {
      goto L50;
    }

/* for the ideal lowpass or highpass design - input fc */

  L40:
    if (fin == stdin)
      printf(" Enter cutoff frequency (as a fraction of sample frequency): ");
    fscanf(fin, "%f", &fc);
    if (fc > (float) 0. && fc < (float) .5) {
      goto L60;
    }
    printf("You typed in %f.  The value must be between 0 and 0.5\n", fc);

    goto L40;

/* for the ideal bandpass or bandstop design - input fl and fh */

  L50:
    printf(" Enter lower cutoff frequency (as a fraction of sample frequency): ");
    fscanf(fin, "%f", &fl);
    printf(" Enter upper cutoff frequency (as a fraction of sample frequency): ");
    fscanf(fin, "%f", &fh);
    if (fl > (float) 0. && fl < (float) .5 && fh > (float) 0. && fh < (float) .5
	&& fh > fl) {
      goto L60;
    }
    if (fl < (float) 0. || fl > (float) .5)
      printf("Cutoff freq=%f is out of bounds, renter data\n", fl);
    if (fh < (float) 0. || fh > (float) .5)
      printf("Cutoff freq=%f is out of bounds, renter data\n", fh);
    if (fh < fl)
      printf("fh=%f is smaller than fl=%f, renter data\n", fh, fl);
    goto L50;
  L60:
    if (itype != 7) {
      goto L70;
    }

/* input for chebyshev window--2 of the 3 parameters nf, dplog, and df */
/* must be specified, where dplog is the desired filter ripple(db scale), 
 */
/* df is the transition width (normalized) of the filter, */
/* and nf is the filter length.  the unspecified parameter */
/* is read in with the zero value. */

    printf(" Enter chebyshev ripple (in db), and optional transition width,\n");
    printf(" separated by a comma, using decimal points: ");
    fscanf(fin, "%f,%f", &dplog, &df);
    dp = pow(10.0, (double) (-(double) dplog / (float) 20.));
    chebc_(&nf, &dp, &df, &n, &x0, &xn);

/* ieo is an even, odd indicator, ieo = 0 for even, , ieo = 1 for odd */
L70:
 ieo = nf % 2;

/*   The following seemed wrong: had 1 instead of 4.  Even length */
/*   lowpass filters could result in the original. */

    if (ieo == 1 || jtype == 4 || jtype == 3) {
      goto L80;
    }
    printf(" Order must be odd for highpass of lowpass filters -- being increased by 1.\n");

    ++nf;
    n = (nf + 1) / 2;
    ieo = 1;
  L80:

/* compute ideal (unwindowed) impulse response for filter */

    c1 = fc;
    if (jtype == 3 || jtype == 4) {
      c1 = fh - fl;
    }
    if (ieo == 1) {
      g[0] = c1 * (float) 2.;
    }
    i1 = ieo + 1;
    i__1 = n;
    for (i = i1; i <= i__1; ++i) {
      xn = (float) (i - 1);
      if (ieo == 0) {
	xn += (float) .5;
      }
      c = pi * xn;
      c3 = c * c1;
      if (jtype == 1 || jtype == 2) {
	c3 *= (float) 2.;
      }
      g[i - 1] = sin(c3) / c;
      if (jtype == 3 || jtype == 4) {
	g[i - 1] = g[i - 1] * (float) 2. *cos(c * (fl + fh));
      }
/* L90: */
    }

/* compute a rectangular window */

    if (itype == 1)
      printf("rectangular window-nf=%d\n", nf);
    i__1 = n;
    for (i = 1; i <= i__1; ++i) {
      w[i - 1] = (float) 1.;
/* L100: */
    }

/* dispatch on window type */

    switch ((int) itype) {
    case 1:
      goto L200;
    case 2:
      goto L110;
    case 3:
      goto L120;
    case 4:
      goto L140;
    case 5:
      goto L150;
    case 6:
      goto L160;
    case 7:
      goto L170;
    }

/* triangular window */

  L110:
    triang_(&nf, w, &n, &ieo);
    printf("triangular window-nf=%d\n", nf);

    goto L180;

/* hamming window */

  L120:
    alpha = (float) .54;
    printf("hamming window length=%d\n", nf);

  L130:
    beta = (float) 1. - alpha;
    hammin_(&nf, w, &n, &ieo, &alpha, &beta);
    printf("alpha=%f\n", alpha);
    goto L180;

/* generalized hamming window */
/* form of window is w(m)=alpha+beta*cos((twopi*m)/(nf-1)) */
/* beta is automatically set to 1.-alpha */
/* read in alpha */

  L140:
    printf("specify alpha for generalized hamming window: ");
    fscanf(fin,"%f ", &alpha);
    printf("generalized hamming window length= %d \n", nf);
    goto L130;

/* hanning window */

  L150:
    alpha = (float) .5;
    printf("hanning window length= %d\n", nf);

/* increase nf by 2 and n by 1 for hanning window so zero */
/* endpoints are not part of window */

    nf += 2;
    ++n;
    goto L130;

/* kaiser (i0-sinh) window */
/* need to specify parameter att=stopband attenuation in db */

  L160:
    printf("specify attenuation in db for kaiser window: ");
    fscanf(fin, "%f", &att);
    if (att > (float) 50.) {
      beta = (att - (float) 8.7) * (float) .1102;
    }
    if (att >= (float) 20.96 && att <= (float) 50.) {
      beta = pow((double) (att - (float) 20.96), 0.4 ) * (float) .58417 + (att - (float) 20.96) *
	(float) .07886;
    }
    if (att < (float) 20.96) {
      beta = (float) 0.;
    }
    kaiser_(&nf, w, &n, &ieo, &beta);
    printf("kaiser window length=%d\n", nf);

    printf("attenuaton=%f beta=%f\n", att, beta);
    goto L180;

/* chebyshev window */

  L170:
    cheby_(&nf, w, &n, &ieo, &dp, &df, &x0, &xn);
    printf("Chebyshev window length= %d\n", nf);
    printf("ripple= %f transition width=%f\n", dp, df);

/* window ideal filter response */
/* change back nf and n for hanning window */

  L180:
    if (itype == 5) {
      nf += -2;
    }
    if (itype == 5) {
      --n;
    }
    i__1 = n;
    for (i = 1; i <= i__1; ++i) {
      g[i - 1] *= w[i - 1];
/* L190: */
    }

/* optionally print out results */

  L200:
    if (fin == stdin)
      printf(" Print window values? (y/n): ");
    fscanf(fin, "%s", answer);
    if (*answer == 'n' || *answer == 'N') {
      goto L220;
    }
    printf("window values\n");
    i__1 = n;
    for (i = 1; i <= i__1; ++i) {
      j = n + 1 - i;
      k = nf + 1 - i;
      printf("          w(%3d)=% E =w(%4d)\n", i, w[j - 1], k);
/* L210: */
    }
  L220:
    if (jtype == 1)
      printf("**lowpass filter design**\n");
    if (jtype == 2)
      printf("**highpass filter design**\n");
    if (jtype == 3)
      printf("**bandpass filter design**\n");
    if (jtype == 4)
      printf("**bandstop filter design**\n");
    if (jtype == 1)
      printf(" ideal lowpass cutoff=     %f\n", fc);
    if (jtype == 2)
      printf(" ideal highpass cutoff=    %f\n", fc);
    if (jtype == 3 || jtype == 4)
      printf(" ideal cutoff frequencies= %f %f\n", fl, fh);
    if (jtype == 1 || jtype == 3) {
      goto L240;
    }
    i__1 = n;
    for (i = 2; i <= i__1; ++i) {
      g[i - 1] = -(double) g[i - 1];
/* L230: */
    }
    g[0] = (float) 1. - g[0];

/* write out impulse response */



  L240:
    i__1 = n;
    for (i = 1; i <= i__1; ++i) {
      j = n + 1 - i;
      k = nf + 1 - i;
      printf("          h(%3d)=% E =h(%4d)\n", i, g[j - 1], k);

/* L250: */
    }
    flchar_(&nf, &itype, &jtype, &fc, &fl, &fh, &n, &ieo, g, &otcd2);

/*   Open output file */

    fname[0] = '\0';
    if (fin == stdin) {
      printf(" Enter name of window values output file ");
      printf(" (Sorry, no tilde-expansion.  Give path relative to your home directory): ");
    }
    fscanf(fin, "%s", fname);
    /*<       open (1, file=fname) > */
    if (fin != stdin)
      printf("window values output file: %s\n", fname);
    if ((fout = fopen(fname, "w")) == NULL) {
      perror("Could not open output file:");
    } else {
      outimp_(g, &n, &nf, &ieo, fout);
      fclose(fout);
    }
    printf(" Another design? (y/n): ");
    scanf("%s", answer);
    if (*answer == 'y' || *answer == 'Y') {
      goto L10;
    }
    printf("End of program\n");
    return 0;
  }				/* MAIN__ */


/* ----------------------------------------------------------------------- */
/* subroutine:  triang */
/* triangular window */
/* ----------------------------------------------------------------------- */

/* Subroutine */ int triang_(nf, w, n, ieo)
     int *nf;
     float *w;
     int *n;
     int *ieo;
{
    /* System generated locals */
    int i__1;

    /* Local variables */
    static int i;
    static float fn, xi;


/*  nf = filter length in samples */
/*   w = window coefficients for half the window */
/*   n = half window length=(nf+1)/2 */
/* ieo = even - odd indication--ieo=0 for nf even */

    /* Parameter adjustments */
    --w;

    /* Function Body */
    fn = (float) (*n);
    i__1 = *n;
    for (i = 1; i <= i__1; ++i) {
      xi = (float) (i - 1);
      if (*ieo == 0) {
	xi += (float) .5;
      }
      w[i] = (float) 1. - xi / fn;
/* L10: */
    }
    return 0;
  }				/* triang_ */


/* ----------------------------------------------------------------------- */
/* subroutine:  hammin */
/* generalized hamming window routine */
/* window is w(n) = alpha + beta * cos( twopi*(n-1) / (nf-1) ) */
/* ----------------------------------------------------------------------- */

/* Subroutine */ int hammin_(nf, w, n, ieo, alpha, beta)
     int *nf;
     float *w;
     int *n;
     int *ieo;
     float *alpha;
     float *beta;
{
    /* System generated locals */
    int i__1;

    /* Builtin functions */
    double atan(), cos();

    /* Local variables */
    static int i;
    static float fi, fn, pi2;


/*    nf = filter length in samples */
/*     w = window array of size n */
/*     n = half length of filter=(nf+1)/2 */
/*   ieo = even odd indicator--ieo=0 if nf even */
/* alpha = constant of window */
/*  beta = constant of window--generally beta=1-alpha */

    /* Parameter adjustments */
    --w;

    /* Function Body */
    pi2 = atan((float) 1.) * (float) 8.;
    fn = (float) (*nf - 1);
    i__1 = *n;
    for (i = 1; i <= i__1; ++i) {
      fi = (float) (i - 1);
      if (*ieo == 0) {
	fi += (float) .5;
      }
      w[i] = *alpha + *beta * cos(pi2 * fi / fn);
/* L10: */
    }
    return 0;
  }				/* hammin_ */


/* ----------------------------------------------------------------------- */
/* subroutine:  kaiser */
/* kaiser window */
/* ----------------------------------------------------------------------- */

/* Subroutine */ int kaiser_(nf, w, n, ieo, beta)
     int *nf;
     float *w;
     int *n;
     int *ieo;
     float *beta;
{
    /* System generated locals */
    int i__1;
    float r__1;

    /* Builtin functions */
    double sqrt();

    /* Local variables */
    static float xind;
    static int i;
    static float xi, bes;
    extern double ino_();


/*   nf = filter length in samples */
/*    w = window array of size n */
/*    n = filter half length=(nf+1)/2 */
/*  ieo = even odd indicator--ieo=0 if nf even */
/* beta = parameter of kaiser window */

    /* Parameter adjustments */
    --w;

    /* Function Body */
    bes = ino_(beta);
    xind = (float) (*nf - 1) * (float) (*nf - 1);
    i__1 = *n;
    for (i = 1; i <= i__1; ++i) {
      xi = (float) (i - 1);
      if (*ieo == 0) {
	xi += (float) .5;
      }
      xi = xi * (float) 4. *xi;
      r__1 = *beta * sqrt((float) 1. - xi / xind);
      w[i] = ino_(&r__1);
      w[i] /= bes;
/* L10: */
    }
    return 0;
  }				/* kaiser_ */


/* ----------------------------------------------------------------------- */
/* function:  ino */
/* bessel function for kaiser window */
/* ----------------------------------------------------------------------- */

  double ino_(x)
     float *x;
{
    /* System generated locals */
    float ret_val;

    /* Local variables */
    static float e;
    static int i;
    static float t, y, de, xi, sde;

    y = *x / (float) 2.;
    t = (float) 1e-8;
    e = (float) 1.;
    de = (float) 1.;
    for (i = 1; i <= 25; ++i) {
      xi = (float) i;
      de = de * y / xi;
      sde = de * de;
      e += sde;
      if (e * t - sde <= (float) 0.) {
	goto L10;
      }
      else {
	goto L20;
      }
    L10:
      ;
    }
  L20:
    ret_val = e;
    return ret_val;
  }				/* ino_ */


/* ----------------------------------------------------------------------- */
/* subroutine:  chebc */
/* subroutine to generate chebyshev window parameters when */
/* one of the three parameters nf,dp and df is unspecified */
/* ----------------------------------------------------------------------- */

/* Subroutine */ int chebc_(nf, dp, df, n, x0, xn)
     int *nf;
     float *dp;
     float *df;
     int *n;
     float *x0;
     float *xn;
{
    /* System generated locals */
    float r__1;

    /* Builtin functions */
    double atan(), cos(), cosh();

    /* Local variables */
    static float x, c0, c1, c2, pi;
    extern double arccos_(), coshin_();


/* nf = filter length (in samples) */
/* dp = filter ripple (absolute scale) */
/* df = normalized transition width of filter */
/*  n = (nf+1)/2 = filter half length */
/* x0 = (3-c0)/(1+c0) with c0=cos(pi*df) = chebyshev window constant */
/* xn = nf-1 */

    pi = atan((float) 1.) * (float) 4.;
    if (*nf != 0) {
      goto L10;
    }

/* dp,df specified, determine nf */

    r__1 = (*dp + (float) 1.) / *dp;
    c1 = coshin_(&r__1);
    c0 = cos(pi * *df);
    r__1 = (float) 1. / c0;
    x = c1 / coshin_(&r__1) + (float) 1.;

/* increment by 1 to give nf which meets or exceeds specs on dp and df */

    *nf = x + (float) 1.;
    *n = (*nf + 1) / 2;
    *xn = (float) (*nf - 1);
    goto L30;
  L10:
    if (*df != (float) 0.) {
      goto L20;
    }

/* nf,dp specified, determine df */

    *xn = (float) (*nf - 1);
    r__1 = (*dp + (float) 1.) / *dp;
    c1 = coshin_(&r__1);
    c2 = cosh(c1 / *xn);
    r__1 = (float) 1. / c2;
    *df = arccos_(&r__1) / pi;
    goto L30;

/* nf,df specified, determine dp */

  L20:
    *xn = (float) (*nf - 1);
    c0 = cos(pi * *df);
    r__1 = (float) 1. / c0;
    c1 = *xn * coshin_(&r__1);
    *dp = (float) 1. / (cosh(c1) - (float) 1.);
  L30:
    *x0 = ((float) 3. - cos(pi * (float) 2. * *df)) / (cos(pi * (float) 2. * *df)
						       + (float) 1.);
    return 0;
  }				/* chebc_ */


/* ----------------------------------------------------------------------- */
/* subroutine:  cheby */
/* dolph chebyshev window design */
/* ----------------------------------------------------------------------- */

/* Subroutine */ int cheby_(nf, w, n, ieo, dp, df, x0, xn)
     int *nf;
     float *w;
     int *n;
     int *ieo;
     float *dp;
     float *df;
     float *x0;
     float *xn;
{
    /* System generated locals */
    int i__1, i__2;

    /* Builtin functions */
    double atan(), cos(), cosh(), sin();

    /* Local variables */
    static float beta, f;
    static int i, j;
    static float p, alpha, x, c1, c2, twopi, pi[1024], xi, pr[1024], xj;
    extern double arccos_(), coshin_();
    static float fnf, pie, sum, twn;


/*  nf = filter length in samples */
/*   w = window array of size n */
/*   n = half length of filter = (nf+1)/2 */
/* ieo = even-odd indicator--ieo=0 for nf even */
/*  dp = window ripple on an absolute scale */
/*  df = normalized transition width of window */
/*  x0 = window parameter related to transition width */
/*  xn = nf-1 */

    /* Parameter adjustments */
    --w;

    /* Function Body */
    pie = atan((float) 1.) * (float) 4.;
    *xn = (float) (*nf - 1);
    fnf = (float) (*nf);
    alpha = (*x0 + (float) 1.) / (float) 2.;
    beta = (*x0 - (float) 1.) / (float) 2.;
    twopi = pie * (float) 2.;
    c2 = *xn / (float) 2.;
    i__1 = *nf;
    for (i = 1; i <= i__1; ++i) {
      xi = (float) (i - 1);
      f = xi / fnf;
      x = alpha * cos(twopi * f) + beta;
      if (dabs(x) - (float) 1. <= (float) 0.) {
	goto L10;
      }
      else {
	goto L20;
      }
    L10:
      p = *dp * cos(c2 * arccos_(&x));
      goto L30;
    L20:
      p = *dp * cosh(c2 * coshin_(&x));
    L30:
      pi[i - 1] = (float) 0.;
      pr[i - 1] = p;

/* for even length filters use a one-half sample delay */
/* also the frequency response is antisymmetric in frequency */

      if (*ieo == 1) {
	goto L40;
      }
      pr[i - 1] = p * cos(pie * f);
      pi[i - 1] = -(double) p *sin(pie * f);
      if (i > *nf / 2 + 1) {
	pr[i - 1] = -(double) pr[i - 1];
      }
      if (i > *nf / 2 + 1) {
	pi[i - 1] = -(double) pi[i - 1];
      }
    L40:
      ;
    }

/* use dft to give window */

    twn = twopi / fnf;
    i__1 = *n;
    for (i = 1; i <= i__1; ++i) {
      xi = (float) (i - 1);
      sum = (float) 0.;
      i__2 = *nf;
      for (j = 1; j <= i__2; ++j) {
	xj = (float) (j - 1);
	sum = sum + pr[j - 1] * cos(twn * xj * xi) + pi[j - 1] * sin(twn *
								   xj * xi);
/* L50: */
      }
      w[i] = sum;
/* L60: */
    }
    c1 = w[1];
    i__1 = *n;
    for (i = 1; i <= i__1; ++i) {
      w[i] /= c1;
/* L70: */
    }
    return 0;
  }				/* cheby_ */


/* ----------------------------------------------------------------------- */
/* function:  coshin */
/* function for hyperbolic inverse cosine of x */
/* ----------------------------------------------------------------------- */

  double coshin_(x)
     float *x;
{
    /* System generated locals */
    float ret_val;

    /* Builtin functions */
    double sqrt(), log();

    ret_val = log(*x + sqrt(*x * *x - (float) 1.));
    return ret_val;
  }				/* coshin_ */


/* ----------------------------------------------------------------------- */
/* function:  arccos */
/* function for inverse cosine of x */
/* ----------------------------------------------------------------------- */

  double arccos_(x)
     float *x;
{
    /* System generated locals */
    float ret_val;

    /* Builtin functions */
    double sqrt(), atan();

    /* Local variables */
    static float a;

    if (*x < (float) 0.) {
      goto L30;
    }
    else if (*x == 0) {
      goto L20;
    }
    else {
      goto L10;
    }
  L10:
    a = sqrt((float) 1. - *x * *x) / *x;
    ret_val = atan(a);
    return ret_val;
  L20:
    ret_val = atan((float) 1.) * (float) 2.;
    return ret_val;
  L30:
    a = sqrt((float) 1. - *x * *x) / *x;
    ret_val = atan(a) + atan((float) 1.) * (float) 4.;
    return ret_val;
  }				/* arccos_ */


/* ----------------------------------------------------------------------- */
/* function:  cosh */
/* function for hyperbolic cosine of x */
/* ----------------------------------------------------------------------- */

  double cosh_(x)
     float *x;
{
    /* System generated locals */
    float ret_val;

    /* Builtin functions */
    double exp();

    ret_val = (exp(*x) + exp(-(double) (*x))) / (float) 2.;
    return ret_val;
  }				/* cosh_ */


/* ----------------------------------------------------------------------- */
/* subroutine:  flchar */
/* subroutine to determine filter characteristics */
/* ----------------------------------------------------------------------- */

/* Subroutine */ int flchar_(nf, itype, jtype, fc, fl, fh, n, ieo, g, otcd2)
     int *nf;
     int *itype;
     int *jtype;
     float *fc;
     float *fl;
     float *fh;
     int *n;
     int *ieo;
     float *g;
     int *otcd2;
{
    /* System generated locals */
    int i__1, i__2;

    /* Builtin functions */
    double atan(), cos();

    /* Local variables */
    static float resp[2048], sumi, twni;
    static int i, j;
    static float f1, f2, db, pi;
    static int nr;
    static float xi, xj;
    extern /* Subroutine */ int ripple_();
    static float sum, xnr, twn;

/*    nf = filter length in samples */
/* itype = window type */
/* jtype = filter type */
/*    fc = ideal cutoff of lp or hp filter */
/*    fl = lower cutoff of bp or bs filter */
/*    fh = upper cutoff of bp or bs filter */
/*     n = filter half length = (nf+1) / 2 */
/*   ieo = even odd indicator */
/*     g = filter array of size n */
/* otcd2 = output code for line printer used in write statements */


/* not for for triangular window */

    /* Parameter adjustments */
    --g;

    /* Function Body */
    if (*itype == 2) {
      return 0;
    }

/* dft to get freq resp */

    pi = atan((float) 1.) * (float) 4.;

/* up to 4096 pt dft */

    nr = *nf << 3;
    if (nr > 2048) {
      nr = 2048;
    }
    xnr = (float) nr;
    twn = pi / xnr;
    sumi = -(double) g[1] / (float) 2.;
    if (*ieo == 0) {
      sumi = (float) 0.;
    }
    i__1 = nr;
    for (i = 1; i <= i__1; ++i) {
      xi = (float) (i - 1);
      twni = twn * xi;
      sum = sumi;
      i__2 = *n;
      for (j = 1; j <= i__2; ++j) {
	xj = (float) (j - 1);
	if (*ieo == 0) {
	  xj += (float) .5;
	}
	sum += g[j] * cos(xj * twni);
/* L10: */
      }
      resp[i - 1] = sum * (float) 2.;
/* L20: */
    }

/* dispatch on filter type */

    switch ((int) *jtype) {
    case 1:
      goto L30;
    case 2:
      goto L40;
    case 3:
      goto L50;
    case 4:
      goto L60;
    }

/* lowpass */

  L30:
    ripple_(&nr, &c_b233, &c_b234, fc, resp, &f1, &f2, &db);
    printf("passband cutoff %f ripple %f db\n", f2, db);

    ripple_(&nr, &c_b234, fc, &c_b240, resp, &f1, &f2, &db);
    printf("stopband cutoff %f ripple %f db\n", f1, db);

    return 0;

/* highpass */

  L40:
    ripple_(&nr, &c_b234, &c_b234, fc, resp, &f1, &f2, &db);
    printf("stopband cutoff %f ripple %f db\n", f2, db);

    ripple_(&nr, &c_b233, fc, &c_b240, resp, &f1, &f2, &db);
    printf("passband cutoff %f ripple %f db\n", f1, db);

    return 0;

/* bandpass */

  L50:
    ripple_(&nr, &c_b234, &c_b234, fl, resp, &f1, &f2, &db);
    printf("stopband cutoff %f ripple %f db\n", f2, db);

    ripple_(&nr, &c_b233, fl, fh, resp, &f1, &f2, &db);
    printf("passband cutoffs %f %f ripple %f db\n", f1, f2, db);

    ripple_(&nr, &c_b234, fh, &c_b240, resp, &f1, &f2, &db);
    printf("stopband cutoff %f ripple %f db\n", f1, db);
    return 0;

/* stopband */

  L60:
    ripple_(&nr, &c_b233, &c_b234, fl, resp, &f1, &f2, &db);
    printf("passband cutoff %f ripple %f db\n", f2, db);

    ripple_(&nr, &c_b234, fl, fh, resp, &f1, &f2, &db);
    printf("stopband cutoffs %f %f ripple %f\n", f1, f2, db);

    ripple_(&nr, &c_b233, fh, &c_b240, resp, &f1, &f2, &db);
    printf("passband cutoff %f ripple %f db\n", f1, db);

    return 0;
  }				/* flchar_ */


/* ----------------------------------------------------------------------- */
/* subroutine:  ripple */
/* finds largest ripple in band and locates band edges based on the */
/* point where the transition region crosses the measured ripple bound */
/* ----------------------------------------------------------------------- */

/* Subroutine */ int ripple_(nr, rideal, flow, fhi, resp, f1, f2, db)
     int *nr;
     float *rideal;
     float *flow;
     float *fhi;
     float *resp;
     float *f1;
     float *f2;
     float *db;
{
    /* System generated locals */
    int i__1;
    float r__1, r__2;

    /* Local variables */
    static int ifhi;
    static float rmin, ripl, rmax;
    static int i, j, iflow;
    static float x0, x1, y1, y0, xi, xnr;


/*     nr = size of resp */
/* rideal = ideal frequency response */
/*   flow = low edge of ideal band */
/*    fhi = high edge of ideal band */
/*   resp = frequency response of size nr */
/*     f1 = computed lower band edge */
/*     f2 = computed upper band edge */
/*     db = deviation from ideal response in db */

    /* Parameter adjustments */
    --resp;

    /* Function Body */
    xnr = (float) (*nr);

/* band limits */

    iflow = xnr * (float) 2. **flow + (float) 1.5;
    ifhi = xnr * (float) 2. **fhi + (float) 1.5;
    if (iflow == 0) {
      iflow = 1;
    }
    if (ifhi >= *nr) {
      ifhi = *nr - 1;
    }

/* find max and min peaks in band */

    rmin = *rideal;
    rmax = *rideal;
    i__1 = ifhi;
    for (i = iflow; i <= i__1; ++i) {
      if (resp[i] <= rmax || resp[i] < resp[i - 1] || resp[i] < resp[i + 1]) {
	goto L10;
      }
      rmax = resp[i];
    L10:
      if (resp[i] >= rmin || resp[i] > resp[i - 1] || resp[i] > resp[i + 1]) {
	goto L20;
      }
      rmin = resp[i];
    L20:
      ;
    }

/* peak deviation from ideal */

/* Computing MAX */
    r__1 = rmax - *rideal, r__2 = *rideal - rmin;
    ripl = (double)max(r__1, r__2);

/* search for lower band edge */

    *f1 = *flow;
    if (*flow == (float) 0.) {
      goto L50;
    }
    i__1 = ifhi;
    for (i = iflow; i <= i__1; ++i) {
      if ((r__1 = resp[i] - *rideal, dabs(r__1)) <= ripl) {
	goto L40;
      }
/* L30: */
    }
  L40:
    xi = (float) (i - 1);

/* linear interpolation of band edge frequency to improve accuracy */

    x1 = xi * (float) .5 / xnr;
    x0 = (xi - (float) 1.) * (float) .5 / xnr;
    y1 = (r__1 = resp[i] - *rideal, dabs(r__1));
    y0 = (r__1 = resp[i - 1] - *rideal, dabs(r__1));
    *f1 = (x1 - x0) / (y1 - y0) * (ripl - y0) + x0;

/* search for upper band edge */

  L50:
    *f2 = *fhi;
    if (*fhi == (float) .5) {
      goto L80;
    }
    i__1 = ifhi;
    for (i = iflow; i <= i__1; ++i) {
      j = ifhi + iflow - i;
      if ((r__1 = resp[j] - *rideal, dabs(r__1)) <= ripl) {
	goto L70;
      }
/* L60: */
    }
  L70:
    xi = (float) (j - 1);

/* linear interpolation of band edge frequency to improve accuracy */

    x1 = xi * (float) .5 / xnr;
    x0 = (xi + (float) 1.) * (float) .5 / xnr;
    y1 = (r__1 = resp[j] - *rideal, dabs(r__1));
    y0 = (r__1 = resp[j + 1] - *rideal, dabs(r__1));
    *f2 = (x1 - x0) / (y1 - y0) * (ripl - y0) + x0;

/* deviation from ideal in db */

  L80:
    *db = log10e * log(ripl + *rideal) * (float) 20.;
    return 0;
  }				/* ripple_ */


/* ----------------------------------------------------------------------- */

/* ----------------------------------------------------------------------- */
/* subroutine: outimp */
/*    Prints out on unit ioutd the full */
/*    impulse response as format 13x,f15.8.  This makes the impulse */
/*    response accessible to other programs without the need for parsing */
/*    the output of the program. */
/* ----------------------------------------------------------------------- */

/* Subroutine */ int outimp_(g, n, nf, ieo, ioutd)
     float *g;
     int *n;
     int *nf;
     int *ieo;
     FILE *ioutd;
{
    /* System generated locals */
    int i__1;

    /* Builtin functions */

    /* Local variables */
    static int i, j, l;

/*      Removed filter order output for Gabriel compatibility */

/*     write(ioutd,5) nf */
/*  5  format(i15) */
    /* Parameter adjustments */
    --g;

    /* Function Body */
    l = *n;
    if (*ieo == 0) {
      goto L10;
    }
    l = *n - 1;
  L10:
    i__1 = l;
    for (i = 1; i <= i__1; ++i) {
      j = *n + 1 - i;
      fprintf(ioutd, "% E\n", g[j]);

/* L20: */
    }
    if (*ieo == 0) {
      goto L30;
    }
    fprintf(ioutd, "% 15.8E\n ", g[1]);

  L30:
    for (i = l; i >= 1; --i) {
      j = *n + 1 - i;
      fprintf(ioutd, "% E\n ", g[j]);

/* L40: */
    }
    fclose(ioutd);
/* L60: */
    return 0;
  }				/* outimp_ */
