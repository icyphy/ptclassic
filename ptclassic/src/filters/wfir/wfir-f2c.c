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
/* wfir.f -- translated by f2c (version of 23 April 1993  18:34:30).
   You must link the resulting object file with the libraries:
	-lf2c -lm   (in that order)
*/

#include "f2c.h"

/* Table of constant values */

static integer c__1 = 1;
static integer c__3 = 3;
static integer c__4 = 4;
static doublereal c_b81 = 10.;
static doublereal c_b121 = .4;
static real c_b233 = (float)1.;
static real c_b234 = (float)0.;
static real c_b240 = (float).5;

/* @(#)wfir.f	1.2	1/11/91 */
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

/* Main program */ MAIN__()
{
    /* Format strings */
    static char fmt_9997[] = "(\002 filter length =\002,i4,\002 is out of bo\
unds\002)";
    static char fmt_9995[] = "(\002 Cutoff freq=\002,f14.7,\002 is out of bo\
unds, reenter da\002,\002ta\002)";
    static char fmt_9992[] = "(\002 fh=\002,f14.7,\002 is smaller than fl\
=\002,f14.7,\002 reenter\002,\002 data\002)";
    static char fmt_9993[] = "(2f14.7)";
    static char fmt_9989[] = "(\002 rectangular window-nf=\002,i4)";
    static char fmt_9988[] = "(\002 triangular window-nf=\002,i4)";
    static char fmt_9987[] = "(\002 hamming window length=\002,i4)";
    static char fmt_9986[] = "(\002 alpha=\002,f14.7)";
    static char fmt_9985[] = "(\002 specify alpha for generalized hamming wi\
ndow\002)";
    static char fmt_9984[] = "(\002 generalized hamming window length=\002,i\
4)";
    static char fmt_9983[] = "(\002 hanning window length=\002,i4)";
    static char fmt_9982[] = "(\002 specify attenuation in db(f14.7)\002,\
\002 for kaiser wind\002,\002ow\002)";
    static char fmt_9981[] = "(\002 kaiser window length=\002,i4)";
    static char fmt_9980[] = "(\002  attenuation=\002,f14.7,\002  beta=\002,\
f14.7)";
    static char fmt_9979[] = "(\002 Chebyshev window length=\002,i4)";
    static char fmt_9978[] = "(\002 ripple=\002,f14.7,\002  transition wid\
th=\002,f14.7)";
    static char fmt_9975[] = "(\002 window values\002)";
    static char fmt_9974[] = "(10x,\002 w(\002,i3,\002)=\002,e15.8,\002 =w\
(\002,i4,\002)\002)";
    static char fmt_9973[] = "(\002 **lowpass filter design**\002)";
    static char fmt_9972[] = "(\002 **highpass filter design**\002)";
    static char fmt_9971[] = "(\002 **bandpass filter design**\002)";
    static char fmt_9970[] = "(\002 **bandstop filter design**\002)";
    static char fmt_9969[] = "(\002 ideal lowpass cutoff=\002,f14.7)";
    static char fmt_9968[] = "(\002 ideal highpass cutoff=\002,f14.7)";
    static char fmt_9967[] = "(\002 ideal cutoff frequencies=\002,2f14.7)";
    static char fmt_9966[] = "(10x,\002 h(\002,i3,\002)=\002,e15.8,\002 =h\
(\002,i4,\002)\002)";
    static char fmt_9965[] = "(\002 \002/)";

    /* System generated locals */
    integer i__1;
    doublereal d__1;
    olist o__1;
    cllist cl__1;

    /* Builtin functions */
    double atan();
    integer s_wsfe(), do_fio(), e_wsfe(), s_rsfe(), e_rsfe(), s_cmp(), f_open(
	    ), s_rsle(), do_lio(), e_rsle();
    double pow_dd(), sin(), cos();
    integer f_clos();
    /* Subroutine */ int s_stop();

    /* Local variables */
    static real beta;
    static integer otcd1, otcd2;
    static real c;
    extern /* Subroutine */ int chebc_();
    static real g[512];
    static integer i, j, k, n;
    static real alpha;
    static char fname[100];
    static real w[512];
    extern /* Subroutine */ int cheby_();
    static integer incod;
    static real dplog;
    extern /* Subroutine */ int flush_();
    static real c1;
    static integer ioutd;
    static real c3;
    static integer itype, jtype, i1;
    static real twopi, x0, fc, df, fh, fl;
    static integer nf;
    static real dp, pi;
    extern /* Subroutine */ int flchar_();
    static real xn;
    extern /* Subroutine */ int hammin_(), kaiser_(), triang_();
    static char answer[1];
    extern /* Subroutine */ int outimp_();
    static integer ieo;
    static real att;

    /* Fortran I/O blocks */
    static cilist io___5 = { 0, 6, 0, "(,a)", 0 };
    static cilist io___6 = { 0, 6, 0, "(,a)", 0 };
    static cilist io___7 = { 0, 6, 0, "(/,a)", 0 };
    static cilist io___8 = { 0, 6, 0, "(,a)", 0 };
    static cilist io___9 = { 0, 5, 0, "(a12)", 0 };
    static cilist io___12 = { 0, 6, 0, "(/,a)", 0 };
    static cilist io___13 = { 0, 0, 0, 0, 0 };
    static cilist io___15 = { 0, 0, 0, fmt_9997, 0 };
    static cilist io___16 = { 0, 6, 0, "(,a,/,10x,a)", 0 };
    static cilist io___17 = { 0, 0, 0, 0, 0 };
    static cilist io___19 = { 0, 6, 0, "(,a,/,20x,a)", 0 };
    static cilist io___20 = { 0, 0, 0, 0, 0 };
    static cilist io___23 = { 0, 6, 0, "(,a)", 0 };
    static cilist io___24 = { 0, 0, 0, 0, 0 };
    static cilist io___26 = { 0, 0, 0, fmt_9995, 0 };
    static cilist io___27 = { 0, 6, 0, "(,a)", 0 };
    static cilist io___28 = { 0, 0, 0, 0, 0 };
    static cilist io___30 = { 0, 6, 0, "(,a)", 0 };
    static cilist io___31 = { 0, 0, 0, 0, 0 };
    static cilist io___33 = { 0, 0, 0, fmt_9995, 0 };
    static cilist io___34 = { 0, 0, 0, fmt_9995, 0 };
    static cilist io___35 = { 0, 0, 0, fmt_9992, 0 };
    static cilist io___36 = { 0, 6, 0, "(,a)", 0 };
    static cilist io___37 = { 0, 6, 0, "(,a)", 0 };
    static cilist io___38 = { 0, 0, 0, fmt_9993, 0 };
    static cilist io___45 = { 0, 6, 0, "(,a)", 0 };
    static cilist io___52 = { 0, 0, 0, fmt_9989, 0 };
    static cilist io___54 = { 0, 0, 0, fmt_9988, 0 };
    static cilist io___56 = { 0, 0, 0, fmt_9987, 0 };
    static cilist io___58 = { 0, 0, 0, fmt_9986, 0 };
    static cilist io___59 = { 0, 0, 0, fmt_9985, 0 };
    static cilist io___60 = { 0, 0, 0, fmt_9993, 0 };
    static cilist io___61 = { 0, 0, 0, fmt_9984, 0 };
    static cilist io___62 = { 0, 0, 0, fmt_9983, 0 };
    static cilist io___63 = { 0, 0, 0, fmt_9982, 0 };
    static cilist io___64 = { 0, 0, 0, fmt_9993, 0 };
    static cilist io___66 = { 0, 0, 0, fmt_9981, 0 };
    static cilist io___67 = { 0, 0, 0, fmt_9980, 0 };
    static cilist io___68 = { 0, 0, 0, fmt_9979, 0 };
    static cilist io___69 = { 0, 0, 0, fmt_9978, 0 };
    static cilist io___70 = { 0, 6, 0, "(/,a)", 0 };
    static cilist io___71 = { 0, 5, 0, "(a1)", 0 };
    static cilist io___73 = { 0, 0, 0, fmt_9975, 0 };
    static cilist io___76 = { 0, 0, 0, fmt_9974, 0 };
    static cilist io___77 = { 0, 0, 0, fmt_9973, 0 };
    static cilist io___78 = { 0, 0, 0, fmt_9972, 0 };
    static cilist io___79 = { 0, 0, 0, fmt_9971, 0 };
    static cilist io___80 = { 0, 0, 0, fmt_9970, 0 };
    static cilist io___81 = { 0, 0, 0, fmt_9969, 0 };
    static cilist io___82 = { 0, 0, 0, fmt_9968, 0 };
    static cilist io___83 = { 0, 0, 0, fmt_9967, 0 };
    static cilist io___84 = { 0, 0, 0, fmt_9966, 0 };
    static cilist io___85 = { 0, 0, 0, fmt_9965, 0 };
    static cilist io___87 = { 0, 6, 0, "(,a)", 0 };
    static cilist io___88 = { 0, 6, 0, "(,a)", 0 };
    static cilist io___89 = { 0, 0, 0, "(a)", 0 };
    static cilist io___90 = { 0, 6, 0, "(/,a)", 0 };
    static cilist io___91 = { 0, 5, 0, "(a1)", 0 };



    pi = atan((float)1.) * (float)4.;
    twopi = pi * (float)2.;

/* define i/o device codes */
/* input: input to this program is user-interactive */
/*        that is - a question is written on the user */
/*        terminal (otcd1) and the user types in the answer. */

/* output: standard output is otcd1 and otcd2 */

/*      otcd1 = i1mach(4) */
    otcd1 = 6;
/*      otcd2 = i1mach(2) */
    otcd2 = 6;

    s_wsfe(&io___5);
    do_fio(&c__1, " PROVISIONAL WINDOW FIR FILTER DESIGN ", 38L);
    e_wsfe();
    s_wsfe(&io___6);
    do_fio(&c__1, " USE AT YOUR OWN RISK --------------- ", 38L);
    e_wsfe();

/* input the filter length(nf), window type(itype) and filter type(jtype) 
*/

L10:
    s_wsfe(&io___7);
    do_fio(&c__1, " Enter name of input command file (press <Enter> for manu\
al entry, ", 67L);
    e_wsfe();
    s_wsfe(&io___8);
    do_fio(&c__1, " Sorry, no tilde-expansion.  Give path relative to your h\
ome or startup directory): ", 84L);
    e_wsfe();
    s_rsfe(&io___9);
    do_fio(&c__1, fname, 100L);
    e_rsfe();
    if (s_cmp(fname, " ", 100L, 1L) == 0) {
	incod = 5;
    } else {
	incod = 3;
	o__1.oerr = 0;
	o__1.ounit = 3;
	o__1.ofnmlen = 100;
	o__1.ofnm = fname;
	o__1.orl = 0;
	o__1.osta = 0;
	o__1.oacc = 0;
	o__1.ofm = 0;
	o__1.oblnk = 0;
	f_open(&o__1);
    }

L15:
    s_wsfe(&io___12);
    do_fio(&c__1, " Enter filter length: ", 22L);
    e_wsfe();
    io___13.ciunit = incod;
    s_rsle(&io___13);
    do_lio(&c__3, &c__1, (char *)&nf, (ftnlen)sizeof(integer));
    e_rsle();
    if (nf <= 1024) {
	goto L30;
    }
L20:
    io___15.ciunit = otcd2;
    s_wsfe(&io___15);
    do_fio(&c__1, (char *)&nf, (ftnlen)sizeof(integer));
    e_wsfe();
    goto L15;
L30:
    s_wsfe(&io___16);
    do_fio(&c__1, " Enter window type (1=Rectangular, 2=Triangular,", 48L);
    do_fio(&c__1, "3=Hamming, 4=Gen. Hamming, 5=Hanning, 6=Kaiser, 7=Chebysh\
ev): ", 62L);
    e_wsfe();
    io___17.ciunit = incod;
    s_rsle(&io___17);
    do_lio(&c__3, &c__1, (char *)&itype, (ftnlen)sizeof(integer));
    e_rsle();
/* L35: */
    if (itype != 7 && nf < 3) {
	goto L20;
    }
    if (itype == 7 && (nf == 1 || nf == 2)) {
	goto L20;
    }
    s_wsfe(&io___19);
    do_fio(&c__1, " Enter filter type (1=Lowpass, 2=Highpass,", 42L);
    do_fio(&c__1, "3=Bandpass, 4=Bandstop): ", 25L);
    e_wsfe();
    io___20.ciunit = incod;
    s_rsle(&io___20);
    do_lio(&c__3, &c__1, (char *)&jtype, (ftnlen)sizeof(integer));
    e_rsle();

/* n is half the length of the symmetric filter */

    n = (nf + 1) / 2;
    if (jtype != 1 && jtype != 2) {
	goto L50;
    }

/* for the ideal lowpass or highpass design - input fc */

L40:
    s_wsfe(&io___23);
    do_fio(&c__1, " Enter cutoff frequency (as a fraction of sample frequenc\
y): ", 61L);
    e_wsfe();
    io___24.ciunit = incod;
    s_rsle(&io___24);
    do_lio(&c__4, &c__1, (char *)&fc, (ftnlen)sizeof(real));
    e_rsle();
    if (fc > (float)0. && fc < (float).5) {
	goto L60;
    }
    io___26.ciunit = otcd1;
    s_wsfe(&io___26);
    do_fio(&c__1, (char *)&fc, (ftnlen)sizeof(real));
    e_wsfe();
    goto L40;

/* for the ideal bandpass or bandstop design - input fl and fh */

L50:
    s_wsfe(&io___27);
    do_fio(&c__1, " Enter lower cutoff frequency (as a fraction of sample fr\
equency): ", 67L);
    e_wsfe();
    io___28.ciunit = incod;
    s_rsle(&io___28);
    do_lio(&c__4, &c__1, (char *)&fl, (ftnlen)sizeof(real));
    e_rsle();
    s_wsfe(&io___30);
    do_fio(&c__1, " Enter upper cutoff frequency (as a fraction of sample fr\
equency): ", 67L);
    e_wsfe();
    io___31.ciunit = incod;
    s_rsle(&io___31);
    do_lio(&c__4, &c__1, (char *)&fh, (ftnlen)sizeof(real));
    e_rsle();
    if (fl > (float)0. && fl < (float).5 && fh > (float)0. && fh < (float).5 
	    && fh > fl) {
	goto L60;
    }
    if (fl < (float)0. || fl > (float).5) {
	io___33.ciunit = otcd1;
	s_wsfe(&io___33);
	do_fio(&c__1, (char *)&fl, (ftnlen)sizeof(real));
	e_wsfe();
    }
    if (fh < (float)0. || fh > (float).5) {
	io___34.ciunit = otcd1;
	s_wsfe(&io___34);
	do_fio(&c__1, (char *)&fh, (ftnlen)sizeof(real));
	e_wsfe();
    }
    if (fh < fl) {
	io___35.ciunit = otcd1;
	s_wsfe(&io___35);
	do_fio(&c__1, (char *)&fh, (ftnlen)sizeof(real));
	do_fio(&c__1, (char *)&fl, (ftnlen)sizeof(real));
	e_wsfe();
    }
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

    s_wsfe(&io___36);
    do_fio(&c__1, " Enter chebyshev ripple (in db), and optional transition \
width,", 63L);
    e_wsfe();
    s_wsfe(&io___37);
    do_fio(&c__1, " separated by a comma, using decimal points: ", 45L);
    e_wsfe();
    io___38.ciunit = incod;
    s_rsfe(&io___38);
    do_fio(&c__1, (char *)&dplog, (ftnlen)sizeof(real));
    do_fio(&c__1, (char *)&df, (ftnlen)sizeof(real));
    e_rsfe();
    d__1 = (doublereal) (-(doublereal)dplog / (float)20.);
    dp = pow_dd(&c_b81, &d__1);
    chebc_(&nf, &dp, &df, &n, &x0, &xn);

/* ieo is an even, odd indicator, ieo = 0 for even, ieo = 1 for odd */

L70:
    ieo = nf % 2;

/*   The following seemed wrong: had 1 instead of 4.  Even length */
/*   lowpass filters could result in the original. */

    if (ieo == 1 || jtype == 4 || jtype == 3) {
	goto L80;
    }
    s_wsfe(&io___45);
    do_fio(&c__1, " Order must be odd for highpass of lowpass filters -- bei\
ng increased by 1. ", 76L);
    e_wsfe();
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
	g[0] = c1 * (float)2.;
    }
    i1 = ieo + 1;
    i__1 = n;
    for (i = i1; i <= i__1; ++i) {
	xn = (real) (i - 1);
	if (ieo == 0) {
	    xn += (float).5;
	}
	c = pi * xn;
	c3 = c * c1;
	if (jtype == 1 || jtype == 2) {
	    c3 *= (float)2.;
	}
	g[i - 1] = sin(c3) / c;
	if (jtype == 3 || jtype == 4) {
	    g[i - 1] = g[i - 1] * (float)2. * cos(c * (fl + fh));
	}
/* L90: */
    }

/* compute a rectangular window */

    if (itype == 1) {
	io___52.ciunit = otcd2;
	s_wsfe(&io___52);
	do_fio(&c__1, (char *)&nf, (ftnlen)sizeof(integer));
	e_wsfe();
    }
    i__1 = n;
    for (i = 1; i <= i__1; ++i) {
	w[i - 1] = (float)1.;
/* L100: */
    }

/* dispatch on window type */

    switch ((int)itype) {
	case 1:  goto L200;
	case 2:  goto L110;
	case 3:  goto L120;
	case 4:  goto L140;
	case 5:  goto L150;
	case 6:  goto L160;
	case 7:  goto L170;
    }

/* triangular window */

L110:
    triang_(&nf, w, &n, &ieo);
    io___54.ciunit = otcd2;
    s_wsfe(&io___54);
    do_fio(&c__1, (char *)&nf, (ftnlen)sizeof(integer));
    e_wsfe();
    goto L180;

/* hamming window */

L120:
    alpha = (float).54;
    io___56.ciunit = otcd2;
    s_wsfe(&io___56);
    do_fio(&c__1, (char *)&nf, (ftnlen)sizeof(integer));
    e_wsfe();
L130:
    beta = (float)1. - alpha;
    hammin_(&nf, w, &n, &ieo, &alpha, &beta);
    io___58.ciunit = otcd2;
    s_wsfe(&io___58);
    do_fio(&c__1, (char *)&alpha, (ftnlen)sizeof(real));
    e_wsfe();
    goto L180;

/* generalized hamming window */
/* form of window is w(m)=alpha+beta*cos((twopi*m)/(nf-1)) */
/* beta is automatically set to 1.-alpha */
/* read in alpha */

L140:
    io___59.ciunit = otcd1;
    s_wsfe(&io___59);
    e_wsfe();
    io___60.ciunit = incod;
    s_rsfe(&io___60);
    do_fio(&c__1, (char *)&alpha, (ftnlen)sizeof(real));
    e_rsfe();
    io___61.ciunit = otcd2;
    s_wsfe(&io___61);
    do_fio(&c__1, (char *)&nf, (ftnlen)sizeof(integer));
    e_wsfe();
    goto L130;

/* hanning window */

L150:
    alpha = (float).5;
    io___62.ciunit = otcd2;
    s_wsfe(&io___62);
    do_fio(&c__1, (char *)&nf, (ftnlen)sizeof(integer));
    e_wsfe();

/* increase nf by 2 and n by 1 for hanning window so zero */
/* endpoints are not part of window */

    nf += 2;
    ++n;
    goto L130;

/* kaiser (i0-sinh) window */
/* need to specify parameter att=stopband attenuation in db */

L160:
    io___63.ciunit = otcd1;
    s_wsfe(&io___63);
    e_wsfe();
    io___64.ciunit = incod;
    s_rsfe(&io___64);
    do_fio(&c__1, (char *)&att, (ftnlen)sizeof(real));
    e_rsfe();
    if (att > (float)50.) {
	beta = (att - (float)8.7) * (float).1102;
    }
    if (att >= (float)20.96 && att <= (float)50.) {
	d__1 = (doublereal) (att - (float)20.96);
	beta = pow_dd(&d__1, &c_b121) * (float).58417 + (att - (float)20.96) *
		 (float).07886;
    }
    if (att < (float)20.96) {
	beta = (float)0.;
    }
    kaiser_(&nf, w, &n, &ieo, &beta);
    io___66.ciunit = otcd2;
    s_wsfe(&io___66);
    do_fio(&c__1, (char *)&nf, (ftnlen)sizeof(integer));
    e_wsfe();
    io___67.ciunit = otcd2;
    s_wsfe(&io___67);
    do_fio(&c__1, (char *)&att, (ftnlen)sizeof(real));
    do_fio(&c__1, (char *)&beta, (ftnlen)sizeof(real));
    e_wsfe();
    goto L180;

/* chebyshev window */

L170:
    cheby_(&nf, w, &n, &ieo, &dp, &df, &x0, &xn);
    io___68.ciunit = otcd2;
    s_wsfe(&io___68);
    do_fio(&c__1, (char *)&nf, (ftnlen)sizeof(integer));
    e_wsfe();
    io___69.ciunit = otcd2;
    s_wsfe(&io___69);
    do_fio(&c__1, (char *)&dp, (ftnlen)sizeof(real));
    do_fio(&c__1, (char *)&df, (ftnlen)sizeof(real));
    e_wsfe();

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
    s_wsfe(&io___70);
    do_fio(&c__1, " Print window values? (y/n): ", 29L);
    e_wsfe();
    s_rsfe(&io___71);
    do_fio(&c__1, answer, 1L);
    e_rsfe();
    if (*answer == 'n' || *answer == 'N') {
	goto L220;
    }
    io___73.ciunit = otcd2;
    s_wsfe(&io___73);
    e_wsfe();
    i__1 = n;
    for (i = 1; i <= i__1; ++i) {
	j = n + 1 - i;
	k = nf + 1 - i;
	io___76.ciunit = otcd2;
	s_wsfe(&io___76);
	do_fio(&c__1, (char *)&i, (ftnlen)sizeof(integer));
	do_fio(&c__1, (char *)&w[j - 1], (ftnlen)sizeof(real));
	do_fio(&c__1, (char *)&k, (ftnlen)sizeof(integer));
	e_wsfe();
/* L210: */
    }
L220:
    if (jtype == 1) {
	io___77.ciunit = otcd2;
	s_wsfe(&io___77);
	e_wsfe();
    }
    if (jtype == 2) {
	io___78.ciunit = otcd2;
	s_wsfe(&io___78);
	e_wsfe();
    }
    if (jtype == 3) {
	io___79.ciunit = otcd2;
	s_wsfe(&io___79);
	e_wsfe();
    }
    if (jtype == 4) {
	io___80.ciunit = otcd2;
	s_wsfe(&io___80);
	e_wsfe();
    }
    if (jtype == 1) {
	io___81.ciunit = otcd2;
	s_wsfe(&io___81);
	do_fio(&c__1, (char *)&fc, (ftnlen)sizeof(real));
	e_wsfe();
    }
    if (jtype == 2) {
	io___82.ciunit = otcd2;
	s_wsfe(&io___82);
	do_fio(&c__1, (char *)&fc, (ftnlen)sizeof(real));
	e_wsfe();
    }
    if (jtype == 3 || jtype == 4) {
	io___83.ciunit = otcd2;
	s_wsfe(&io___83);
	do_fio(&c__1, (char *)&fl, (ftnlen)sizeof(real));
	do_fio(&c__1, (char *)&fh, (ftnlen)sizeof(real));
	e_wsfe();
    }
    if (jtype == 1 || jtype == 3) {
	goto L240;
    }
    i__1 = n;
    for (i = 2; i <= i__1; ++i) {
	g[i - 1] = -(doublereal)g[i - 1];
/* L230: */
    }
    g[0] = (float)1. - g[0];

/* write out impulse response */



L240:
    i__1 = n;
    for (i = 1; i <= i__1; ++i) {
	j = n + 1 - i;
	k = nf + 1 - i;
	io___84.ciunit = otcd2;
	s_wsfe(&io___84);
	do_fio(&c__1, (char *)&i, (ftnlen)sizeof(integer));
	do_fio(&c__1, (char *)&g[j - 1], (ftnlen)sizeof(real));
	do_fio(&c__1, (char *)&k, (ftnlen)sizeof(integer));
	e_wsfe();
/* L250: */
    }
    flchar_(&nf, &itype, &jtype, &fc, &fl, &fh, &n, &ieo, g, &otcd2);
    io___85.ciunit = otcd2;
    s_wsfe(&io___85);
    e_wsfe();

/*   Open output file */

    ioutd = 1;
    s_wsfe(&io___87);
    do_fio(&c__1, " Enter name of window values output file or <enter> ", 52L)
	    ;
    e_wsfe();
    s_wsfe(&io___88);
    do_fio(&c__1, " (Sorry, no tilde-expansion.  Give path relative to your \
home directory) ", 73L);
    e_wsfe();
    io___89.ciunit = incod;
    s_rsfe(&io___89);
    do_fio(&c__1, fname, 100L);
    e_rsfe();
    if (s_cmp(fname, " ", 100L, 1L) == 0) {
	goto L260;
    }
    o__1.oerr = 0;
    o__1.ounit = ioutd;
    o__1.ofnmlen = 100;
    o__1.ofnm = fname;
    o__1.orl = 0;
    o__1.osta = 0;
    o__1.oacc = 0;
    o__1.ofm = 0;
    o__1.oblnk = 0;
    f_open(&o__1);
    outimp_(g, &n, &nf, &ieo, &ioutd);
    flush_(&ioutd);
    cl__1.cerr = 0;
    cl__1.cunit = ioutd;
    cl__1.csta = 0;
    f_clos(&cl__1);
L260:
    cl__1.cerr = 0;
    cl__1.cunit = 3;
    cl__1.csta = 0;
    f_clos(&cl__1);
    s_wsfe(&io___90);
    do_fio(&c__1, " Another design? (y/n): ", 24L);
    e_wsfe();
    s_rsfe(&io___91);
    do_fio(&c__1, answer, 1L);
    e_rsfe();
    if (*answer == 'y' || *answer == 'Y') {
	goto L10;
    }
    s_stop("End of program", 14L);
} /* MAIN__ */


/* ----------------------------------------------------------------------- */
/* subroutine:  triang */
/* triangular window */
/* ----------------------------------------------------------------------- */

/* Subroutine */ int triang_(nf, w, n, ieo)
integer *nf;
real *w;
integer *n, *ieo;
{
    /* System generated locals */
    integer i__1;

    /* Local variables */
    static integer i;
    static real fn, xi;


/*  nf = filter length in samples */
/*   w = window coefficients for half the window */
/*   n = half window length=(nf+1)/2 */
/* ieo = even - odd indication--ieo=0 for nf even */

    /* Parameter adjustments */
    --w;

    /* Function Body */
    fn = (real) (*n);
    i__1 = *n;
    for (i = 1; i <= i__1; ++i) {
	xi = (real) (i - 1);
	if (*ieo == 0) {
	    xi += (float).5;
	}
	w[i] = (float)1. - xi / fn;
/* L10: */
    }
    return 0;
} /* triang_ */


/* ----------------------------------------------------------------------- */
/* subroutine:  hammin */
/* generalized hamming window routine */
/* window is w(n) = alpha + beta * cos( twopi*(n-1) / (nf-1) ) */
/* ----------------------------------------------------------------------- */

/* Subroutine */ int hammin_(nf, w, n, ieo, alpha, beta)
integer *nf;
real *w;
integer *n, *ieo;
real *alpha, *beta;
{
    /* System generated locals */
    integer i__1;

    /* Builtin functions */
    double atan(), cos();

    /* Local variables */
    static integer i;
    static real fi, fn, pi2;


/*    nf = filter length in samples */
/*     w = window array of size n */
/*     n = half length of filter=(nf+1)/2 */
/*   ieo = even odd indicator--ieo=0 if nf even */
/* alpha = constant of window */
/*  beta = constant of window--generally beta=1-alpha */

    /* Parameter adjustments */
    --w;

    /* Function Body */
    pi2 = atan((float)1.) * (float)8.;
    fn = (real) (*nf - 1);
    i__1 = *n;
    for (i = 1; i <= i__1; ++i) {
	fi = (real) (i - 1);
	if (*ieo == 0) {
	    fi += (float).5;
	}
	w[i] = *alpha + *beta * cos(pi2 * fi / fn);
/* L10: */
    }
    return 0;
} /* hammin_ */


/* ----------------------------------------------------------------------- */
/* subroutine:  kaiser */
/* kaiser window */
/* ----------------------------------------------------------------------- */

/* Subroutine */ int kaiser_(nf, w, n, ieo, beta)
integer *nf;
real *w;
integer *n, *ieo;
real *beta;
{
    /* System generated locals */
    integer i__1;
    real r__1;

    /* Builtin functions */
    double sqrt();

    /* Local variables */
    static real xind;
    static integer i;
    static real xi, bes;
    extern doublereal ino_();


/*   nf = filter length in samples */
/*    w = window array of size n */
/*    n = filter half length=(nf+1)/2 */
/*  ieo = even odd indicator--ieo=0 if nf even */
/* beta = parameter of kaiser window */

    /* Parameter adjustments */
    --w;

    /* Function Body */
    bes = ino_(beta);
    xind = (real) (*nf - 1) * (real) (*nf - 1);
    i__1 = *n;
    for (i = 1; i <= i__1; ++i) {
	xi = (real) (i - 1);
	if (*ieo == 0) {
	    xi += (float).5;
	}
	xi = xi * (float)4. * xi;
	r__1 = *beta * sqrt((float)1. - xi / xind);
	w[i] = ino_(&r__1);
	w[i] /= bes;
/* L10: */
    }
    return 0;
} /* kaiser_ */


/* ----------------------------------------------------------------------- */
/* function:  ino */
/* bessel function for kaiser window */
/* ----------------------------------------------------------------------- */

doublereal ino_(x)
real *x;
{
    /* System generated locals */
    real ret_val;

    /* Local variables */
    static real e;
    static integer i;
    static real t, y, de, xi, sde;

    y = *x / (float)2.;
    t = (float)1e-8;
    e = (float)1.;
    de = (float)1.;
    for (i = 1; i <= 25; ++i) {
	xi = (real) i;
	de = de * y / xi;
	sde = de * de;
	e += sde;
	if (e * t - sde <= (float)0.) {
	    goto L10;
	} else {
	    goto L20;
	}
L10:
	;
    }
L20:
    ret_val = e;
    return ret_val;
} /* ino_ */


/* ----------------------------------------------------------------------- */
/* subroutine:  chebc */
/* subroutine to generate chebyshev window parameters when */
/* one of the three parameters nf,dp and df is unspecified */
/* ----------------------------------------------------------------------- */

/* Subroutine */ int chebc_(nf, dp, df, n, x0, xn)
integer *nf;
real *dp, *df;
integer *n;
real *x0, *xn;
{
    /* System generated locals */
    real r__1;

    /* Builtin functions */
    double atan(), cos(), cosh();

    /* Local variables */
    static real x, c0, c1, c2, pi;
    extern doublereal arccos_(), coshin_();


/* nf = filter length (in samples) */
/* dp = filter ripple (absolute scale) */
/* df = normalized transition width of filter */
/*  n = (nf+1)/2 = filter half length */
/* x0 = (3-c0)/(1+c0) with c0=cos(pi*df) = chebyshev window constant */
/* xn = nf-1 */

    pi = atan((float)1.) * (float)4.;
    if (*nf != 0) {
	goto L10;
    }

/* dp,df specified, determine nf */

    r__1 = (*dp + (float)1.) / *dp;
    c1 = coshin_(&r__1);
    c0 = cos(pi * *df);
    r__1 = (float)1. / c0;
    x = c1 / coshin_(&r__1) + (float)1.;

/* increment by 1 to give nf which meets or exceeds specs on dp and df */

    *nf = x + (float)1.;
    *n = (*nf + 1) / 2;
    *xn = (real) (*nf - 1);
    goto L30;
L10:
    if (*df != (float)0.) {
	goto L20;
    }

/* nf,dp specified, determine df */

    *xn = (real) (*nf - 1);
    r__1 = (*dp + (float)1.) / *dp;
    c1 = coshin_(&r__1);
    c2 = cosh(c1 / *xn);
    r__1 = (float)1. / c2;
    *df = arccos_(&r__1) / pi;
    goto L30;

/* nf,df specified, determine dp */

L20:
    *xn = (real) (*nf - 1);
    c0 = cos(pi * *df);
    r__1 = (float)1. / c0;
    c1 = *xn * coshin_(&r__1);
    *dp = (float)1. / (cosh(c1) - (float)1.);
L30:
    *x0 = ((float)3. - cos(pi * (float)2. * *df)) / (cos(pi * (float)2. * *df)
	     + (float)1.);
    return 0;
} /* chebc_ */


/* ----------------------------------------------------------------------- */
/* subroutine:  cheby */
/* dolph chebyshev window design */
/* ----------------------------------------------------------------------- */

/* Subroutine */ int cheby_(nf, w, n, ieo, dp, df, x0, xn)
integer *nf;
real *w;
integer *n, *ieo;
real *dp, *df, *x0, *xn;
{
    /* System generated locals */
    integer i__1, i__2;

    /* Builtin functions */
    double atan(), cos(), cosh(), sin();

    /* Local variables */
    static real beta, f;
    static integer i, j;
    static real p, alpha, x, c1, c2, twopi, pi[1024], xi, pr[1024], xj;
    extern doublereal arccos_(), coshin_();
    static real fnf, pie, sum, twn;


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
    pie = atan((float)1.) * (float)4.;
    *xn = (real) (*nf - 1);
    fnf = (real) (*nf);
    alpha = (*x0 + (float)1.) / (float)2.;
    beta = (*x0 - (float)1.) / (float)2.;
    twopi = pie * (float)2.;
    c2 = *xn / (float)2.;
    i__1 = *nf;
    for (i = 1; i <= i__1; ++i) {
	xi = (real) (i - 1);
	f = xi / fnf;
	x = alpha * cos(twopi * f) + beta;
	if (dabs(x) - (float)1. <= (float)0.) {
	    goto L10;
	} else {
	    goto L20;
	}
L10:
	p = *dp * cos(c2 * arccos_(&x));
	goto L30;
L20:
	p = *dp * cosh(c2 * coshin_(&x));
L30:
	pi[i - 1] = (float)0.;
	pr[i - 1] = p;

/* for even length filters use a one-half sample delay */
/* also the frequency response is antisymmetric in frequency */

	if (*ieo == 1) {
	    goto L40;
	}
	pr[i - 1] = p * cos(pie * f);
	pi[i - 1] = -(doublereal)p * sin(pie * f);
	if (i > *nf / 2 + 1) {
	    pr[i - 1] = -(doublereal)pr[i - 1];
	}
	if (i > *nf / 2 + 1) {
	    pi[i - 1] = -(doublereal)pi[i - 1];
	}
L40:
	;
    }

/* use dft to give window */

    twn = twopi / fnf;
    i__1 = *n;
    for (i = 1; i <= i__1; ++i) {
	xi = (real) (i - 1);
	sum = (float)0.;
	i__2 = *nf;
	for (j = 1; j <= i__2; ++j) {
	    xj = (real) (j - 1);
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
} /* cheby_ */


/* ----------------------------------------------------------------------- */
/* function:  coshin */
/* function for hyperbolic inverse cosine of x */
/* ----------------------------------------------------------------------- */

doublereal coshin_(x)
real *x;
{
    /* System generated locals */
    real ret_val;

    /* Builtin functions */
    double sqrt(), log();

    ret_val = log(*x + sqrt(*x * *x - (float)1.));
    return ret_val;
} /* coshin_ */


/* ----------------------------------------------------------------------- */
/* function:  arccos */
/* function for inverse cosine of x */
/* ----------------------------------------------------------------------- */

doublereal arccos_(x)
real *x;
{
    /* System generated locals */
    real ret_val;

    /* Builtin functions */
    double sqrt(), atan();

    /* Local variables */
    static real a;

    if (*x < (float)0.) {
	goto L30;
    } else if (*x == 0) {
	goto L20;
    } else {
	goto L10;
    }
L10:
    a = sqrt((float)1. - *x * *x) / *x;
    ret_val = atan(a);
    return ret_val;
L20:
    ret_val = atan((float)1.) * (float)2.;
    return ret_val;
L30:
    a = sqrt((float)1. - *x * *x) / *x;
    ret_val = atan(a) + atan((float)1.) * (float)4.;
    return ret_val;
} /* arccos_ */


/* ----------------------------------------------------------------------- */
/* function:  cosh */
/* function for hyperbolic cosine of x */
/* ----------------------------------------------------------------------- */

doublereal cosh_(x)
real *x;
{
    /* System generated locals */
    real ret_val;

    /* Builtin functions */
    double exp();

    ret_val = (exp(*x) + exp(-(doublereal)(*x))) / (float)2.;
    return ret_val;
} /* cosh_ */


/* ----------------------------------------------------------------------- */
/* subroutine:  flchar */
/* subroutine to determine filter characteristics */
/* ----------------------------------------------------------------------- */

/* Subroutine */ int flchar_(nf, itype, jtype, fc, fl, fh, n, ieo, g, otcd2)
integer *nf, *itype, *jtype;
real *fc, *fl, *fh;
integer *n, *ieo;
real *g;
integer *otcd2;
{
    /* Format strings */
    static char fmt_9999[] = "(\002 passband cutoff \002,f6.4,\002  ripple\
 \002,f8.3,\002 db\002)";
    static char fmt_9998[] = "(\002 stopband cutoff \002,f6.4,\002  ripple\
 \002,f8.3,\002 db\002)";
    static char fmt_9997[] = "(\002 passband cutoffs \002,f6.4,2x,f6.4,\002 \
 ripple\002,f9.3,\002 db\002)";
    static char fmt_9996[] = "(\002 stopband cutoffs \002,f6.4,2x,f6.4,\002 \
 ripple\002,f9.3,\002 db\002)";

    /* System generated locals */
    integer i__1, i__2;

    /* Builtin functions */
    double atan(), cos();
    integer s_wsfe(), do_fio(), e_wsfe();

    /* Local variables */
    static real resp[2048], sumi, twni;
    static integer i, j;
    static real f1, f2, db, pi;
    static integer nr;
    static real xi, xj;
    extern /* Subroutine */ int ripple_();
    static real sum, xnr, twn;

    /* Fortran I/O blocks */
    static cilist io___149 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___150 = { 0, 0, 0, fmt_9998, 0 };
    static cilist io___151 = { 0, 0, 0, fmt_9998, 0 };
    static cilist io___152 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___153 = { 0, 0, 0, fmt_9998, 0 };
    static cilist io___154 = { 0, 0, 0, fmt_9997, 0 };
    static cilist io___155 = { 0, 0, 0, fmt_9998, 0 };
    static cilist io___156 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___157 = { 0, 0, 0, fmt_9996, 0 };
    static cilist io___158 = { 0, 0, 0, fmt_9999, 0 };



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

    pi = atan((float)1.) * (float)4.;

/* up to 4096 pt dft */

    nr = *nf << 3;
    if (nr > 2048) {
	nr = 2048;
    }
    xnr = (real) nr;
    twn = pi / xnr;
    sumi = -(doublereal)g[1] / (float)2.;
    if (*ieo == 0) {
	sumi = (float)0.;
    }
    i__1 = nr;
    for (i = 1; i <= i__1; ++i) {
	xi = (real) (i - 1);
	twni = twn * xi;
	sum = sumi;
	i__2 = *n;
	for (j = 1; j <= i__2; ++j) {
	    xj = (real) (j - 1);
	    if (*ieo == 0) {
		xj += (float).5;
	    }
	    sum += g[j] * cos(xj * twni);
/* L10: */
	}
	resp[i - 1] = sum * (float)2.;
/* L20: */
    }

/* dispatch on filter type */

    switch ((int)*jtype) {
	case 1:  goto L30;
	case 2:  goto L40;
	case 3:  goto L50;
	case 4:  goto L60;
    }

/* lowpass */

L30:
    ripple_(&nr, &c_b233, &c_b234, fc, resp, &f1, &f2, &db);
    io___149.ciunit = *otcd2;
    s_wsfe(&io___149);
    do_fio(&c__1, (char *)&f2, (ftnlen)sizeof(real));
    do_fio(&c__1, (char *)&db, (ftnlen)sizeof(real));
    e_wsfe();
    ripple_(&nr, &c_b234, fc, &c_b240, resp, &f1, &f2, &db);
    io___150.ciunit = *otcd2;
    s_wsfe(&io___150);
    do_fio(&c__1, (char *)&f1, (ftnlen)sizeof(real));
    do_fio(&c__1, (char *)&db, (ftnlen)sizeof(real));
    e_wsfe();
    return 0;

/* highpass */

L40:
    ripple_(&nr, &c_b234, &c_b234, fc, resp, &f1, &f2, &db);
    io___151.ciunit = *otcd2;
    s_wsfe(&io___151);
    do_fio(&c__1, (char *)&f2, (ftnlen)sizeof(real));
    do_fio(&c__1, (char *)&db, (ftnlen)sizeof(real));
    e_wsfe();
    ripple_(&nr, &c_b233, fc, &c_b240, resp, &f1, &f2, &db);
    io___152.ciunit = *otcd2;
    s_wsfe(&io___152);
    do_fio(&c__1, (char *)&f1, (ftnlen)sizeof(real));
    do_fio(&c__1, (char *)&db, (ftnlen)sizeof(real));
    e_wsfe();
    return 0;

/* bandpass */

L50:
    ripple_(&nr, &c_b234, &c_b234, fl, resp, &f1, &f2, &db);
    io___153.ciunit = *otcd2;
    s_wsfe(&io___153);
    do_fio(&c__1, (char *)&f2, (ftnlen)sizeof(real));
    do_fio(&c__1, (char *)&db, (ftnlen)sizeof(real));
    e_wsfe();
    ripple_(&nr, &c_b233, fl, fh, resp, &f1, &f2, &db);
    io___154.ciunit = *otcd2;
    s_wsfe(&io___154);
    do_fio(&c__1, (char *)&f1, (ftnlen)sizeof(real));
    do_fio(&c__1, (char *)&f2, (ftnlen)sizeof(real));
    do_fio(&c__1, (char *)&db, (ftnlen)sizeof(real));
    e_wsfe();
    ripple_(&nr, &c_b234, fh, &c_b240, resp, &f1, &f2, &db);
    io___155.ciunit = *otcd2;
    s_wsfe(&io___155);
    do_fio(&c__1, (char *)&f1, (ftnlen)sizeof(real));
    do_fio(&c__1, (char *)&db, (ftnlen)sizeof(real));
    e_wsfe();
    return 0;

/* stopband */

L60:
    ripple_(&nr, &c_b233, &c_b234, fl, resp, &f1, &f2, &db);
    io___156.ciunit = *otcd2;
    s_wsfe(&io___156);
    do_fio(&c__1, (char *)&f2, (ftnlen)sizeof(real));
    do_fio(&c__1, (char *)&db, (ftnlen)sizeof(real));
    e_wsfe();
    ripple_(&nr, &c_b234, fl, fh, resp, &f1, &f2, &db);
    io___157.ciunit = *otcd2;
    s_wsfe(&io___157);
    do_fio(&c__1, (char *)&f1, (ftnlen)sizeof(real));
    do_fio(&c__1, (char *)&f2, (ftnlen)sizeof(real));
    do_fio(&c__1, (char *)&db, (ftnlen)sizeof(real));
    e_wsfe();
    ripple_(&nr, &c_b233, fh, &c_b240, resp, &f1, &f2, &db);
    io___158.ciunit = *otcd2;
    s_wsfe(&io___158);
    do_fio(&c__1, (char *)&f1, (ftnlen)sizeof(real));
    do_fio(&c__1, (char *)&db, (ftnlen)sizeof(real));
    e_wsfe();
    return 0;
} /* flchar_ */


/* ----------------------------------------------------------------------- */
/* subroutine:  ripple */
/* finds largest ripple in band and locates band edges based on the */
/* point where the transition region crosses the measured ripple bound */
/* ----------------------------------------------------------------------- */

/* Subroutine */ int ripple_(nr, rideal, flow, fhi, resp, f1, f2, db)
integer *nr;
real *rideal, *flow, *fhi, *resp, *f1, *f2, *db;
{
    /* System generated locals */
    integer i__1;
    real r__1, r__2;

    /* Builtin functions */
    double r_lg10();

    /* Local variables */
    static integer ifhi;
    static real rmin, ripl, rmax;
    static integer i, j, iflow;
    static real x0, x1, y1, y0, xi, xnr;


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
    xnr = (real) (*nr);

/* band limits */

    iflow = xnr * (float)2. * *flow + (float)1.5;
    ifhi = xnr * (float)2. * *fhi + (float)1.5;
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
	if (resp[i] <= rmax || resp[i] < resp[i - 1] || resp[i] < resp[i + 1])
		 {
	    goto L10;
	}
	rmax = resp[i];
L10:
	if (resp[i] >= rmin || resp[i] > resp[i - 1] || resp[i] > resp[i + 1])
		 {
	    goto L20;
	}
	rmin = resp[i];
L20:
	;
    }

/* peak deviation from ideal */

/* Computing MAX */
    r__1 = rmax - *rideal, r__2 = *rideal - rmin;
    ripl = dmax(r__1,r__2);

/* search for lower band edge */

    *f1 = *flow;
    if (*flow == (float)0.) {
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
    xi = (real) (i - 1);

/* linear interpolation of band edge frequency to improve accuracy */

    x1 = xi * (float).5 / xnr;
    x0 = (xi - (float)1.) * (float).5 / xnr;
    y1 = (r__1 = resp[i] - *rideal, dabs(r__1));
    y0 = (r__1 = resp[i - 1] - *rideal, dabs(r__1));
    *f1 = (x1 - x0) / (y1 - y0) * (ripl - y0) + x0;

/* search for upper band edge */

L50:
    *f2 = *fhi;
    if (*fhi == (float).5) {
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
    xi = (real) (j - 1);

/* linear interpolation of band edge frequency to improve accuracy */

    x1 = xi * (float).5 / xnr;
    x0 = (xi + (float)1.) * (float).5 / xnr;
    y1 = (r__1 = resp[j] - *rideal, dabs(r__1));
    y0 = (r__1 = resp[j + 1] - *rideal, dabs(r__1));
    *f2 = (x1 - x0) / (y1 - y0) * (ripl - y0) + x0;

/* deviation from ideal in db */

L80:
    r__1 = ripl + *rideal;
    *db = r_lg10(&r__1) * (float)20.;
    return 0;
} /* ripple_ */


/* ----------------------------------------------------------------------- */

/* ----------------------------------------------------------------------- */
/* subroutine: outimp */
/*    Prints out on unit ioutd the full */
/*    impulse response as format 13x,f15.8.  This makes the impulse */
/*    response accessible to other programs without the need for parsing */
/*    the output of the program. */
/* ----------------------------------------------------------------------- */

/* Subroutine */ int outimp_(g, n, nf, ieo, ioutd)
real *g;
integer *n, *nf, *ieo, *ioutd;
{
    /* Format strings */
    static char fmt_50[] = "(e15.8)";

    /* System generated locals */
    integer i__1;
    cllist cl__1;

    /* Builtin functions */
    integer s_wsfe(), do_fio(), e_wsfe(), f_clos();

    /* Local variables */
    static integer i, j, l;

    /* Fortran I/O blocks */
    static cilist io___175 = { 0, 0, 0, fmt_50, 0 };
    static cilist io___176 = { 0, 0, 0, fmt_50, 0 };
    static cilist io___177 = { 0, 0, 0, fmt_50, 0 };



/* 	Removed filter order output for Gabriel compatibility */

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
	io___175.ciunit = *ioutd;
	s_wsfe(&io___175);
	do_fio(&c__1, (char *)&g[j], (ftnlen)sizeof(real));
	e_wsfe();
/* L20: */
    }
    if (*ieo == 0) {
	goto L30;
    }
    io___176.ciunit = *ioutd;
    s_wsfe(&io___176);
    do_fio(&c__1, (char *)&g[1], (ftnlen)sizeof(real));
    e_wsfe();
L30:
    for (i = l; i >= 1; --i) {
	j = *n + 1 - i;
	io___177.ciunit = *ioutd;
	s_wsfe(&io___177);
	do_fio(&c__1, (char *)&g[j], (ftnlen)sizeof(real));
	e_wsfe();
/* L40: */
    }
    cl__1.cerr = 0;
    cl__1.cunit = *ioutd;
    cl__1.csta = 0;
    f_clos(&cl__1);
/* L60: */
    return 0;
} /* outimp_ */

