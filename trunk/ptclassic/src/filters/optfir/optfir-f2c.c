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
/*  -- translated by f2c (version of 23 April 1993  18:34:30).
   You must link the resulting object file with the libraries:
	-lf2c -lm   (in that order)
*/

#include "f2c.h"

/* Common Block Declarations */

struct {
    real des[1600], wt[1600], alpha[100];
    integer iext[100], nfcns, ngrid;
    doublereal pi2, ad[100], dev, x[100], y[100];
    real grid[1600];
} _BLNK__;

#define _BLNK__1 _BLNK__

/* Table of constant values */

static integer c__1 = 1;
static integer c__3 = 3;
static integer c__4 = 4;
static integer c__5 = 5;
static doublereal c_b106 = 10.;
static real c_b169 = 0.f;
static real c_b219 = .5f;
static integer c__9 = 9;

/*<       Program Optfir >*/
/* Main program */ MAIN__(void)
{
    /* Format strings */
    static char fmt_360[] = "(/\002 Finite Impulse Response (FIR)\002/,\002 "
	    "Linear Phase Digital Filter Design\002/,\002 Remez Exchange Algo"
	    "rithm\002/)";
    static char fmt_365[] = "(\002 Bandpass Filter\002/)";
    static char fmt_370[] = "(\002 Differentiator\002/)";
    static char fmt_375[] = "(\002 Hilbert Transformer\002/)";
    static char fmt_376[] = "(\002 Half-band Filter\002/)";
    static char fmt_378[] = "(\002 Filter length = \002,i3/)";
    static char fmt_379[] = "(\002 Filter length determined by approximatio"
	    "n\002/)";
    static char fmt_380[] = "(\002 Impulse Response Coefficients:\002/)";
    static char fmt_382[] = "(10x,\002h(\002,i3,\002) = \002,e14.7,\002 = h"
	    "(\002,i4,\002)\002)";
    static char fmt_383[] = "(10x,\002h(\002,i3,\002) = \002,e14.7,\002 = "
	    "-h(\002,i4,\002)\002)";
    static char fmt_384[] = "(10x,\002h(\002,i3,\002) = \002,e14.7)";
    static char fmt_385[] = "(25x,4(\002Band\002,i3,8x))";
    static char fmt_390[] = "(/\002 Lower band edge:\002,5f15.7)";
    static char fmt_395[] = "(\002 Upper band edge:\002,5f15.7)";
    static char fmt_400[] = "(\002 Desired value:\002,2x,5f15.7)";
    static char fmt_405[] = "(\002 Desired slope:\002,2x,5f15.7)";
    static char fmt_410[] = "(\002 Weight factor:\002,2x,5f15.7)";
    static char fmt_425[] = "(\002 Deviation:\002,6x,5f15.7)";
    static char fmt_435[] = "(\002 Deviation in dB:\002,5f15.7)";
    static char fmt_455[] = "(/\002 Extremal frequencies:\002//(2x,5f12.7))";
    static char fmt_510[] = "(1pe14.6)";

    /* System generated locals */
    integer i__1, i__2;
    real r__1;
    doublereal d__1, d__2;
    olist o__1;
    cllist cl__1;

    /* Builtin functions */
    integer s_wsfe(cilist *), do_fio(integer *, char *, ftnlen), e_wsfe(void),
	     s_rsfe(cilist *), e_rsfe(void), s_cmp(char *, char *, ftnlen, 
	    ftnlen), f_open(olist *), s_rsle(cilist *), do_lio(integer *, 
	    integer *, char *, ftnlen), e_rsle(void);
    double pow_dd(doublereal *, doublereal *), d_lg10(doublereal *), cos(
	    doublereal), sin(doublereal);
    integer s_wsle(cilist *), e_wsle(void);
    double r_lg10(real *);
    integer f_clos(cllist *);
    /* Subroutine */ int s_stop(char *, ftnlen);

    /* Local variables */
    static real edge[20], delf;
    static integer nodd;
    extern doublereal wate_(real *, real *, real *, integer *, integer *);
    static real temp;
    static doublereal attn;
    static integer nrox;
    static real h[100];
    static integer i, j, k, l, lband;
    static char fname[100];
    static integer nfmax, nfilt, idist;
    extern /* Subroutine */ int flush_(integer *), remez_(real *, integer *);
    static integer jtype;
    static doublereal svrip;
    static real change;
    static integer io;
    static doublereal pi;
    static real fs, fx[10], deltaf;
    static integer ncoeff, nbands, nz;
    static real deviat[10];
    static doublereal ripple;
    static char answer[1];
    static integer nm1;
    extern doublereal eff_(real *, real *, integer *, integer *, integer *);
    static integer neg;
    static real fup;
    static integer kup;
    static real wtx[10];

    /* Fortran I/O blocks */
    static cilist io___3 = { 0, 6, 0, "(,a)", 0 };
    static cilist io___4 = { 0, 6, 0, "(,a)", 0 };
    static cilist io___5 = { 0, 6, 0, "(/,a)", 0 };
    static cilist io___6 = { 0, 6, 0, "(,a)", 0 };
    static cilist io___7 = { 0, 5, 0, "(a12)", 0 };
    static cilist io___10 = { 0, 6, 0, "(,a,/,20x,a)", 0 };
    static cilist io___11 = { 0, 0, 0, 0, 0 };
    static cilist io___13 = { 0, 6, 0, "(,a)", 0 };
    static cilist io___14 = { 0, 0, 0, 0, 0 };
    static cilist io___16 = { 0, 6, 0, "(,a)", 0 };
    static cilist io___17 = { 0, 0, 0, 0, 0 };
    static cilist io___21 = { 0, 6, 0, "(/,a)", 0 };
    static cilist io___22 = { 0, 0, 0, 0, 0 };
    static cilist io___25 = { 0, 6, 0, "(,a)", 0 };
    static cilist io___26 = { 0, 0, 0, 0, 0 };
    static cilist io___28 = { 0, 6, 0, "(,a,i3,a)", 0 };
    static cilist io___30 = { 0, 0, 0, 0, 0 };
    static cilist io___31 = { 0, 6, 0, "(,a,i3,a)", 0 };
    static cilist io___32 = { 0, 0, 0, 0, 0 };
    static cilist io___33 = { 0, 6, 0, "(,a,i3,a)", 0 };
    static cilist io___34 = { 0, 0, 0, 0, 0 };
    static cilist io___35 = { 0, 6, 0, "(,a,i3,a)", 0 };
    static cilist io___36 = { 0, 0, 0, 0, 0 };
    static cilist io___37 = { 0, 6, 0, "(,a)", 0 };
    static cilist io___38 = { 0, 0, 0, "(a1)", 0 };
    static cilist io___41 = { 0, 6, 0, "(,a)", 0 };
    static cilist io___42 = { 0, 6, 0, "(,a)", 0 };
    static cilist io___43 = { 0, 0, 0, "(a)", 0 };
    static cilist io___45 = { 0, 6, 0, "(,a)", 0 };
    static cilist io___46 = { 0, 0, 0, 0, 0 };
    static cilist io___48 = { 0, 6, 0, "(,a)", 0 };
    static cilist io___49 = { 0, 0, 0, 0, 0 };
    static cilist io___53 = { 0, 6, 0, "(/,a,i4)", 0 };
    static cilist io___55 = { 0, 6, 0, "(/,a,/)", 0 };
    static cilist io___66 = { 0, 6, 0, fmt_360, 0 };
    static cilist io___67 = { 0, 6, 0, fmt_365, 0 };
    static cilist io___68 = { 0, 6, 0, fmt_370, 0 };
    static cilist io___69 = { 0, 6, 0, fmt_375, 0 };
    static cilist io___70 = { 0, 6, 0, fmt_376, 0 };
    static cilist io___71 = { 0, 6, 0, fmt_378, 0 };
    static cilist io___72 = { 0, 6, 0, fmt_379, 0 };
    static cilist io___73 = { 0, 6, 0, fmt_380, 0 };
    static cilist io___75 = { 0, 6, 0, fmt_382, 0 };
    static cilist io___76 = { 0, 6, 0, fmt_383, 0 };
    static cilist io___77 = { 0, 6, 0, fmt_384, 0 };
    static cilist io___78 = { 0, 6, 0, 0, 0 };
    static cilist io___80 = { 0, 6, 0, fmt_385, 0 };
    static cilist io___81 = { 0, 6, 0, fmt_390, 0 };
    static cilist io___82 = { 0, 6, 0, fmt_395, 0 };
    static cilist io___83 = { 0, 6, 0, fmt_400, 0 };
    static cilist io___84 = { 0, 6, 0, fmt_405, 0 };
    static cilist io___85 = { 0, 6, 0, fmt_410, 0 };
    static cilist io___87 = { 0, 6, 0, fmt_425, 0 };
    static cilist io___88 = { 0, 6, 0, fmt_435, 0 };
    static cilist io___89 = { 0, 6, 0, fmt_455, 0 };
    static cilist io___91 = { 0, 1, 0, fmt_510, 0 };
    static cilist io___92 = { 0, 1, 0, fmt_510, 0 };
    static cilist io___93 = { 0, 1, 0, fmt_510, 0 };
    static cilist io___94 = { 0, 1, 0, fmt_510, 0 };
    static cilist io___95 = { 0, 1, 0, fmt_510, 0 };
    static cilist io___96 = { 0, 1, 0, fmt_510, 0 };
    static cilist io___97 = { 0, 1, 0, fmt_510, 0 };
    static cilist io___98 = { 0, 1, 0, fmt_510, 0 };
    static cilist io___99 = { 0, 1, 0, fmt_510, 0 };
    static cilist io___100 = { 0, 1, 0, fmt_510, 0 };
    static cilist io___101 = { 0, 6, 0, 0, 0 };
    static cilist io___102 = { 0, 6, 0, 0, 0 };
    static cilist io___103 = { 0, 6, 0, "(/,a)", 0 };
    static cilist io___104 = { 0, 5, 0, "(a1)", 0 };


/* @(#)optfir.f	1.1	11/2/90 */

/*   This program designs optimal equirriple FIR digital filters */

/*<       double precision attn,ripple,svrip >*/
/*<       double precision pi2,pi,ad,dev,x,y >*/
/*<       character fname*100, answer*1 >*/
/*<       common des,wt,alpha,iext,nfcns,ngrid,pi2,ad,dev,x,y,grid >*/
/*<       dimension iext(100),ad(100),alpha(100),x(100),y(100),h(100) >*/
/*<       dimension des(1600),grid(1600),wt(1600) >*/
/*<       dimension edge(20),fx(10),wtx(10),deviat(10) >*/
/*<       pi=3.141592653589793 >*/
    pi = 3.141592653589793f;
/*<       pi2=6.283185307179586 >*/
    _BLNK__1.pi2 = 6.283185307179586f;
/* Maximum filter length */
/*<       nfmax=256 >*/
    nfmax = 256;
/*<       write (*,'(,a)') ' PROVISIONAL EQUIRRIPLE FILTER DESIGN ' >*/
    s_wsfe(&io___3);
    do_fio(&c__1, " PROVISIONAL EQUIRRIPLE FILTER DESIGN ", 38L);
    e_wsfe();
/*<       write (*,'(,a)') ' USE AT YOUR OWN RISK --------------- ' >*/
    s_wsfe(&io___4);
    do_fio(&c__1, " USE AT YOUR OWN RISK --------------- ", 38L);
    e_wsfe();
/*<   100 continue >*/
L100:
/*<    >*/
    s_wsfe(&io___5);
    do_fio(&c__1, " Enter name of input command file (press <Enter> for manu"
	    "al entry, ", 67L);
    e_wsfe();
/*<    >*/
    s_wsfe(&io___6);
    do_fio(&c__1, " Sorry, no tilde-expansion.  Give path relative to your h"
	    "ome or startup directory): ", 84L);
    e_wsfe();
/*<       read (*,'(a12)') fname >*/
    s_rsfe(&io___7);
    do_fio(&c__1, fname, 100L);
    e_rsfe();
/*<       if (fname .eq. ' ') then >*/
    if (s_cmp(fname, " ", 100L, 1L) == 0) {
/*<          io = 5 >*/
	io = 5;
/*<       else >*/
    } else {
/*<          io = 3 >*/
	io = 3;
/*<          open (3, file=fname) >*/
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
/*<       endif >*/
    }
/*<    >*/
    s_wsfe(&io___10);
    do_fio(&c__1, " Enter filter type (1=Bandpass, 2=Differentiator,", 49L);
    do_fio(&c__1, "3=Hilbert transformer, 4=Half-band): ", 37L);
    e_wsfe();
/*<       read (io,*) jtype >*/
    io___11.ciunit = io;
    s_rsle(&io___11);
    do_lio(&c__3, &c__1, (char *)&jtype, (ftnlen)sizeof(integer));
    e_rsle();
/*<       write (*,'(,a)') ' Enter filter length (enter 0 for estimate): ' >*/
    s_wsfe(&io___13);
    do_fio(&c__1, " Enter filter length (enter 0 for estimate): ", 45L);
    e_wsfe();
/*<       read (io,*) nfilt >*/
    io___14.ciunit = io;
    s_rsle(&io___14);
    do_lio(&c__3, &c__1, (char *)&nfilt, (ftnlen)sizeof(integer));
    e_rsle();
/*<       if(nfilt.gt.nfmax.or.(nfilt.lt.3.and.nfilt.ne.0)) go to 515 >*/
    if (nfilt > nfmax || nfilt < 3 && nfilt != 0) {
	goto L515;
    }
/*<       write (*,'(,a)') ' Enter sampling rate of filter: ' >*/
    s_wsfe(&io___16);
    do_fio(&c__1, " Enter sampling rate of filter: ", 32L);
    e_wsfe();
/*<       read (io,*) fs >*/
    io___17.ciunit = io;
    s_rsle(&io___17);
    do_lio(&c__4, &c__1, (char *)&fs, (ftnlen)sizeof(real));
    e_rsle();
/*<       if (jtype .eq. 4) then >*/
    if (jtype == 4) {
/*<          nfilt = (nfilt + 1) / 2 >*/
	nfilt = (nfilt + 1) / 2;
/*<          nbands = 1 >*/
	nbands = 1;
/*<          edge(1) = 0.0 >*/
	edge[0] = 0.f;
/*<          write (*,'(/,a\)') ' Enter passband edge frequency: ' >*/
	s_wsfe(&io___21);
	do_fio(&c__1, " Enter passband edge frequency: ", 32L);
	e_wsfe();
/*<          read (io,*) edge(2) >*/
	io___22.ciunit = io;
	s_rsle(&io___22);
	do_lio(&c__4, &c__1, (char *)&edge[1], (ftnlen)sizeof(real));
	e_rsle();
/*<          edge(2) = 2.0 * edge(2) / fs >*/
	edge[1] = edge[1] * 2.f / fs;
/*<          fx(1) = 0.5 >*/
	fx[0] = .5f;
/*<          wtx(1) = 1.0 >*/
	wtx[0] = 1.f;
/*<          go to 118 >*/
	goto L118;
/*<       endif >*/
    }
/*<       write (*,'(,a)') ' Enter number of filter bands: ' >*/
    s_wsfe(&io___25);
    do_fio(&c__1, " Enter number of filter bands: ", 31L);
    e_wsfe();
/*<       read (io,*) nbands >*/
    io___26.ciunit = io;
    s_rsle(&io___26);
    do_lio(&c__3, &c__1, (char *)&nbands, (ftnlen)sizeof(integer));
    e_rsle();
/*<       if(nbands.le.0) nbands=1 >*/
    if (nbands <= 0) {
	nbands = 1;
    }
/*<       do 115 i=1,nbands >*/
    i__1 = nbands;
    for (i = 1; i <= i__1; ++i) {
/*<       write (*,'(,a,i3,a)') ' Enter lower band edge for band', i, ': ' >*/
	s_wsfe(&io___28);
	do_fio(&c__1, " Enter lower band edge for band", 31L);
	do_fio(&c__1, (char *)&i, (ftnlen)sizeof(integer));
	do_fio(&c__1, ": ", 2L);
	e_wsfe();
/*<       j=1+2*(i-1) >*/
	j = (i - 1 << 1) + 1;
/*<       read (io,*) edge(j) >*/
	io___30.ciunit = io;
	s_rsle(&io___30);
	do_lio(&c__4, &c__1, (char *)&edge[j - 1], (ftnlen)sizeof(real));
	e_rsle();
/*<       edge(j)=edge(j)/fs >*/
	edge[j - 1] /= fs;
/*<       write (*,'(,a,i3,a)') ' Enter upper band edge for band', i, ': ' >*/
	s_wsfe(&io___31);
	do_fio(&c__1, " Enter upper band edge for band", 31L);
	do_fio(&c__1, (char *)&i, (ftnlen)sizeof(integer));
	do_fio(&c__1, ": ", 2L);
	e_wsfe();
/*<       read (io,*) edge(j+1) >*/
	io___32.ciunit = io;
	s_rsle(&io___32);
	do_lio(&c__4, &c__1, (char *)&edge[j], (ftnlen)sizeof(real));
	e_rsle();
/*<       edge(j+1)=edge(j+1)/fs >*/
	edge[j] /= fs;
/*<       write (*,'(,a,i3,a)') ' Enter desired value for band', i, ': ' >*/
	s_wsfe(&io___33);
	do_fio(&c__1, " Enter desired value for band", 29L);
	do_fio(&c__1, (char *)&i, (ftnlen)sizeof(integer));
	do_fio(&c__1, ": ", 2L);
	e_wsfe();
/*<       read (io,*) fx(i) >*/
	io___34.ciunit = io;
	s_rsle(&io___34);
	do_lio(&c__4, &c__1, (char *)&fx[i - 1], (ftnlen)sizeof(real));
	e_rsle();
/*<       write (*,'(,a,i3,a)') ' Enter weight factor for band', i, ': ' >*/
	s_wsfe(&io___35);
	do_fio(&c__1, " Enter weight factor for band", 29L);
	do_fio(&c__1, (char *)&i, (ftnlen)sizeof(integer));
	do_fio(&c__1, ": ", 2L);
	e_wsfe();
/*<       read (io,*) wtx(i) >*/
	io___36.ciunit = io;
	s_rsle(&io___36);
	do_lio(&c__4, &c__1, (char *)&wtx[i - 1], (ftnlen)sizeof(real));
	e_rsle();
/*<   115 continue >*/
/* L115: */
    }
/*<       write (*,'(,a)') ' Do you want x/sin(x) predistortion? (y/n): ' >*/
    s_wsfe(&io___37);
    do_fio(&c__1, " Do you want x/sin(x) predistortion? (y/n): ", 44L);
    e_wsfe();
/*<       read (io,'(a1)') answer >*/
    io___38.ciunit = io;
    s_rsfe(&io___38);
    do_fio(&c__1, answer, 1L);
    e_rsfe();
/*<       if (answer .eq. 'y' .or. answer .eq. 'Y') idist=1 >*/
    if (*answer == 'y' || *answer == 'Y') {
	idist = 1;
    }
/*<   118 write (*,'(,a)') ' Enter name of coefficient output file ' >*/
L118:
    s_wsfe(&io___41);
    do_fio(&c__1, " Enter name of coefficient output file ", 39L);
    e_wsfe();
/*<    >*/
    s_wsfe(&io___42);
    do_fio(&c__1, " (Sorry, no tilde-expansion.  Give path relativeve to you"
	    "r home directory): ", 76L);
    e_wsfe();
/*<       read (io,'(a)') fname >*/
    io___43.ciunit = io;
    s_rsfe(&io___43);
    do_fio(&c__1, fname, 100L);
    e_rsfe();
/*<       open (1, file=fname) >*/
    o__1.oerr = 0;
    o__1.ounit = 1;
    o__1.ofnmlen = 100;
    o__1.ofnm = fname;
    o__1.orl = 0;
    o__1.osta = 0;
    o__1.oacc = 0;
    o__1.ofm = 0;
    o__1.oblnk = 0;
    f_open(&o__1);
/*<       if(nfilt.eq.0) go to 120 >*/
    if (nfilt == 0) {
	goto L120;
    }
/*<       nrox=0 >*/
    nrox = 0;
/*<       go to 130 >*/
    goto L130;
/*<   120 continue >*/
L120:
/*<       write (*,'(,a)') ' Enter desired passband ripple in dB: ' >*/
    s_wsfe(&io___45);
    do_fio(&c__1, " Enter desired passband ripple in dB: ", 38L);
    e_wsfe();
/*<       read (io,*) ripple >*/
    io___46.ciunit = io;
    s_rsle(&io___46);
    do_lio(&c__5, &c__1, (char *)&ripple, (ftnlen)sizeof(doublereal));
    e_rsle();
/*<       write (*,'(,a)') ' Enter desired stopband attenuation in dB: ' >*/
    s_wsfe(&io___48);
    do_fio(&c__1, " Enter desired stopband attenuation in dB: ", 43L);
    e_wsfe();
/*<       read (io,*) attn >*/
    io___49.ciunit = io;
    s_rsle(&io___49);
    do_lio(&c__5, &c__1, (char *)&attn, (ftnlen)sizeof(doublereal));
    e_rsle();
/*<       ripple=10.**(ripple/20.) >*/
    d__1 = ripple / 20.f;
    ripple = pow_dd(&c_b106, &d__1);
/*<       attn=10.**(-attn/20.) >*/
    d__1 = -attn / 20.f;
    attn = pow_dd(&c_b106, &d__1);
/*<       ripple=(ripple-1)/(ripple+1) >*/
    ripple = (ripple - 1) / (ripple + 1);
/*<       ripple=dlog10(ripple) >*/
    ripple = d_lg10(&ripple);
/*<       attn=dlog10(attn) >*/
    attn = d_lg10(&attn);
/*<       svrip=ripple >*/
    svrip = ripple;
/*<    >*/
/* Computing 2nd power */
    d__1 = ripple;
/* Computing 2nd power */
    d__2 = ripple;
    ripple = (d__1 * d__1 * .005309f + ripple * .07114f - .4761f) * attn - (
	    d__2 * d__2 * .00266f + ripple * .5941f + .4278f);
/*<       attn=11.01217+0.51244*(svrip-attn) >*/
    attn = (svrip - attn) * .51244f + 11.01217f;
/*<       deltaf=edge(3)-edge(2) >*/
    deltaf = edge[2] - edge[1];
/*<       nfilt=ripple/deltaf-attn*deltaf+1 >*/
    nfilt = (integer) (ripple / deltaf - attn * deltaf + 1);
/*<       write (*,'(/,a,i4)') ' Estimated filter length = ', nfilt >*/
    s_wsfe(&io___53);
    do_fio(&c__1, " Estimated filter length = ", 27L);
    do_fio(&c__1, (char *)&nfilt, (ftnlen)sizeof(integer));
    e_wsfe();
/*<       if(nfilt.gt.nfmax.or.(nfilt.lt.3.and.nfilt.ne.0)) go to 515 >*/
    if (nfilt > nfmax || nfilt < 3 && nfilt != 0) {
	goto L515;
    }
/*<       nrox=1 >*/
    nrox = 1;
/*<   130 neg=1 >*/
L130:
    neg = 1;
/*<       write (*,'(/,a,/)') ' Executing ...' >*/
    s_wsfe(&io___55);
    do_fio(&c__1, " Executing ...", 14L);
    e_wsfe();
/*<       if(jtype .eq. 1 .or. jtype .eq. 4) neg = 0 >*/
    if (jtype == 1 || jtype == 4) {
	neg = 0;
    }
/*<       nodd=nfilt/2 >*/
    nodd = nfilt / 2;
/*<       nodd=nfilt-2*nodd >*/
    nodd = nfilt - (nodd << 1);
/*<       nfcns=nfilt/2 >*/
    _BLNK__1.nfcns = nfilt / 2;
/*<       if(nodd.eq.1.and.neg.eq.0) nfcns=nfcns+1 >*/
    if (nodd == 1 && neg == 0) {
	++_BLNK__1.nfcns;
    }
/*<       grid(1)=edge(1) >*/
    _BLNK__1.grid[0] = edge[0];
/*<       delf=16*nfcns >*/
    delf = (real) (_BLNK__1.nfcns << 4);
/*<       delf=0.5/delf >*/
    delf = .5f / delf;
/*<       if(neg.eq.0) go to 135 >*/
    if (neg == 0) {
	goto L135;
    }
/*<       if(edge(1).lt.delf)grid(1)=delf >*/
    if (edge[0] < delf) {
	_BLNK__1.grid[0] = delf;
    }
/*<   135 continue >*/
L135:
/*<       j=1 >*/
    j = 1;
/*<       l=1 >*/
    l = 1;
/*<       lband=1 >*/
    lband = 1;
/*<   140 fup=edge(l+1) >*/
L140:
    fup = edge[l];
/*<   145 temp=grid(j) >*/
L145:
    temp = _BLNK__1.grid[j - 1];
/*<       des(j)=eff(temp,fx,lband,jtype,idist) >*/
    _BLNK__1.des[j - 1] = eff_(&temp, fx, &lband, &jtype, &idist);
/*<       wt(j)=wate(temp,fx,wtx,lband,jtype) >*/
    _BLNK__1.wt[j - 1] = wate_(&temp, fx, wtx, &lband, &jtype);
/*<       j=j+1 >*/
    ++j;
/*<       grid(j)=temp+delf >*/
    _BLNK__1.grid[j - 1] = temp + delf;
/*<       if(grid(j).gt.fup) go to 150 >*/
    if (_BLNK__1.grid[j - 1] > fup) {
	goto L150;
    }
/*<       go to 145 >*/
    goto L145;
/*<   150 grid(j-1)=fup >*/
L150:
    _BLNK__1.grid[j - 2] = fup;
/*<       des(j-1)=eff(fup,fx,lband,jtype,idist) >*/
    _BLNK__1.des[j - 2] = eff_(&fup, fx, &lband, &jtype, &idist);
/*<       wt(j-1)=wate(fup,fx,wtx,lband,jtype) >*/
    _BLNK__1.wt[j - 2] = wate_(&fup, fx, wtx, &lband, &jtype);
/*<       lband=lband+1 >*/
    ++lband;
/*<       l=l+2 >*/
    l += 2;
/*<       if(lband.gt.nbands) go to 160 >*/
    if (lband > nbands) {
	goto L160;
    }
/*<       grid(j)=edge(l) >*/
    _BLNK__1.grid[j - 1] = edge[l - 1];
/*<       go to 140 >*/
    goto L140;
/*<   160 ngrid=j-1 >*/
L160:
    _BLNK__1.ngrid = j - 1;
/*<       if(neg.ne.nodd) go to 165 >*/
    if (neg != nodd) {
	goto L165;
    }
/*<       if(grid(ngrid).gt.(0.5-delf)) ngrid=ngrid-1 >*/
    if (_BLNK__1.grid[_BLNK__1.ngrid - 1] > .5f - delf) {
	--_BLNK__1.ngrid;
    }
/*<   165 continue >*/
L165:
/*<       if(neg)170,170,180 >*/
    if (neg <= 0) {
	goto L170;
    } else {
	goto L180;
    }
/*<   170 if(nodd.eq.1) go to 200 >*/
L170:
    if (nodd == 1) {
	goto L200;
    }
/*<       do 175 j=1,ngrid >*/
    i__1 = _BLNK__1.ngrid;
    for (j = 1; j <= i__1; ++j) {
/*<       change=dcos(pi*grid(j)) >*/
	change = cos(pi * _BLNK__1.grid[j - 1]);
/*<       des(j)=des(j)/change >*/
	_BLNK__1.des[j - 1] /= change;
/*<   175 wt(j)=wt(j)*change >*/
/* L175: */
	_BLNK__1.wt[j - 1] *= change;
    }
/*<       go to 200 >*/
    goto L200;
/*<   180 if(nodd.eq.1) go to 190 >*/
L180:
    if (nodd == 1) {
	goto L190;
    }
/*<       do 185 j=1,ngrid >*/
    i__1 = _BLNK__1.ngrid;
    for (j = 1; j <= i__1; ++j) {
/*<       change=dsin(pi*grid(j)) >*/
	change = sin(pi * _BLNK__1.grid[j - 1]);
/*<       des(j)=des(j)/change >*/
	_BLNK__1.des[j - 1] /= change;
/*<   185 wt(j)=wt(j)*change >*/
/* L185: */
	_BLNK__1.wt[j - 1] *= change;
    }
/*<       go to 200 >*/
    goto L200;
/*<   190 do 195 j=1,ngrid >*/
L190:
    i__1 = _BLNK__1.ngrid;
    for (j = 1; j <= i__1; ++j) {
/*<       change=dsin(pi2*grid(j)) >*/
	change = sin(_BLNK__1.pi2 * _BLNK__1.grid[j - 1]);
/*<       des(j)=des(j)/change >*/
	_BLNK__1.des[j - 1] /= change;
/*<   195 wt(j)=wt(j)*change >*/
/* L195: */
	_BLNK__1.wt[j - 1] *= change;
    }
/*<   200 temp=float(ngrid-1)/float(nfcns) >*/
L200:
    temp = (real) (_BLNK__1.ngrid - 1) / (real) _BLNK__1.nfcns;
/*<       do 210 j=1,nfcns >*/
    i__1 = _BLNK__1.nfcns;
    for (j = 1; j <= i__1; ++j) {
/*<   210 iext(j)=(j-1)*temp+1 >*/
/* L210: */
	_BLNK__1.iext[j - 1] = (j - 1) * temp + 1;
    }
/*<       iext(nfcns+1)=ngrid >*/
    _BLNK__1.iext[_BLNK__1.nfcns] = _BLNK__1.ngrid;
/*<       nm1=nfcns-1 >*/
    nm1 = _BLNK__1.nfcns - 1;
/*<       nz=nfcns+1 >*/
    nz = _BLNK__1.nfcns + 1;
/*<       call Remez(edge,nbands) >*/
    remez_(edge, &nbands);
/*<       if(neg) 300,300,320 >*/
    if (neg <= 0) {
	goto L300;
    } else {
	goto L320;
    }
/*<   300 if(nodd.eq.0) go to 310 >*/
L300:
    if (nodd == 0) {
	goto L310;
    }
/*<       do 305 j=1,nm1 >*/
    i__1 = nm1;
    for (j = 1; j <= i__1; ++j) {
/*<   305 h(j)=0.5*alpha(nz-j) >*/
/* L305: */
	h[j - 1] = _BLNK__1.alpha[nz - j - 1] * .5f;
    }
/*<       h(nfcns)=alpha(1) >*/
    h[_BLNK__1.nfcns - 1] = _BLNK__1.alpha[0];
/*<       go to 350 >*/
    goto L350;
/*<   310 h(1)=0.25*alpha(nfcns) >*/
L310:
    h[0] = _BLNK__1.alpha[_BLNK__1.nfcns - 1] * .25f;
/*<       do 315 j=2,nm1 >*/
    i__1 = nm1;
    for (j = 2; j <= i__1; ++j) {
/*<   315 h(j)=0.25*(alpha(nz-j)+alpha(nfcns+2-j)) >*/
/* L315: */
	h[j - 1] = (_BLNK__1.alpha[nz - j - 1] + _BLNK__1.alpha[
		_BLNK__1.nfcns + 2 - j - 1]) * .25f;
    }
/*<       h(nfcns)=0.5*alpha(1)+0.25*alpha(2) >*/
    h[_BLNK__1.nfcns - 1] = _BLNK__1.alpha[0] * .5f + _BLNK__1.alpha[1] * 
	    .25f;
/*<       go to 350 >*/
    goto L350;
/*<   320 if(nodd.eq.0) go to 330 >*/
L320:
    if (nodd == 0) {
	goto L330;
    }
/*<       h(1)=0.25*alpha(nfcns) >*/
    h[0] = _BLNK__1.alpha[_BLNK__1.nfcns - 1] * .25f;
/*<       h(2)=0.25*alpha(nm1) >*/
    h[1] = _BLNK__1.alpha[nm1 - 1] * .25f;
/*<       do 325 j=3,nm1 >*/
    i__1 = nm1;
    for (j = 3; j <= i__1; ++j) {
/*<   325 h(j)=0.25*(alpha(nz-j)-alpha(nfcns+3-j)) >*/
/* L325: */
	h[j - 1] = (_BLNK__1.alpha[nz - j - 1] - _BLNK__1.alpha[
		_BLNK__1.nfcns + 3 - j - 1]) * .25f;
    }
/*<       h(nfcns)=0.5*alpha(1)-0.25*alpha(3) >*/
    h[_BLNK__1.nfcns - 1] = _BLNK__1.alpha[0] * .5f - _BLNK__1.alpha[2] * 
	    .25f;
/*<       h(nz)=0.0 >*/
    h[nz - 1] = 0.f;
/*<       go to 350 >*/
    goto L350;
/*<   330 h(1)=0.25*alpha(nfcns) >*/
L330:
    h[0] = _BLNK__1.alpha[_BLNK__1.nfcns - 1] * .25f;
/*<       do 335 j=2,nm1 >*/
    i__1 = nm1;
    for (j = 2; j <= i__1; ++j) {
/*<   335 h(j)=0.25*(alpha(nz-j)-alpha(nfcns+2-j)) >*/
/* L335: */
	h[j - 1] = (_BLNK__1.alpha[nz - j - 1] - _BLNK__1.alpha[
		_BLNK__1.nfcns + 2 - j - 1]) * .25f;
    }
/*<       h(nfcns)=0.5*alpha(1)-0.25*alpha(2) >*/
    h[_BLNK__1.nfcns - 1] = _BLNK__1.alpha[0] * .5f - _BLNK__1.alpha[1] * 
	    .25f;
/*<   350 write(6,360) >*/
L350:
    s_wsfe(&io___66);
    e_wsfe();
/*<    >*/
/*<       if(jtype.eq.1) write(6,365) >*/
    if (jtype == 1) {
	s_wsfe(&io___67);
	e_wsfe();
    }
/*<   365 format(' Bandpass Filter'/) >*/
/*<       if(jtype.eq.2) write(6,370) >*/
    if (jtype == 2) {
	s_wsfe(&io___68);
	e_wsfe();
    }
/*<   370 format(' Differentiator'/) >*/
/*<       if(jtype.eq.3) write(6,375) >*/
    if (jtype == 3) {
	s_wsfe(&io___69);
	e_wsfe();
    }
/*<   375 format(' Hilbert Transformer'/) >*/
/*<       if(jtype.eq.4) write(6,376) >*/
    if (jtype == 4) {
	s_wsfe(&io___70);
	e_wsfe();
    }
/*<   376 format(' Half-band Filter'/) >*/
/*<       write(6,378) nfilt >*/
    s_wsfe(&io___71);
    do_fio(&c__1, (char *)&nfilt, (ftnlen)sizeof(integer));
    e_wsfe();
/*<   378 format(' Filter length = ',i3/) >*/
/*<       if(nrox.eq.1) write(6,379) >*/
    if (nrox == 1) {
	s_wsfe(&io___72);
	e_wsfe();
    }
/*<   379 format(' Filter length determined by approximation'/) >*/
/*<       write(6,380) >*/
    s_wsfe(&io___73);
    e_wsfe();
/*<   380 format(' Impulse Response Coefficients:'/) >*/
/*<       do 381 j=1,nfcns >*/
    i__1 = _BLNK__1.nfcns;
    for (j = 1; j <= i__1; ++j) {
/*<       k=nfilt+1-j >*/
	k = nfilt + 1 - j;
/*<       if(neg.eq.0) write(6,382)j,h(j),k >*/
	if (neg == 0) {
	    s_wsfe(&io___75);
	    do_fio(&c__1, (char *)&j, (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&h[j - 1], (ftnlen)sizeof(real));
	    do_fio(&c__1, (char *)&k, (ftnlen)sizeof(integer));
	    e_wsfe();
	}
/*<       if(neg.eq.1) write(6,383)j,h(j),k >*/
	if (neg == 1) {
	    s_wsfe(&io___76);
	    do_fio(&c__1, (char *)&j, (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&h[j - 1], (ftnlen)sizeof(real));
	    do_fio(&c__1, (char *)&k, (ftnlen)sizeof(integer));
	    e_wsfe();
	}
/*<   381 continue >*/
/* L381: */
    }
/*<   382 format(10x,'h(',i3,') = ', e14.7,' = h(',i4,')') >*/
/*<   383 format(10x,'h(',i3,') = ', e14.7,' = -h(',i4,')') >*/
/*<       if(neg.eq.1.and.nodd.eq.1) write(6,384) nz, 0.0 >*/
    if (neg == 1 && nodd == 1) {
	s_wsfe(&io___77);
	do_fio(&c__1, (char *)&nz, (ftnlen)sizeof(integer));
	do_fio(&c__1, (char *)&c_b169, (ftnlen)sizeof(real));
	e_wsfe();
    }
/*<   384 format(10x,'h(',i3,') = ', e14.7) >*/
/*<       write (*,*) >*/
    s_wsle(&io___78);
    e_wsle();

/* 	DEBUG */

/*     Pause 'Press <Enter> to continue ...' */
/*<       do 450 k=1,nbands,4 >*/
    i__1 = nbands;
    for (k = 1; k <= i__1; k += 4) {
/*<       kup=k+3 >*/
	kup = k + 3;
/*<       if(kup.gt.nbands) kup=nbands >*/
	if (kup > nbands) {
	    kup = nbands;
	}
/*<       write(6,385)(j,j=k,kup) >*/
	s_wsfe(&io___80);
	i__2 = kup;
	for (j = k; j <= i__2; ++j) {
	    do_fio(&c__1, (char *)&j, (ftnlen)sizeof(integer));
	}
	e_wsfe();
/*<   385 format(25x,4('Band',i3,8x)) >*/
/*<       write(6,390)(edge(2*j-1),j=k,kup) >*/
	s_wsfe(&io___81);
	i__2 = kup;
	for (j = k; j <= i__2; ++j) {
	    do_fio(&c__1, (char *)&edge[(j << 1) - 2], (ftnlen)sizeof(real));
	}
	e_wsfe();
/*<   390 format(/' Lower band edge:',5f15.7) >*/
/*<       write(6,395)(edge(2*j),j=k,kup) >*/
	s_wsfe(&io___82);
	i__2 = kup;
	for (j = k; j <= i__2; ++j) {
	    do_fio(&c__1, (char *)&edge[(j << 1) - 1], (ftnlen)sizeof(real));
	}
	e_wsfe();
/*<   395 format(' Upper band edge:',5f15.7) >*/
/*<       if(jtype.ne.2) write(6,400) (fx(j),j=k,kup) >*/
	if (jtype != 2) {
	    s_wsfe(&io___83);
	    i__2 = kup;
	    for (j = k; j <= i__2; ++j) {
		do_fio(&c__1, (char *)&fx[j - 1], (ftnlen)sizeof(real));
	    }
	    e_wsfe();
	}
/*<   400 format(' Desired value:',2x,5f15.7) >*/
/*<       if(jtype.eq.2) write(6,405) (fx(j),j=k,kup) >*/
	if (jtype == 2) {
	    s_wsfe(&io___84);
	    i__2 = kup;
	    for (j = k; j <= i__2; ++j) {
		do_fio(&c__1, (char *)&fx[j - 1], (ftnlen)sizeof(real));
	    }
	    e_wsfe();
	}
/*<   405 format(' Desired slope:',2x,5f15.7) >*/
/*<       write(6,410) (wtx(j),j=k,kup) >*/
	s_wsfe(&io___85);
	i__2 = kup;
	for (j = k; j <= i__2; ++j) {
	    do_fio(&c__1, (char *)&wtx[j - 1], (ftnlen)sizeof(real));
	}
	e_wsfe();
/*<   410 format(' Weight factor:',2x,5f15.7) >*/
/*<       do 420 j=k,kup >*/
	i__2 = kup;
	for (j = k; j <= i__2; ++j) {
/*<   420 deviat(j)=dev/wtx(j) >*/
/* L420: */
	    deviat[j - 1] = _BLNK__1.dev / wtx[j - 1];
	}
/*<       write(6,425) (deviat(j),j=k,kup) >*/
	s_wsfe(&io___87);
	i__2 = kup;
	for (j = k; j <= i__2; ++j) {
	    do_fio(&c__1, (char *)&deviat[j - 1], (ftnlen)sizeof(real));
	}
	e_wsfe();
/*<   425 format(' Deviation:',6x,5f15.7) >*/
/*<       do 430 j=k,kup >*/
	i__2 = kup;
	for (j = k; j <= i__2; ++j) {
/*<       if(fx(j).eq.1.0) deviat(j)=(1.0+deviat(j))/(1.0-deviat(j)) >*/
	    if (fx[j - 1] == 1.f) {
		deviat[j - 1] = (deviat[j - 1] + 1.f) / (1.f - deviat[j - 1]);
	    }
/*<   430 deviat(j)=20.0*alog10(deviat(j)) >*/
/* L430: */
	    deviat[j - 1] = r_lg10(&deviat[j - 1]) * 20.f;
	}
/*<       write(6,435) (deviat(j),j=k,kup) >*/
	s_wsfe(&io___88);
	i__2 = kup;
	for (j = k; j <= i__2; ++j) {
	    do_fio(&c__1, (char *)&deviat[j - 1], (ftnlen)sizeof(real));
	}
	e_wsfe();
/*<   435 format(' Deviation in dB:',5f15.7) >*/
/*<   450 continue >*/
/* L450: */
    }
/*<       write(6,455) (grid(iext(j)),j=1,nz) >*/
    s_wsfe(&io___89);
    i__1 = nz;
    for (j = 1; j <= i__1; ++j) {
	do_fio(&c__1, (char *)&_BLNK__1.grid[_BLNK__1.iext[j - 1] - 1], (
		ftnlen)sizeof(real));
    }
    e_wsfe();
/*<   455 format(/' Extremal frequencies:'//(2x,5f12.7)) >*/
/*<       if (nodd .eq. 1) then >*/
    if (nodd == 1) {
/*<          if (neg .eq. 1) nfcns = nfcns + 1 >*/
	if (neg == 1) {
	    ++_BLNK__1.nfcns;
	}
/*<          ncoeff = 2 * nfcns - 1 >*/
	ncoeff = (_BLNK__1.nfcns << 1) - 1;
/*<       else >*/
    } else {
/*<         ncoeff = 2 * nfcns >*/
	ncoeff = _BLNK__1.nfcns << 1;
/*<       endif >*/
    }
/*<       if (jtype .eq. 4) then >*/
    if (jtype == 4) {

/*   Remove the writing of the filter length, for */
/*   Compatibility with Gabriel */
/*        write(1,500) 2 * ncoeff - 1 */

/*<   500    format(i4) >*/
/* L500: */
/*<          do 502 i = 1, ncoeff - 1 >*/
	i__1 = ncoeff - 1;
	for (i = 1; i <= i__1; ++i) {
/*<             if (i .lt. nfcns) then >*/
	    if (i < _BLNK__1.nfcns) {
/*<                write (1,510) h(i) >*/
		s_wsfe(&io___91);
		do_fio(&c__1, (char *)&h[i - 1], (ftnlen)sizeof(real));
		e_wsfe();
/*<                write (1,510) 0.0 >*/
		s_wsfe(&io___92);
		do_fio(&c__1, (char *)&c_b169, (ftnlen)sizeof(real));
		e_wsfe();
/*<             elseif (i .eq. nfcns) then >*/
	    } else if (i == _BLNK__1.nfcns) {
/*<                write (1,510) h(i) >*/
		s_wsfe(&io___93);
		do_fio(&c__1, (char *)&h[i - 1], (ftnlen)sizeof(real));
		e_wsfe();
/*<                write (1,510) 0.5 >*/
		s_wsfe(&io___94);
		do_fio(&c__1, (char *)&c_b219, (ftnlen)sizeof(real));
		e_wsfe();
/*<                write (1,510) h(i) >*/
		s_wsfe(&io___95);
		do_fio(&c__1, (char *)&h[i - 1], (ftnlen)sizeof(real));
		e_wsfe();
/*<             else >*/
	    } else {
/*<                write (1,510) 0.0 >*/
		s_wsfe(&io___96);
		do_fio(&c__1, (char *)&c_b169, (ftnlen)sizeof(real));
		e_wsfe();
/*<                write (1,510) h(ncoeff-i) >*/
		s_wsfe(&io___97);
		do_fio(&c__1, (char *)&h[ncoeff - i - 1], (ftnlen)sizeof(real)
			);
		e_wsfe();
/*<             endif >*/
	    }
/*<   502    continue >*/
/* L502: */
	}
/*<       else >*/
    } else {

/*   Remove the writing of the filter length, for */
/*   Compatibility with Gabriel */
/*        write(1,500) ncoeff */

/*<          do 505 i = 1, ncoeff >*/
	i__1 = ncoeff;
	for (i = 1; i <= i__1; ++i) {
/*<             if(i .le. nfcns) write(1,510) h(i) >*/
	    if (i <= _BLNK__1.nfcns) {
		s_wsfe(&io___98);
		do_fio(&c__1, (char *)&h[i - 1], (ftnlen)sizeof(real));
		e_wsfe();
	    }
/*<             if(i .gt. nfcns) then >*/
	    if (i > _BLNK__1.nfcns) {
/*<                if(neg .eq. 0) then >*/
		if (neg == 0) {
/*<                   write(1,510) h(ncoeff-i+1) >*/
		    s_wsfe(&io___99);
		    do_fio(&c__1, (char *)&h[ncoeff - i], (ftnlen)sizeof(real)
			    );
		    e_wsfe();
/*<                else >*/
		} else {
/*<                   write(1,510) -h(ncoeff-i+1) >*/
		    s_wsfe(&io___100);
		    r__1 = -(doublereal)h[ncoeff - i];
		    do_fio(&c__1, (char *)&r__1, (ftnlen)sizeof(real));
		    e_wsfe();
/*<                endif >*/
		}
/*<             endif >*/
	    }
/*<   505    continue >*/
/* L505: */
	}
/*<       endif >*/
    }
/*<   510 format(1pe14.6) >*/
/*<       write (*,*) >*/
    s_wsle(&io___101);
    e_wsle();
/*<       go to 520 >*/
    goto L520;

/*   Catch Errors */

/*<   515 print *, '*** FILTER TOO LONG OR SHORT FOR PROGRAM ***' >*/
L515:
    s_wsle(&io___102);
    do_lio(&c__9, &c__1, "*** FILTER TOO LONG OR SHORT FOR PROGRAM ***", 44L);
    e_wsle();

/*   Find out whether to terminate program */

/*<   520 call flush(1) >*/
L520:
    flush_(&c__1);
/*<       close(1) >*/
    cl__1.cerr = 0;
    cl__1.cunit = 1;
    cl__1.csta = 0;
    f_clos(&cl__1);
/*<       write (*,'(/,a\)') ' Another design? (y/n): ' >*/
    s_wsfe(&io___103);
    do_fio(&c__1, " Another design? (y/n): ", 24L);
    e_wsfe();
/*<       read (*,'(a1)') answer >*/
    s_rsfe(&io___104);
    do_fio(&c__1, answer, 1L);
    e_rsfe();
/*<       if (answer .eq. 'y' .or. answer .eq. 'Y') go to 100 >*/
    if (*answer == 'y' || *answer == 'Y') {
	goto L100;
    }
/*<       stop 'End of program' >*/
    s_stop("End of program", 14L);
/*<       end >*/
    return 0;
} /* MAIN__ */


/*<       Function Eff(freq,fx,lband,jtype,idist) >*/
doublereal eff_(real *freq, real *fx, integer *lband, integer *jtype, integer 
	*idist)
{
    /* System generated locals */
    real ret_val;

    /* Builtin functions */
    double sin(doublereal);

    /* Local variables */
    static real x;

/*<       dimension fx(10) >*/
/*<       if(jtype.eq.2) go to 1 >*/
    /* Parameter adjustments */
    --fx;

    /* Function Body */
    if (*jtype == 2) {
	goto L1;
    }
/*<       eff=fx(lband) >*/
    ret_val = fx[*lband];
/*<       if(idist.eq.0) return >*/
    if (*idist == 0) {
	return ret_val;
    }
/*<       x=3.141593*freq >*/
    x = *freq * 3.141593f;
/*<       if(x.ne.0) eff=eff*x/sin(x) >*/
    if (x != 0.f) {
	ret_val = ret_val * x / sin(x);
    }
/*<       return >*/
    return ret_val;
/*<     1 eff=fx(lband)*freq >*/
L1:
    ret_val = fx[*lband] * *freq;
/*<       return >*/
    return ret_val;
/*<       end >*/
} /* eff_ */


/*<       Function Wate(freq,fx,wtx,lband,jtype) >*/
doublereal wate_(real *freq, real *fx, real *wtx, integer *lband, integer *
	jtype)
{
    /* System generated locals */
    real ret_val;

/*<       dimension fx(10),wtx(10) >*/
/*<       if(jtype.eq.2) go to 1 >*/
    /* Parameter adjustments */
    --wtx;
    --fx;

    /* Function Body */
    if (*jtype == 2) {
	goto L1;
    }
/*<       wate=wtx(lband) >*/
    ret_val = wtx[*lband];
/*<       return >*/
    return ret_val;
/*<     1 if(fx(lband).lt.0.0001) go to 2 >*/
L1:
    if (fx[*lband] < 1e-4f) {
	goto L2;
    }
/*<       wate=wtx(lband)/freq >*/
    ret_val = wtx[*lband] / *freq;
/*<       return >*/
    return ret_val;
/*<     2 wate=wtx(lband) >*/
L2:
    ret_val = wtx[*lband];
/*<       return >*/
    return ret_val;
/*<       end >*/
} /* wate_ */


/*<       Subroutine Remez(edge,nbands) >*/
/* Subroutine */ int remez_(real *edge, integer *nbands)
{
    /* System generated locals */
    integer i__1, i__2;

    /* Builtin functions */
    double cos(doublereal), acos(doublereal);

    /* Local variables */
    static doublereal dden;
    static real delf, devl;
    extern /* Subroutine */ int ouch_(void);
    static real comp;
    static integer luck;
    static doublereal dnum;
    static integer klow;
    static doublereal a[100];
    extern doublereal d_(integer *, integer *, integer *);
    static integer j, k, l;
    static doublereal p[100], q[100], dtemp;
    static real gtemp;
    static integer niter, k1;
    static real y1, aa, bb, cn;
    static integer kn;
    static real ft, xe;
    static integer jchnge, nu, nz;
    static real xt;
    static integer itrmax, jm1, nm1, jp1;
    extern doublereal gee_(integer *, integer *);
    static integer kid;
    static real fsh;
    static integer kkk, jet;
    static real err;
    static integer kup, knz, nut;
    static real ynz;
    static integer nzz, nut1;

/*<       double precision pi2,dnum,dden,dtemp,a,p,q >*/
/*<       double precision d,ad,dev,x,y >*/
/*<       common des,wt,alpha,iext,nfcns,ngrid,pi2,ad,dev,x,y,grid >*/
/*<       dimension edge(20) >*/
/*<       dimension iext(100),ad(100),alpha(100),x(100),y(100) >*/
/*<       dimension des(1600),grid(1600),wt(1600) >*/
/*<       dimension a(100),p(100),q(100) >*/
/*<       itrmax=25 >*/
    /* Parameter adjustments */
    --edge;

    /* Function Body */
    itrmax = 25;
/*<       devl=-1.0 >*/
    devl = -1.f;
/*<       nz=nfcns+1 >*/
    nz = _BLNK__1.nfcns + 1;
/*<       nzz=nfcns+2 >*/
    nzz = _BLNK__1.nfcns + 2;
/*<       niter=0 >*/
    niter = 0;
/*<   100 continue >*/
L100:
/*<       iext(nzz)=ngrid+1 >*/
    _BLNK__1.iext[nzz - 1] = _BLNK__1.ngrid + 1;
/*<       niter=niter+1 >*/
    ++niter;
/*<       if(niter.gt.itrmax) go to 400 >*/
    if (niter > itrmax) {
	goto L400;
    }
/*<       do 110 j=1,nz >*/
    i__1 = nz;
    for (j = 1; j <= i__1; ++j) {
/*<       dtemp=grid(iext(j)) >*/
	dtemp = _BLNK__1.grid[_BLNK__1.iext[j - 1] - 1];
/*<       dtemp=dcos(dtemp*pi2) >*/
	dtemp = cos(dtemp * _BLNK__1.pi2);
/*<   110 x(j)=dtemp >*/
/* L110: */
	_BLNK__1.x[j - 1] = dtemp;
    }
/*<       jet=(nfcns-1)/15+1 >*/
    jet = (_BLNK__1.nfcns - 1) / 15 + 1;
/*<       do 120 j=1,nz >*/
    i__1 = nz;
    for (j = 1; j <= i__1; ++j) {
/*<       kid=j >*/
	kid = j;
/*<   120 ad(j)=d(kid,nz,jet) >*/
/* L120: */
	_BLNK__1.ad[j - 1] = d_(&kid, &nz, &jet);
    }
/*<       dnum=0.0 >*/
    dnum = 0.f;
/*<       dden=0.0 >*/
    dden = 0.f;
/*<       k=1 >*/
    k = 1;
/*<       do 130 j=1,nz >*/
    i__1 = nz;
    for (j = 1; j <= i__1; ++j) {
/*<       l=iext(j) >*/
	l = _BLNK__1.iext[j - 1];
/*<       dtemp=ad(j)*des(l) >*/
	dtemp = _BLNK__1.ad[j - 1] * _BLNK__1.des[l - 1];
/*<       dnum=dnum+dtemp >*/
	dnum += dtemp;
/*<       dtemp=k*ad(j)/wt(l) >*/
	dtemp = k * _BLNK__1.ad[j - 1] / _BLNK__1.wt[l - 1];
/*<       dden=dden+dtemp >*/
	dden += dtemp;
/*<   130 k=-k >*/
/* L130: */
	k = -k;
    }
/*<       dev=dnum/dden >*/
    _BLNK__1.dev = dnum / dden;

/* 	DEBUG */

/*     write(6,131) dev */
/* 131 format(1x,12hdeviation = ,f12.9) */

/*<       nu=1 >*/
    nu = 1;
/*<       if(dev.gt.0.0) nu=-1 >*/
    if (_BLNK__1.dev > 0.) {
	nu = -1;
    }
/*<       dev=-nu*dev >*/
    _BLNK__1.dev = -nu * _BLNK__1.dev;
/*<       k=nu >*/
    k = nu;
/*<       do 140 j=1,nz >*/
    i__1 = nz;
    for (j = 1; j <= i__1; ++j) {
/*<       l=iext(j) >*/
	l = _BLNK__1.iext[j - 1];
/*<       dtemp=k*dev/wt(l) >*/
	dtemp = k * _BLNK__1.dev / _BLNK__1.wt[l - 1];
/*<       y(j)=des(l)+dtemp >*/
	_BLNK__1.y[j - 1] = _BLNK__1.des[l - 1] + dtemp;
/*<   140 k=-k >*/
/* L140: */
	k = -k;
    }
/*<       if(dev.ge.devl) go to 150 >*/
    if (_BLNK__1.dev >= devl) {
	goto L150;
    }
/*<       call Ouch >*/
    ouch_();
/*<       go to 400 >*/
    goto L400;
/*<   150 devl=dev >*/
L150:
    devl = _BLNK__1.dev;
/*<       jchnge=0 >*/
    jchnge = 0;
/*<       k1=iext(1) >*/
    k1 = _BLNK__1.iext[0];
/*<       knz=iext(nz) >*/
    knz = _BLNK__1.iext[nz - 1];
/*<       klow=0 >*/
    klow = 0;
/*<       nut=-nu >*/
    nut = -nu;
/*<       j=1 >*/
    j = 1;
/*<   200 if(j.eq.nzz) ynz=comp >*/
L200:
    if (j == nzz) {
	ynz = comp;
    }
/*<       if(j.ge.nzz) go to 300 >*/
    if (j >= nzz) {
	goto L300;
    }
/*<       kup=iext(j+1) >*/
    kup = _BLNK__1.iext[j];
/*<       l=iext(j)+1 >*/
    l = _BLNK__1.iext[j - 1] + 1;
/*<       nut=-nut >*/
    nut = -nut;
/*<       if(j.eq.2) y1=comp >*/
    if (j == 2) {
	y1 = comp;
    }
/*<       comp=dev >*/
    comp = _BLNK__1.dev;
/*<       if(l.ge.kup) go to 220 >*/
    if (l >= kup) {
	goto L220;
    }
/*<       err=gee(l,nz) >*/
    err = gee_(&l, &nz);
/*<       err=(err-des(l))*wt(l) >*/
    err = (err - _BLNK__1.des[l - 1]) * _BLNK__1.wt[l - 1];
/*<       dtemp=nut*err-comp >*/
    dtemp = nut * err - comp;
/*<       if(dtemp.le.0.0) go to 220 >*/
    if (dtemp <= 0.) {
	goto L220;
    }
/*<       comp=nut*err >*/
    comp = nut * err;
/*<   210 l=l+1 >*/
L210:
    ++l;
/*<       if(l.ge.kup) go to 215 >*/
    if (l >= kup) {
	goto L215;
    }
/*<       err=gee(l,nz) >*/
    err = gee_(&l, &nz);
/*<       err=(err-des(l))*wt(l) >*/
    err = (err - _BLNK__1.des[l - 1]) * _BLNK__1.wt[l - 1];
/*<       dtemp=nut*err-comp >*/
    dtemp = nut * err - comp;
/*<       if(dtemp.le.0.0) go to 215 >*/
    if (dtemp <= 0.) {
	goto L215;
    }
/*<       comp=nut*err >*/
    comp = nut * err;
/*<       go to 210 >*/
    goto L210;
/*<   215 iext(j)=l-1 >*/
L215:
    _BLNK__1.iext[j - 1] = l - 1;
/*<       j=j+1 >*/
    ++j;
/*<       klow=l-1 >*/
    klow = l - 1;
/*<       jchnge=jchnge+1 >*/
    ++jchnge;
/*<       go to 200 >*/
    goto L200;
/*<   220 l=l-1 >*/
L220:
    --l;
/*<   225 l=l-1 >*/
L225:
    --l;
/*<       if(l.le.klow) go to 250 >*/
    if (l <= klow) {
	goto L250;
    }
/*<       err=gee(l,nz) >*/
    err = gee_(&l, &nz);
/*<       err=(err-des(l))*wt(l) >*/
    err = (err - _BLNK__1.des[l - 1]) * _BLNK__1.wt[l - 1];
/*<       dtemp=nut*err-comp >*/
    dtemp = nut * err - comp;
/*<       if(dtemp.gt.0.0) go to 230 >*/
    if (dtemp > 0.) {
	goto L230;
    }
/*<       if(jchnge.le.0) go to 225 >*/
    if (jchnge <= 0) {
	goto L225;
    }
/*<       go to 260 >*/
    goto L260;
/*<   230 comp=nut*err >*/
L230:
    comp = nut * err;
/*<   235 l=l-1 >*/
L235:
    --l;
/*<       if(l.le.klow) go to 240 >*/
    if (l <= klow) {
	goto L240;
    }
/*<       err=gee(l,nz) >*/
    err = gee_(&l, &nz);
/*<       err=(err-des(l))*wt(l) >*/
    err = (err - _BLNK__1.des[l - 1]) * _BLNK__1.wt[l - 1];
/*<       dtemp=nut*err-comp >*/
    dtemp = nut * err - comp;
/*<       if(dtemp.le.0.0) go to 240 >*/
    if (dtemp <= 0.) {
	goto L240;
    }
/*<       comp=nut*err >*/
    comp = nut * err;
/*<       go to 235 >*/
    goto L235;
/*<   240 klow=iext(j) >*/
L240:
    klow = _BLNK__1.iext[j - 1];
/*<       iext(j)=l+1 >*/
    _BLNK__1.iext[j - 1] = l + 1;
/*<       j=j+1 >*/
    ++j;
/*<       jchnge=jchnge+1 >*/
    ++jchnge;
/*<       go to 200 >*/
    goto L200;
/*<   250 l=iext(j)+1 >*/
L250:
    l = _BLNK__1.iext[j - 1] + 1;
/*<       if(jchnge.ge.0) go to 215 >*/
    if (jchnge >= 0) {
	goto L215;
    }
/*<   255 l=l+1 >*/
L255:
    ++l;
/*<       if(l.ge.kup) go to 260 >*/
    if (l >= kup) {
	goto L260;
    }
/*<       err=gee(l,nz) >*/
    err = gee_(&l, &nz);
/*<       err=(err-des(l))*wt(l) >*/
    err = (err - _BLNK__1.des[l - 1]) * _BLNK__1.wt[l - 1];
/*<       dtemp=nut*err-comp >*/
    dtemp = nut * err - comp;
/*<       if(dtemp.le.0.0) go to 255 >*/
    if (dtemp <= 0.) {
	goto L255;
    }
/*<       comp=nut*err >*/
    comp = nut * err;
/*<       go to 210 >*/
    goto L210;
/*<   260 klow=iext(j) >*/
L260:
    klow = _BLNK__1.iext[j - 1];
/*<       j=j+1 >*/
    ++j;
/*<       go to 200 >*/
    goto L200;
/*<   300 if(j.gt.nzz) go to 320 >*/
L300:
    if (j > nzz) {
	goto L320;
    }
/*<       if(k1.gt.iext(1)) k1=iext(1) >*/
    if (k1 > _BLNK__1.iext[0]) {
	k1 = _BLNK__1.iext[0];
    }
/*<       if(knz.lt.iext(nz)) knz=iext(nz) >*/
    if (knz < _BLNK__1.iext[nz - 1]) {
	knz = _BLNK__1.iext[nz - 1];
    }
/*<       nut1=nut >*/
    nut1 = nut;
/*<       nut=-nu >*/
    nut = -nu;
/*<       l=0 >*/
    l = 0;
/*<       kup=k1 >*/
    kup = k1;
/*<       comp=ynz*(1.00001) >*/
    comp = ynz * 1.00001f;
/*<       luck=1 >*/
    luck = 1;
/*<   310 l=l+1 >*/
L310:
    ++l;
/*<       if(l.ge.kup) go to 315 >*/
    if (l >= kup) {
	goto L315;
    }
/*<       err=gee(l,nz) >*/
    err = gee_(&l, &nz);
/*<       err=(err-des(l))*wt(l) >*/
    err = (err - _BLNK__1.des[l - 1]) * _BLNK__1.wt[l - 1];
/*<       dtemp=nut*err-comp >*/
    dtemp = nut * err - comp;
/*<       if(dtemp.le.0.0) go to 310 >*/
    if (dtemp <= 0.) {
	goto L310;
    }
/*<       comp=nut*err >*/
    comp = nut * err;
/*<       j=nzz >*/
    j = nzz;
/*<       go to 210 >*/
    goto L210;
/*<   315 luck=6 >*/
L315:
    luck = 6;
/*<       go to 325 >*/
    goto L325;
/*<   320 if(luck.gt.9) go to 350 >*/
L320:
    if (luck > 9) {
	goto L350;
    }
/*<       if(comp.gt.y1) y1=comp >*/
    if (comp > y1) {
	y1 = comp;
    }
/*<       k1=iext(nzz) >*/
    k1 = _BLNK__1.iext[nzz - 1];
/*<   325 l=ngrid+1 >*/
L325:
    l = _BLNK__1.ngrid + 1;
/*<       klow=knz >*/
    klow = knz;
/*<       nut=-nut1 >*/
    nut = -nut1;
/*<       comp=y1*(1.00001) >*/
    comp = y1 * 1.00001f;
/*<   330 l=l-1 >*/
L330:
    --l;
/*<       if(l.le.klow) go to 340 >*/
    if (l <= klow) {
	goto L340;
    }
/*<       err=gee(l,nz) >*/
    err = gee_(&l, &nz);
/*<       err=(err-des(l))*wt(l) >*/
    err = (err - _BLNK__1.des[l - 1]) * _BLNK__1.wt[l - 1];
/*<       dtemp=nut*err-comp >*/
    dtemp = nut * err - comp;
/*<       if(dtemp.le.0.0) go to 330 >*/
    if (dtemp <= 0.) {
	goto L330;
    }
/*<       j=nzz >*/
    j = nzz;
/*<       comp=nut*err >*/
    comp = nut * err;
/*<       luck=luck+10 >*/
    luck += 10;
/*<       go to 235 >*/
    goto L235;
/*<   340 if(luck.eq.6) go to 370 >*/
L340:
    if (luck == 6) {
	goto L370;
    }
/*<       do 345 j=1,nfcns >*/
    i__1 = _BLNK__1.nfcns;
    for (j = 1; j <= i__1; ++j) {
/*<   345 iext(nzz-j)=iext(nz-j) >*/
/* L345: */
	_BLNK__1.iext[nzz - j - 1] = _BLNK__1.iext[nz - j - 1];
    }
/*<       iext(1)=k1 >*/
    _BLNK__1.iext[0] = k1;
/*<       go to 100 >*/
    goto L100;
/*<   350 kn=iext(nzz) >*/
L350:
    kn = _BLNK__1.iext[nzz - 1];
/*<       do 360 j=1,nfcns >*/
    i__1 = _BLNK__1.nfcns;
    for (j = 1; j <= i__1; ++j) {
/*<   360 iext(j)=iext(j+1) >*/
/* L360: */
	_BLNK__1.iext[j - 1] = _BLNK__1.iext[j];
    }
/*<       iext(nz)=kn >*/
    _BLNK__1.iext[nz - 1] = kn;
/*<       go to 100 >*/
    goto L100;
/*<   370 if(jchnge.gt.0) go to 100 >*/
L370:
    if (jchnge > 0) {
	goto L100;
    }
/*<   400 continue >*/
L400:
/*<       nm1=nfcns-1 >*/
    nm1 = _BLNK__1.nfcns - 1;
/*<       fsh=1.0e-06 >*/
    fsh = 1e-6f;
/*<       gtemp=grid(1) >*/
    gtemp = _BLNK__1.grid[0];
/*<       x(nzz)=-2.0 >*/
    _BLNK__1.x[nzz - 1] = -2.f;
/*<       cn=2.*nfcns-1. >*/
    cn = _BLNK__1.nfcns * 2.f - 1.f;
/*<       delf=1.0/cn >*/
    delf = 1.f / cn;
/*<       l=1 >*/
    l = 1;
/*<       kkk=0 >*/
    kkk = 0;
/*<       if(edge(1).eq.0.0.and.edge(2*nbands).eq.0.5) kkk=1 >*/
    if (edge[1] == 0.f && edge[*nbands * 2] == .5f) {
	kkk = 1;
    }
/*<       if(nfcns.le.3) kkk=1 >*/
    if (_BLNK__1.nfcns <= 3) {
	kkk = 1;
    }
/*<       if(kkk.eq.1) go to 405 >*/
    if (kkk == 1) {
	goto L405;
    }
/*<       dtemp=dcos(pi2*grid(1)) >*/
    dtemp = cos(_BLNK__1.pi2 * _BLNK__1.grid[0]);
/*<       dnum=dcos(pi2*grid(ngrid)) >*/
    dnum = cos(_BLNK__1.pi2 * _BLNK__1.grid[_BLNK__1.ngrid - 1]);
/*<       aa=2.0/(dtemp-dnum) >*/
    aa = 2.f / (dtemp - dnum);
/*<       bb=-(dtemp+dnum)/(dtemp-dnum) >*/
    bb = -(dtemp + dnum) / (dtemp - dnum);
/*<   405 continue >*/
L405:
/*<       do 430 j=1,nfcns >*/
    i__1 = _BLNK__1.nfcns;
    for (j = 1; j <= i__1; ++j) {
/*<       ft=(j-1)*delf >*/
	ft = (j - 1) * delf;
/*<       xt=dcos(pi2*ft) >*/
	xt = cos(_BLNK__1.pi2 * ft);
/*<       if(kkk.eq.1) go to 410 >*/
	if (kkk == 1) {
	    goto L410;
	}
/*<       xt=(xt-bb)/aa >*/
	xt = (xt - bb) / aa;
/*<       ft=acos(xt)/pi2 >*/
	ft = acos(xt) / _BLNK__1.pi2;
/*<   410 xe=x(l) >*/
L410:
	xe = _BLNK__1.x[l - 1];
/*<       if(xt.gt.xe) go to 420 >*/
	if (xt > xe) {
	    goto L420;
	}
/*<       if((xe-xt).lt.fsh) go to 415 >*/
	if (xe - xt < fsh) {
	    goto L415;
	}
/*<       l=l+1 >*/
	++l;
/*<       go to 410 >*/
	goto L410;
/*<   415 a(j)=y(l) >*/
L415:
	a[j - 1] = _BLNK__1.y[l - 1];
/*<       go to 425 >*/
	goto L425;
/*<   420 if((xt-xe).lt.fsh) go to 415 >*/
L420:
	if (xt - xe < fsh) {
	    goto L415;
	}
/*<       grid(1)=ft >*/
	_BLNK__1.grid[0] = ft;
/*<       a(j)=gee(1,nz) >*/
	a[j - 1] = gee_(&c__1, &nz);
/*<   425 continue >*/
L425:
/*<       if(l.gt.1) l=l-1 >*/
	if (l > 1) {
	    --l;
	}
/*<   430 continue >*/
/* L430: */
    }
/*<       grid(1)=gtemp >*/
    _BLNK__1.grid[0] = gtemp;
/*<       dden=pi2/cn >*/
    dden = _BLNK__1.pi2 / cn;
/*<       do 510 j=1,nfcns >*/
    i__1 = _BLNK__1.nfcns;
    for (j = 1; j <= i__1; ++j) {
/*<       dtemp=0.0 >*/
	dtemp = 0.f;
/*<       dnum=(j-1)*dden >*/
	dnum = (j - 1) * dden;
/*<       if(nm1.lt.1) go to 505 >*/
	if (nm1 < 1) {
	    goto L505;
	}
/*<       do 500 k=1,nm1 >*/
	i__2 = nm1;
	for (k = 1; k <= i__2; ++k) {
/*<   500 dtemp=dtemp+a(k+1)*dcos(dnum*k) >*/
/* L500: */
	    dtemp += a[k] * cos(dnum * k);
	}
/*<   505 dtemp=2.0*dtemp+a(1) >*/
L505:
	dtemp = dtemp * 2.f + a[0];
/*<   510 alpha(j)=dtemp >*/
/* L510: */
	_BLNK__1.alpha[j - 1] = dtemp;
    }
/*<       do 550 j=2,nfcns >*/
    i__1 = _BLNK__1.nfcns;
    for (j = 2; j <= i__1; ++j) {
/*<   550 alpha(j)=2.*alpha(j)/cn >*/
/* L550: */
	_BLNK__1.alpha[j - 1] = _BLNK__1.alpha[j - 1] * 2.f / cn;
    }
/*<       alpha(1)=alpha(1)/cn >*/
    _BLNK__1.alpha[0] /= cn;
/*<       if(kkk.eq.1) go to 545 >*/
    if (kkk == 1) {
	goto L545;
    }
/*<       p(1)=2.0*alpha(nfcns)*bb+alpha(nm1) >*/
    p[0] = _BLNK__1.alpha[_BLNK__1.nfcns - 1] * 2.f * bb + _BLNK__1.alpha[nm1 
	    - 1];
/*<       p(2)=2.0*aa*alpha(nfcns) >*/
    p[1] = aa * 2.f * _BLNK__1.alpha[_BLNK__1.nfcns - 1];
/*<       q(1)=alpha(nfcns-2)-alpha(nfcns) >*/
    q[0] = _BLNK__1.alpha[_BLNK__1.nfcns - 3] - _BLNK__1.alpha[_BLNK__1.nfcns 
	    - 1];
/*<       do 540 j=2,nm1 >*/
    i__1 = nm1;
    for (j = 2; j <= i__1; ++j) {
/*<       if(j.lt.nm1) go to 515 >*/
	if (j < nm1) {
	    goto L515;
	}
/*<       aa=0.5*aa >*/
	aa *= .5f;
/*<       bb=0.5*bb >*/
	bb *= .5f;
/*<   515 continue >*/
L515:
/*<       p(j+1)=0.0 >*/
	p[j] = 0.f;
/*<       do 520 k=1,j >*/
	i__2 = j;
	for (k = 1; k <= i__2; ++k) {
/*<       a(k)=p(k) >*/
	    a[k - 1] = p[k - 1];
/*<   520 p(k)=2.0*bb*a(k) >*/
/* L520: */
	    p[k - 1] = bb * 2.f * a[k - 1];
	}
/*<       p(2)=p(2)+a(1)*2.0*aa >*/
	p[1] += a[0] * 2.f * aa;
/*<       jm1=j-1 >*/
	jm1 = j - 1;
/*<       do 525 k=1,jm1 >*/
	i__2 = jm1;
	for (k = 1; k <= i__2; ++k) {
/*<   525 p(k)=p(k)+q(k)+aa*a(k+1) >*/
/* L525: */
	    p[k - 1] = p[k - 1] + q[k - 1] + aa * a[k];
	}
/*<       jp1=j+1 >*/
	jp1 = j + 1;
/*<       do 530 k=3,jp1 >*/
	i__2 = jp1;
	for (k = 3; k <= i__2; ++k) {
/*<   530 p(k)=p(k)+aa*a(k-1) >*/
/* L530: */
	    p[k - 1] += aa * a[k - 2];
	}
/*<       if(j.eq.nm1) go to 540 >*/
	if (j == nm1) {
	    goto L540;
	}
/*<       do 535 k=1,j >*/
	i__2 = j;
	for (k = 1; k <= i__2; ++k) {
/*<   535 q(k)=-a(k) >*/
/* L535: */
	    q[k - 1] = -a[k - 1];
	}
/*<       q(1)=q(1)+alpha(nfcns-1-j) >*/
	q[0] += _BLNK__1.alpha[_BLNK__1.nfcns - 1 - j - 1];
/*<   540 continue >*/
L540:
	;
    }
/*<       do 543 j=1,nfcns >*/
    i__1 = _BLNK__1.nfcns;
    for (j = 1; j <= i__1; ++j) {
/*<   543 alpha(j)=p(j) >*/
/* L543: */
	_BLNK__1.alpha[j - 1] = p[j - 1];
    }
/*<   545 continue >*/
L545:
/*<       if(nfcns.gt.3) return >*/
    if (_BLNK__1.nfcns > 3) {
	return 0;
    }
/*<       alpha(nfcns+1)=0.0 >*/
    _BLNK__1.alpha[_BLNK__1.nfcns] = 0.f;
/*<       alpha(nfcns+2)=0.0 >*/
    _BLNK__1.alpha[_BLNK__1.nfcns + 1] = 0.f;
/*<       return >*/
    return 0;
/*<       end >*/
} /* remez_ */


/*<       Double Precision Function D(k,n,m) >*/
doublereal d_(integer *k, integer *n, integer *m)
{
    /* System generated locals */
    integer i__1, i__2, i__3;
    doublereal ret_val;

    /* Local variables */
    static integer j, l;
    static doublereal q;

/*<       double precision ad,dev,x,y,q,pi2 >*/
/*<       common des,wt,alpha,iext,nfcns,ngrid,pi2,ad,dev,x,y,grid >*/
/*<       dimension iext(100),ad(100),alpha(100),x(100),y(100) >*/
/*<       dimension des(1600),grid(1600),wt(1600) >*/
/*<       d=1.0 >*/
    ret_val = 1.f;
/*<       q=x(k) >*/
    q = _BLNK__1.x[*k - 1];
/*<       do 3 l=1,m >*/
    i__1 = *m;
    for (l = 1; l <= i__1; ++l) {
/*<       do 2 j=l,n,m >*/
	i__2 = *n;
	i__3 = *m;
	for (j = l; i__3 < 0 ? j >= i__2 : j <= i__2; j += i__3) {
/*<       if(j-k)1,2,1 >*/
	    if (j - *k != 0) {
		goto L1;
	    } else {
		goto L2;
	    }
/*<     1 d=2.0*d*(q-x(j)) >*/
L1:
	    ret_val = ret_val * 2.f * (q - _BLNK__1.x[j - 1]);
/*<     2 continue >*/
L2:
	    ;
	}
/*<     3 continue >*/
/* L3: */
    }
/*<       d=1.0/d >*/
    ret_val = 1.f / ret_val;
/*<       return >*/
    return ret_val;
/*<       end >*/
} /* d_ */


/*<       Function Gee(k,n) >*/
doublereal gee_(integer *k, integer *n)
{
    /* System generated locals */
    integer i__1;
    real ret_val;

    /* Builtin functions */
    double cos(doublereal);

    /* Local variables */
    static doublereal c, d;
    static integer j;
    static doublereal p, xf;

/*<       double precision p,c,d,xf,pi2,ad,dev,x,y >*/
/*<       common des,wt,alpha,iext,nfcns,ngrid,pi2,ad,dev,x,y,grid >*/
/*<       dimension iext(100),ad(100),alpha(100),x(100),y(100) >*/
/*<       dimension des(1600),grid(1600),wt(1600) >*/
/*<       p=0.0 >*/
    p = 0.f;
/*<       xf=grid(k) >*/
    xf = _BLNK__1.grid[*k - 1];
/*<       xf=dcos(pi2*xf) >*/
    xf = cos(_BLNK__1.pi2 * xf);
/*<       d=0.0 >*/
    d = 0.f;
/*<       do 1 j=1,n >*/
    i__1 = *n;
    for (j = 1; j <= i__1; ++j) {
/*<       c=xf-x(j) >*/
	c = xf - _BLNK__1.x[j - 1];
/*<       c=ad(j)/c >*/
	c = _BLNK__1.ad[j - 1] / c;
/*<       d=d+c >*/
	d += c;
/*<     1 p=p+c*y(j) >*/
/* L1: */
	p += c * _BLNK__1.y[j - 1];
    }
/*<       gee=p/d >*/
    ret_val = p / d;
/*<       return >*/
    return ret_val;
/*<       end >*/
} /* gee_ */


/*<       Subroutine Ouch >*/
/* Subroutine */ int ouch_(void)
{
/*     write (*,'(/a)') ' ********** FAILURE TO CONVERGE **********' */
/*     write (*,*) 'Probable cause is machine rounding error' */
/*     write (*,*) 'The impulse response may still be correct' */
/*<       return >*/
    return 0;
/*<       end >*/
} /* ouch_ */

/* Main program alias */ int optfir_ () { MAIN__ (); return 0; }
