c @(#)wfir.f	1.2	1/11/91
c main program: window design of linear phase, lowpass, highpass
c               bandpass, and bandstop fir digital filters
c author:       lawrence r. rabiner and carol a. mcgonegal
c               bell laboratories, murray hill, new jersey, 07974
c modified jan. 1978 by doug paul, mit lincoln laboratories
c to include subroutines for obtaining filter band edges and ripples
c
c modified july. 1990 by Edward Lee, UCB, for Gabriel compatibility
c
c input:        nf is the filter length in samples
c                   3 <= nf <= 1024
c
c               itype is the window type
c                   itype = 1     rectangular window
c                   itype = 2     triangular window
c                   itype = 3     hamming window
c                   itype = 4     generalized hamming window
c                   itype = 5     hanning window
c                   itype = 6     kaiser (i0-sinh) window
c                   itype = 7     chebyshev window
c
c               jtype is the filter type
c                   jtype = 1     lowpass filter
c                   jtype = 2     highpass filter
c                   jtype = 3     bandpass filter
c                   jtype = 4     bandstop filter
c
c               fc is the normalized cutoff frequency
c                   0 <= fc <= 0.5
c               fl and fh are the normalized filter cutoff frequencies
c                   0 <= fl <= fh <= 0.5
c               iwp optionally prints out the window values
c                   iwp = 0  do not print
c                   iwp = 1  print
c               imd requests additional runs
c                   imd = 1   new run
c                   imd = 0   terminates program
c-----------------------------------------------------------------------
c
      dimension w(512), g(512)
      integer otcd1, otcd2
      character fname*100
      character answer*1
c
      pi = 4.0*atan(1.0)
      twopi = 2.0*pi
c
c define i/o device codes
c input: input to this program is user-interactive
c        that is - a question is written on the user
c        terminal (otcd1) and the user types in the answer.
c
c output: standard output is otcd1 and otcd2
c
c      otcd1 = i1mach(4)
      otcd1 = 6
c      otcd2 = i1mach(2)
      otcd2 = 6
c
      write (*,'(,a)') ' PROVISIONAL WINDOW FIR FILTER DESIGN '
      write (*,'(,a)') ' USE AT YOUR OWN RISK --------------- '
c
c input the filter length(nf), window type(itype) and filter type(jtype)
c
  10  write (*,'(/,a)') ' Enter name of input command file (press <Enter
     +> for manual entry, '
      write (*,'(,a\)') ' Sorry, no tilde-expansion.  Give path relative
     + to your home or startup directory): '
      read (*,'(a12)') fname
      if (fname .eq. ' ') then
         incod = 5
      else
         incod = 3
         open (3, file=fname)
      endif
c
  15  write (*,'(/,a)') ' Enter filter length: '
      read (incod,*) nf
      if (nf.le.1024) go to 30
  20  write (otcd2,9997) nf
9997  format (16h filter length =, i4, 17h is out of bounds)
      go to 15
  30  write (*,'(,a,/,10x,a)') ' Enter window type (1=Rectangular, 2=Tri
     +angular,', '3=Hamming, 4=Gen. Hamming, 5=Hanning, 6=Kaiser, 7=Cheb
     +yshev): '
      read (incod,*) itype
  35  if (itype.ne.7 .and. nf.lt.3) go to 20
      if (itype.eq.7 .and. (nf.eq.1 .or. nf.eq.2)) go to 20
      write (*,'(,a,/,20x,a)') ' Enter filter type (1=Lowpass, 2=Highpas
     +s,', '3=Bandpass, 4=Bandstop): '
      read (incod,*) jtype
c
c n is half the length of the symmetric filter
c
      n = (nf+1)/2
      if (jtype.ne.1 .and. jtype.ne.2) go to 50
c
c for the ideal lowpass or highpass design - input fc
c
  40  write (*,'(,a)') ' Enter cutoff frequency (as a fraction of sample
     + frequency): '
      read (incod,*) fc
      if (fc.gt.0.0 .and. fc.lt.0.5) go to 60
      write (otcd1,9995) fc
9995  format (13h Cutoff freq=, f14.7, 29h is out of bounds, reenter da,
     *    2hta)
      go to 40
c
c for the ideal bandpass or bandstop design - input fl and fh
c
  50  write (*,'(,a)') ' Enter lower cutoff frequency (as a fraction of 
     +sample frequency): '
      read (incod,*) fl
      write (*,'(,a)') ' Enter upper cutoff frequency (as a fraction of 
     +sample frequency): '
      read (incod,*) fh
      if (fl.gt.0.0 .and. fl.lt.0.5 .and. fh.gt.0.0 .and. fh.lt.0.5
     *    .and. fh.gt.fl) go to 60
      if (fl.lt.0. .or. fl.gt.0.5) write (otcd1,9995) fl
      if (fh.lt.0. .or. fh.gt.0.5) write (otcd1,9995) fh
      if (fh.lt.fl) write (otcd1,9992) fh, fl
9992  format (4h fh=, f14.7, 20h is smaller than fl=, f14.7, 8h reenter,
     *    5h data)
      go to 50
  60  if (itype.ne.7) go to 70
c
c input for chebyshev window--2 of the 3 parameters nf, dplog, and df
c must be specified, where dplog is the desired filter ripple(db scale),
c df is the transition width (normalized) of the filter,
c and nf is the filter length.  the unspecified parameter
c is read in with the zero value.
c
      write (*,'(,a)') ' Enter chebyshev ripple (in db), and optional tr
     +ansition width,'
      write (*,'(,a)') ' separated by a comma, using decimal points: '
      read (incod,9993) dplog, df
9993  format (2f14.7)
      dp = 10.0**(-dplog/20.0)
      call chebc(nf, dp, df, n, x0, xn)
c
c ieo is an even, odd indicator, ieo = 0 for even, ieo = 1 for odd
c
  70  ieo = mod(nf,2)
c
c   The following seemed wrong: had 1 instead of 4.  Even length
c   lowpass filters could result in the original.
c
      if (ieo.eq.1 .or. jtype.eq.4 .or. jtype.eq.3) go to 80
      write (*,'(,a)') ' Order must be odd for highpass of lowpass filte
     +rs -- being increased by 1. '
      nf = nf + 1
      n = (1+nf)/2
      ieo = 1
  80  continue
c
c compute ideal (unwindowed) impulse response for filter
c
      c1 = fc
      if (jtype.eq.3 .or. jtype.eq.4) c1 = fh - fl
      if (ieo.eq.1) g(1) = 2.*c1
      i1 = ieo + 1
      do 90 i=i1,n
        xn = i - 1
        if (ieo.eq.0) xn = xn + 0.5
        c = pi*xn
        c3 = c*c1
        if (jtype.eq.1 .or. jtype.eq.2) c3 = 2.*c3
        g(i) = sin(c3)/c
        if (jtype.eq.3 .or. jtype.eq.4) g(i) = g(i)*2.*cos(c*(fl+fh))
  90  continue
c
c compute a rectangular window
c
      if (itype.eq.1) write (otcd2,9989) nf
9989  format (23h rectangular window-nf=, i4)
      do 100 i=1,n
        w(i) = 1.
 100  continue
c
c dispatch on window type
c
      go to (200, 110, 120, 140, 150, 160, 170), itype
c
c triangular window
c
 110  call triang(nf, w, n, ieo)
      write (otcd2,9988) nf
9988  format (22h triangular window-nf=, i4)
      go to 180
c
c hamming window
c
 120  alpha = 0.54
      write (otcd2,9987) nf
9987  format (23h hamming window length=, i4)
 130  beta = 1. - alpha
      call hammin(nf, w, n, ieo, alpha, beta)
      write (otcd2,9986) alpha
9986  format (7h alpha=, f14.7)
      go to 180
c
c generalized hamming window
c form of window is w(m)=alpha+beta*cos((twopi*m)/(nf-1))
c beta is automatically set to 1.-alpha
c read in alpha
c
 140  write (otcd1,9985)
9985  format (45h specify alpha for generalized hamming window)
      read (incod,9993) alpha
      write (otcd2,9984) nf
9984  format (35h generalized hamming window length=, i4)
      go to 130
c
c hanning window
c
 150  alpha = 0.5
      write (otcd2,9983) nf
9983  format (23h hanning window length=, i4)
c
c increase nf by 2 and n by 1 for hanning window so zero
c endpoints are not part of window
c
      nf = nf + 2
      n = n + 1
      go to 130
c
c kaiser (i0-sinh) window
c need to specify parameter att=stopband attenuation in db
c
 160  write (otcd1,9982)
9982  format (33h specify attenuation in db(f14.7), 16h for kaiser wind,
     *    2how)
      read (incod,9993) att
      if (att.gt.50.) beta = 0.1102*(att-8.7)
      if (att.ge.20.96 .and. att.le.50.) beta = 0.58417*(att-20.96)**
     *    0.4 + 0.07886*(att-20.96)
      if (att.lt.20.96) beta = 0.
      call kaiser(nf, w, n, ieo, beta)
      write (otcd2,9981) nf
9981  format (22h kaiser window length=, i4)
      write (otcd2,9980) att, beta
9980  format (14h  attenuation=, f14.7, 7h  beta=, f14.7)
      go to 180
c
c chebyshev window
c
 170  call cheby(nf, w, n, ieo, dp, df, x0, xn)
      write (otcd2,9979) nf
9979  format (25h Chebyshev window length=, i4)
      write (otcd2,9978) dp, df
9978  format (8h ripple=, f14.7, 19h  transition width=, f14.7)
c
c window ideal filter response
c change back nf and n for hanning window
c
 180  if (itype.eq.5) nf = nf - 2
      if (itype.eq.5) n = n - 1
      do 190 i=1,n
        g(i) = g(i)*w(i)
 190  continue
c
c optionally print out results
c
 200  write (*,'(/,a\)') ' Print window values? (y/n): '
      read (*,'(a1)') answer
      if (answer .eq. 'n' .or. answer .eq. 'N') go to 220
      write (otcd2,9975)
9975  format (14h window values)
      do 210 i=1,n
        j = n + 1 - i
        k = nf + 1 - i
        write (otcd2,9974) i, w(j), k
9974    format (10x, 3h w(, i3, 2h)=, e15.8, 4h =w(, i4, 1h))
 210  continue
 220  if (jtype.eq.1) write (otcd2,9973)
9973  format (26h **lowpass filter design**)
      if (jtype.eq.2) write (otcd2,9972)
9972  format (27h **highpass filter design**)
      if (jtype.eq.3) write (otcd2,9971)
9971  format (27h **bandpass filter design**)
      if (jtype.eq.4) write (otcd2,9970)
9970  format (27h **bandstop filter design**)
      if (jtype.eq.1) write (otcd2,9969) fc
9969  format (22h ideal lowpass cutoff=, f14.7)
      if (jtype.eq.2) write (otcd2,9968) fc
9968  format (23h ideal highpass cutoff=, f14.7)
      if (jtype.eq.3 .or. jtype.eq.4) write (otcd2,9967) fl, fh
9967  format (26h ideal cutoff frequencies=, 2f14.7)
      if (jtype.eq.1 .or. jtype.eq.3) go to 240
      do 230 i=2,n
        g(i) = -g(i)
 230  continue
      g(1) = 1.0 - g(1)
c
c write out impulse response
c
c
c
 240  do 250 i=1,n
        j = n + 1 - i
        k = nf + 1 - i
        write (otcd2,9966) i, g(j), k
9966    format (10x, 3h h(, i3, 2h)=, e15.8, 4h =h(, i4, 1h))
 250  continue
      call flchar(nf, itype, jtype, fc, fl, fh, n, ieo, g, otcd2)
      write (otcd2,9965)
9965  format (1h /)
c
c   Open output file
c
      ioutd = 1
      write (*,'(,a)') ' Enter name of window values output file or <ent
     +er> '
      write (*,'(,a)') ' (Sorry, no tilde-expansion.  Give path relative
     + to your home directory) '
      read (incod,'(a)') fname
      if (fname .eq. ' ') go to 260
      open (ioutd, file=fname)
      call outimp(g,n,nf,ieo,ioutd)
      call flush(ioutd)
      close(ioutd)
 260  close(3)
      write (*,'(/,a\)') ' Another design? (y/n): '
      read (*,'(a1)') answer
      if (answer .eq. 'y' .or. answer .eq. 'Y') go to 10
      stop 'End of program'
      end
c
c-----------------------------------------------------------------------
c subroutine:  triang
c triangular window
c-----------------------------------------------------------------------
c
      subroutine triang(nf, w, n, ieo)
c
c  nf = filter length in samples
c   w = window coefficients for half the window
c   n = half window length=(nf+1)/2
c ieo = even - odd indication--ieo=0 for nf even
c
      dimension w(1)
      fn = n
      do 10 i=1,n
        xi = i - 1
        if (ieo.eq.0) xi = xi + 0.5
        w(i) = 1. - xi/fn
  10  continue
      return
      end
c
c-----------------------------------------------------------------------
c subroutine:  hammin
c generalized hamming window routine
c window is w(n) = alpha + beta * cos( twopi*(n-1) / (nf-1) )
c-----------------------------------------------------------------------
c
      subroutine hammin(nf, w, n, ieo, alpha, beta)
c
c    nf = filter length in samples
c     w = window array of size n
c     n = half length of filter=(nf+1)/2
c   ieo = even odd indicator--ieo=0 if nf even
c alpha = constant of window
c  beta = constant of window--generally beta=1-alpha
c
      dimension w(1)
      pi2 = 8.0*atan(1.0)
      fn = nf - 1
      do 10 i=1,n
        fi = i - 1
        if (ieo.eq.0) fi = fi + 0.5
        w(i) = alpha + beta*cos((pi2*fi)/fn)
  10  continue
      return
      end
c
c-----------------------------------------------------------------------
c subroutine:  kaiser
c kaiser window
c-----------------------------------------------------------------------
c
      subroutine kaiser(nf, w, n, ieo, beta)
c
c   nf = filter length in samples
c    w = window array of size n
c    n = filter half length=(nf+1)/2
c  ieo = even odd indicator--ieo=0 if nf even
c beta = parameter of kaiser window
c
      dimension w(1)
      real ino
      bes = ino(beta)
      xind = float(nf-1)*float(nf-1)
      do 10 i=1,n
        xi = i - 1
        if (ieo.eq.0) xi = xi + 0.5
        xi = 4.*xi*xi
        w(i) = ino(beta*sqrt(1.-xi/xind))
        w(i) = w(i)/bes
  10  continue
      return
      end
c
c-----------------------------------------------------------------------
c function:  ino
c bessel function for kaiser window
c-----------------------------------------------------------------------
c
      real function ino(x)
      y = x/2.
      t = 1.e-08
      e = 1.
      de = 1.
      do 10 i=1,25
        xi = i
        de = de*y/xi
        sde = de*de
        e = e + sde
        if (e*t-sde) 10, 10, 20
  10  continue
  20  ino = e
      return
      end
c
c-----------------------------------------------------------------------
c subroutine:  chebc
c subroutine to generate chebyshev window parameters when
c one of the three parameters nf,dp and df is unspecified
c-----------------------------------------------------------------------
c
      subroutine chebc(nf, dp, df, n, x0, xn)
c
c nf = filter length (in samples)
c dp = filter ripple (absolute scale)
c df = normalized transition width of filter
c  n = (nf+1)/2 = filter half length
c x0 = (3-c0)/(1+c0) with c0=cos(pi*df) = chebyshev window constant
c xn = nf-1
c
      pi = 4.*atan(1.0)
      if (nf.ne.0) go to 10
c
c dp,df specified, determine nf
c
      c1 = coshin((1.+dp)/dp)
      c0 = cos(pi*df)
      x = 1. + c1/coshin(1./c0)
c
c increment by 1 to give nf which meets or exceeds specs on dp and df
c
      nf = x + 1.0
      n = (nf+1)/2
      xn = nf - 1
      go to 30
  10  if (df.ne.0.0) go to 20
c
c nf,dp specified, determine df
c
      xn = nf - 1
      c1 = coshin((1.+dp)/dp)
      c2 = cosh(c1/xn)
      df = arccos(1./c2)/pi
      go to 30
c
c nf,df specified, determine dp
c
  20  xn = nf - 1
      c0 = cos(pi*df)
      c1 = xn*coshin(1./c0)
      dp = 1./(cosh(c1)-1.)
  30  x0 = (3.-cos(2.*pi*df))/(1.+cos(2.*pi*df))
      return
      end
c
c-----------------------------------------------------------------------
c subroutine:  cheby
c dolph chebyshev window design
c-----------------------------------------------------------------------
c
      subroutine cheby(nf, w, n, ieo, dp, df, x0, xn)
c
c  nf = filter length in samples
c   w = window array of size n
c   n = half length of filter = (nf+1)/2
c ieo = even-odd indicator--ieo=0 for nf even
c  dp = window ripple on an absolute scale
c  df = normalized transition width of window
c  x0 = window parameter related to transition width
c  xn = nf-1
c
      dimension w(1)
      dimension pr(1024), pi(1024)
      pie = 4.*atan(1.0)
      xn = nf - 1
      fnf = nf
      alpha = (x0+1.)/2.
      beta = (x0-1.)/2.
      twopi = 2.*pie
      c2 = xn/2.
      do 40 i=1,nf
        xi = i - 1
        f = xi/fnf
        x = alpha*cos(twopi*f) + beta
        if (abs(x)-1.) 10, 10, 20
  10    p = dp*cos(c2*arccos(x))
        go to 30
  20    p = dp*cosh(c2*coshin(x))
  30    pi(i) = 0.
        pr(i) = p
c
c for even length filters use a one-half sample delay
c also the frequency response is antisymmetric in frequency
c
        if (ieo.eq.1) go to 40
        pr(i) = p*cos(pie*f)
        pi(i) = -p*sin(pie*f)
        if (i.gt.(nf/2+1)) pr(i) = -pr(i)
        if (i.gt.(nf/2+1)) pi(i) = -pi(i)
  40  continue
c
c use dft to give window
c
      twn = twopi/fnf
      do 60 i=1,n
        xi = i - 1
        sum = 0.
        do 50 j=1,nf
          xj = j - 1
          sum = sum + pr(j)*cos(twn*xj*xi) + pi(j)*sin(twn*xj*xi)
  50    continue
        w(i) = sum
  60  continue
      c1 = w(1)
      do 70 i=1,n
        w(i) = w(i)/c1
  70  continue
      return
      end
c
c-----------------------------------------------------------------------
c function:  coshin
c function for hyperbolic inverse cosine of x
c-----------------------------------------------------------------------
c
      real function coshin(x)
      coshin = alog(x+sqrt(x*x-1.))
      return
      end
c
c-----------------------------------------------------------------------
c function:  arccos
c function for inverse cosine of x
c-----------------------------------------------------------------------
c
      function arccos(x)
      if (x) 30, 20, 10
  10  a = sqrt(1.-x*x)/x
      arccos = atan(a)
      return
  20  arccos = 2.*atan(1.0)
      return
  30  a = sqrt(1.-x*x)/x
      arccos = atan(a) + 4.*atan(1.0)
      return
      end
c
c-----------------------------------------------------------------------
c function:  cosh
c function for hyperbolic cosine of x
c-----------------------------------------------------------------------
c
      real function cosh(x)
      cosh = (exp(x)+exp(-x))/2.
      return
      end
c
c-----------------------------------------------------------------------
c subroutine:  flchar
c subroutine to determine filter characteristics
c-----------------------------------------------------------------------
c
      subroutine flchar(nf, itype, jtype, fc, fl, fh, n, ieo, g, otcd2)
c
c    nf = filter length in samples
c itype = window type
c jtype = filter type
c    fc = ideal cutoff of lp or hp filter
c    fl = lower cutoff of bp or bs filter
c    fh = upper cutoff of bp or bs filter
c     n = filter half length = (nf+1) / 2
c   ieo = even odd indicator
c     g = filter array of size n
c otcd2 = output code for line printer used in write statements
c
      dimension g(1)
      dimension resp(2048)
      integer otcd2
c
c not for for triangular window
c
      if (itype.eq.2) return
c
c dft to get freq resp
c
      pi = 4.*atan(1.0)
c
c up to 4096 pt dft
c
      nr = 8*nf
      if (nr.gt.2048) nr = 2048
      xnr = nr
      twn = pi/xnr
      sumi = -g(1)/2.
      if (ieo.eq.0) sumi = 0.
      do 20 i=1,nr
        xi = i - 1
        twni = twn*xi
        sum = sumi
        do 10 j=1,n
          xj = j - 1
          if (ieo.eq.0) xj = xj + .5
          sum = sum + g(j)*cos(xj*twni)
  10    continue
        resp(i) = 2.*sum
  20  continue
c
c dispatch on filter type
c
      go to (30, 40, 50, 60), jtype
c
c lowpass
c
  30  call ripple(nr, 1., 0., fc, resp, f1, f2, db)
      write (otcd2,9999) f2, db
9999  format (17h passband cutoff , f6.4, 9h  ripple , f8.3, 3h db)
      call ripple(nr, 0., fc, .5, resp, f1, f2, db)
      write (otcd2,9998) f1, db
9998  format (17h stopband cutoff , f6.4, 9h  ripple , f8.3, 3h db)
      return
c
c highpass
c
  40  call ripple(nr, 0., 0., fc, resp, f1, f2, db)
      write (otcd2,9998) f2, db
      call ripple(nr, 1., fc, .5, resp, f1, f2, db)
      write (otcd2,9999) f1, db
      return
c
c bandpass
c
  50  call ripple(nr, 0., 0., fl, resp, f1, f2, db)
      write (otcd2,9998) f2, db
      call ripple(nr, 1., fl, fh, resp, f1, f2, db)
      write (otcd2,9997) f1, f2, db
9997  format (18h passband cutoffs , f6.4, 2x, f6.4, 8h  ripple, f9.3,
     *    3h db)
      call ripple(nr, 0., fh, .5, resp, f1, f2, db)
      write (otcd2,9998) f1, db
      return
c
c stopband
c
  60  call ripple(nr, 1., 0., fl, resp, f1, f2, db)
      write (otcd2,9999) f2, db
      call ripple(nr, 0., fl, fh, resp, f1, f2, db)
      write (otcd2,9996) f1, f2, db
9996  format (18h stopband cutoffs , f6.4, 2x, f6.4, 8h  ripple, f9.3,
     *    3h db)
      call ripple(nr, 1., fh, .5, resp, f1, f2, db)
      write (otcd2,9999) f1, db
      return
      end
c
c-----------------------------------------------------------------------
c subroutine:  ripple
c finds largest ripple in band and locates band edges based on the
c point where the transition region crosses the measured ripple bound
c-----------------------------------------------------------------------
c
      subroutine ripple(nr, rideal, flow, fhi, resp, f1, f2, db)
c
c     nr = size of resp
c rideal = ideal frequency response
c   flow = low edge of ideal band
c    fhi = high edge of ideal band
c   resp = frequency response of size nr
c     f1 = computed lower band edge
c     f2 = computed upper band edge
c     db = deviation from ideal response in db
c
      dimension resp(1)
      xnr = nr
c
c band limits
c
      iflow = 2.*xnr*flow + 1.5
      ifhi = 2.*xnr*fhi + 1.5
      if (iflow.eq.0) iflow = 1
      if (ifhi.ge.nr) ifhi = nr - 1
c
c find max and min peaks in band
c
      rmin = rideal
      rmax = rideal
      do 20 i=iflow,ifhi
        if (resp(i).le.rmax .or. resp(i).lt.resp(i-1) .or.
     *      resp(i).lt.resp(i+1)) go to 10
        rmax = resp(i)
  10    if (resp(i).ge.rmin .or. resp(i).gt.resp(i-1) .or.
     *      resp(i).gt.resp(i+1)) go to 20
        rmin = resp(i)
  20  continue
c
c peak deviation from ideal
c
      ripl = amax1(rmax-rideal,rideal-rmin)
c
c search for lower band edge
c
      f1 = flow
      if (flow.eq.0.0) go to 50
      do 30 i=iflow,ifhi
        if (abs(resp(i)-rideal).le.ripl) go to 40
  30  continue
  40  xi = i - 1
c
c linear interpolation of band edge frequency to improve accuracy
c
      x1 = .5*xi/xnr
      x0 = .5*(xi-1.)/xnr
      y1 = abs(resp(i)-rideal)
      y0 = abs(resp(i-1)-rideal)
      f1 = (x1-x0)/(y1-y0)*(ripl-y0) + x0
c
c search for upper band edge
c
  50  f2 = fhi
      if (fhi.eq.0.5) go to 80
      do 60 i=iflow,ifhi
        j = ifhi + iflow - i
        if (abs(resp(j)-rideal).le.ripl) go to 70
  60  continue
  70  xi = j - 1
c
c linear interpolation of band edge frequency to improve accuracy
c
      x1 = .5*xi/xnr
      x0 = .5*(xi+1.)/xnr
      y1 = abs(resp(j)-rideal)
      y0 = abs(resp(j+1)-rideal)
      f2 = (x1-x0)/(y1-y0)*(ripl-y0) + x0
c
c deviation from ideal in db
c
  80  db = 20.*alog10(ripl+rideal)
      return
      end
c
c-----------------------------------------------------------------------

c
c-----------------------------------------------------------------------
c subroutine: outimp
c    Prints out on unit ioutd the full
c    impulse response as format 13x,f15.8.  This makes the impulse
c    response accessible to other programs without the need for parsing
c    the output of the program.
c-----------------------------------------------------------------------
c

      subroutine outimp(g,n,nf,ieo,ioutd)
      dimension g(512)
c
c	Removed filter order output for Gabriel compatibility
c
c     write(ioutd,5) nf
c  5  format(i15)

      l = n
      if(ieo.eq.0) go to 10
      l = n-1

  10  do 20 i=1,l
        j = n + 1 - i
      write(ioutd,50) g(j)
  20  continue

	if(ieo.eq.0) go to 30
        write(ioutd,50) g(1)

  30  do 40 i=l,1,-1
        j = n + 1 - i
      write(ioutd,50) g(j)
  40  continue
  50  format(e15.8)
      close(unit=ioutd)
  60  return
      end
