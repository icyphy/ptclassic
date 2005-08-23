      Program Optfir
c @(#)optfir.f	1.1	11/2/90
c
c   This program designs optimal equirriple FIR digital filters
c
      double precision attn,ripple,svrip
      double precision pi2,pi,ad,dev,x,y
      character fname*100, answer*1
      common des,wt,alpha,iext,nfcns,ngrid,pi2,ad,dev,x,y,grid
      dimension iext(100),ad(100),alpha(100),x(100),y(100),h(100)
      dimension des(1600),grid(1600),wt(1600)
      dimension edge(20),fx(10),wtx(10),deviat(10)
      pi=3.141592653589793
      pi2=6.283185307179586
c Maximum filter length
      nfmax=256
      write (*,'(,a)') ' PROVISIONAL EQUIRRIPLE FILTER DESIGN '
      write (*,'(,a)') ' USE AT YOUR OWN RISK --------------- '
  100 continue
      write (*,'(/,a)') ' Enter name of input command file (press <Enter
     +> for manual entry, '
      write (*,'(,a\)') ' Sorry, no tilde-expansion.  Give path relative
     + to your home or startup directory): '
      read (*,'(a12)') fname
      if (fname .eq. ' ') then
         io = 5
      else
         io = 3
         open (3, file=fname)
      endif
      write (*,'(,a,/,20x,a)') ' Enter filter type (1=Bandpass, 2=Differ
     +entiator,', '3=Hilbert transformer, 4=Half-band): '
      read (io,*) jtype
      write (*,'(,a)') ' Enter filter length (enter 0 for estimate): '
      read (io,*) nfilt
      if(nfilt.gt.nfmax.or.(nfilt.lt.3.and.nfilt.ne.0)) go to 515
      write (*,'(,a)') ' Enter sampling rate of filter: '
      read (io,*) fs
      if (jtype .eq. 4) then
         nfilt = (nfilt + 1) / 2
         nbands = 1
         edge(1) = 0.0
         write (*,'(/,a\)') ' Enter passband edge frequency: '
         read (io,*) edge(2)
         edge(2) = 2.0 * edge(2) / fs
         fx(1) = 0.5
         wtx(1) = 1.0
         go to 118
      endif
      write (*,'(,a)') ' Enter number of filter bands: '
      read (io,*) nbands
      if(nbands.le.0) nbands=1
      do 115 i=1,nbands
      write (*,'(,a,i3,a)') ' Enter lower band edge for band', i, ': '
      j=1+2*(i-1)
      read (io,*) edge(j)
      edge(j)=edge(j)/fs
      write (*,'(,a,i3,a)') ' Enter upper band edge for band', i, ': '
      read (io,*) edge(j+1)
      edge(j+1)=edge(j+1)/fs
      write (*,'(,a,i3,a)') ' Enter desired value for band', i, ': '
      read (io,*) fx(i)
      write (*,'(,a,i3,a)') ' Enter weight factor for band', i, ': '
      read (io,*) wtx(i)
  115 continue
      write (*,'(,a)') ' Do you want x/sin(x) predistortion? (y/n): '
      read (io,'(a1)') answer
      if (answer .eq. 'y' .or. answer .eq. 'Y') idist=1
  118 write (*,'(,a)') ' Enter name of coefficient output file '
      write (*,'(,a)') ' (Sorry, no tilde-expansion.  Give path relative
     +ve to your home directory): '
      read (io,'(a)') fname
      open (1, file=fname)
      if(nfilt.eq.0) go to 120
      nrox=0
      go to 130
  120 continue
      write (*,'(,a)') ' Enter desired passband ripple in dB: '
      read (io,*) ripple
      write (*,'(,a)') ' Enter desired stopband attenuation in dB: '
      read (io,*) attn
      ripple=10.**(ripple/20.)
      attn=10.**(-attn/20.)
      ripple=(ripple-1)/(ripple+1)
      ripple=dlog10(ripple)
      attn=dlog10(attn)
      svrip=ripple
      ripple=(0.005309*(ripple**2)+0.07114*ripple-0.4761)*attn
     1-(0.00266*(ripple**2)+0.5941*ripple+0.4278)
      attn=11.01217+0.51244*(svrip-attn)
      deltaf=edge(3)-edge(2)
      nfilt=ripple/deltaf-attn*deltaf+1
      write (*,'(/,a,i4)') ' Estimated filter length = ', nfilt
      if(nfilt.gt.nfmax.or.(nfilt.lt.3.and.nfilt.ne.0)) go to 515
      nrox=1
  130 neg=1
      write (*,'(/,a,/)') ' Executing ...'
      if(jtype .eq. 1 .or. jtype .eq. 4) neg = 0
      nodd=nfilt/2
      nodd=nfilt-2*nodd
      nfcns=nfilt/2
      if(nodd.eq.1.and.neg.eq.0) nfcns=nfcns+1
      grid(1)=edge(1)
      delf=16*nfcns
      delf=0.5/delf
      if(neg.eq.0) go to 135
      if(edge(1).lt.delf)grid(1)=delf
  135 continue
      j=1
      l=1
      lband=1
  140 fup=edge(l+1)
  145 temp=grid(j)
      des(j)=eff(temp,fx,lband,jtype,idist)
      wt(j)=wate(temp,fx,wtx,lband,jtype)
      j=j+1
      grid(j)=temp+delf
      if(grid(j).gt.fup) go to 150
      go to 145
  150 grid(j-1)=fup
      des(j-1)=eff(fup,fx,lband,jtype,idist)
      wt(j-1)=wate(fup,fx,wtx,lband,jtype)
      lband=lband+1
      l=l+2
      if(lband.gt.nbands) go to 160
      grid(j)=edge(l)
      go to 140
  160 ngrid=j-1
      if(neg.ne.nodd) go to 165
      if(grid(ngrid).gt.(0.5-delf)) ngrid=ngrid-1
  165 continue
      if(neg)170,170,180
  170 if(nodd.eq.1) go to 200
      do 175 j=1,ngrid
      change=dcos(pi*grid(j))
      des(j)=des(j)/change
  175 wt(j)=wt(j)*change
      go to 200
  180 if(nodd.eq.1) go to 190
      do 185 j=1,ngrid
      change=dsin(pi*grid(j))
      des(j)=des(j)/change
  185 wt(j)=wt(j)*change
      go to 200
  190 do 195 j=1,ngrid
      change=dsin(pi2*grid(j))
      des(j)=des(j)/change
  195 wt(j)=wt(j)*change
  200 temp=float(ngrid-1)/float(nfcns)
      do 210 j=1,nfcns
  210 iext(j)=(j-1)*temp+1
      iext(nfcns+1)=ngrid
      nm1=nfcns-1
      nz=nfcns+1
      call Remez(edge,nbands)
      if(neg) 300,300,320
  300 if(nodd.eq.0) go to 310
      do 305 j=1,nm1
  305 h(j)=0.5*alpha(nz-j)
      h(nfcns)=alpha(1)
      go to 350
  310 h(1)=0.25*alpha(nfcns)
      do 315 j=2,nm1
  315 h(j)=0.25*(alpha(nz-j)+alpha(nfcns+2-j))
      h(nfcns)=0.5*alpha(1)+0.25*alpha(2)
      go to 350
  320 if(nodd.eq.0) go to 330
      h(1)=0.25*alpha(nfcns)
      h(2)=0.25*alpha(nm1)
      do 325 j=3,nm1
  325 h(j)=0.25*(alpha(nz-j)-alpha(nfcns+3-j))
      h(nfcns)=0.5*alpha(1)-0.25*alpha(3)
      h(nz)=0.0
      go to 350
  330 h(1)=0.25*alpha(nfcns)
      do 335 j=2,nm1
  335 h(j)=0.25*(alpha(nz-j)-alpha(nfcns+2-j))
      h(nfcns)=0.5*alpha(1)-0.25*alpha(2)
  350 write(6,360)
  360 format(/' Finite Impulse Response (FIR)'/
     1,' Linear Phase Digital Filter Design'/
     2,' Remez Exchange Algorithm'/)
      if(jtype.eq.1) write(6,365)
  365 format(' Bandpass Filter'/)
      if(jtype.eq.2) write(6,370)
  370 format(' Differentiator'/)
      if(jtype.eq.3) write(6,375)
  375 format(' Hilbert Transformer'/)
      if(jtype.eq.4) write(6,376)
  376 format(' Half-band Filter'/)
      write(6,378) nfilt
  378 format(' Filter length = ',i3/)
      if(nrox.eq.1) write(6,379)
  379 format(' Filter length determined by approximation'/)
      write(6,380)
  380 format(' Impulse Response Coefficients:'/)
      do 381 j=1,nfcns
      k=nfilt+1-j
      if(neg.eq.0) write(6,382)j,h(j),k
      if(neg.eq.1) write(6,383)j,h(j),k
  381 continue
  382 format(10x,'h(',i3,') = ', e14.7,' = h(',i4,')')
  383 format(10x,'h(',i3,') = ', e14.7,' = -h(',i4,')')
      if(neg.eq.1.and.nodd.eq.1) write(6,384) nz, 0.0
  384 format(10x,'h(',i3,') = ', e14.7)
      write (*,*)
c
c	DEBUG
c
c     Pause 'Press <Enter> to continue ...'
      do 450 k=1,nbands,4
      kup=k+3
      if(kup.gt.nbands) kup=nbands
      write(6,385)(j,j=k,kup)
  385 format(25x,4('Band',i3,8x))
      write(6,390)(edge(2*j-1),j=k,kup)
  390 format(/' Lower band edge:',5f15.7)
      write(6,395)(edge(2*j),j=k,kup)
  395 format(' Upper band edge:',5f15.7)
      if(jtype.ne.2) write(6,400) (fx(j),j=k,kup)
  400 format(' Desired value:',2x,5f15.7)
      if(jtype.eq.2) write(6,405) (fx(j),j=k,kup)
  405 format(' Desired slope:',2x,5f15.7)
      write(6,410) (wtx(j),j=k,kup)
  410 format(' Weight factor:',2x,5f15.7)
      do 420 j=k,kup
  420 deviat(j)=dev/wtx(j)
      write(6,425) (deviat(j),j=k,kup)
  425 format(' Deviation:',6x,5f15.7)
      do 430 j=k,kup
      if(fx(j).eq.1.0) deviat(j)=(1.0+deviat(j))/(1.0-deviat(j))
  430 deviat(j)=20.0*alog10(deviat(j))
      write(6,435) (deviat(j),j=k,kup)
  435 format(' Deviation in dB:',5f15.7)
  450 continue
      write(6,455) (grid(iext(j)),j=1,nz)
  455 format(/' Extremal frequencies:'//(2x,5f12.7))
      if (nodd .eq. 1) then
         if (neg .eq. 1) nfcns = nfcns + 1
         ncoeff = 2 * nfcns - 1
      else
        ncoeff = 2 * nfcns
      endif
      if (jtype .eq. 4) then
c
c   Remove the writing of the filter length, for
c   Compatibility with Gabriel
c        write(1,500) 2 * ncoeff - 1
c
  500    format(i4)
         do 502 i = 1, ncoeff - 1
            if (i .lt. nfcns) then
               write (1,510) h(i)
               write (1,510) 0.0
            elseif (i .eq. nfcns) then
               write (1,510) h(i)
               write (1,510) 0.5
               write (1,510) h(i)
            else
               write (1,510) 0.0
               write (1,510) h(ncoeff-i)
            endif
  502    continue
      else
c
c   Remove the writing of the filter length, for
c   Compatibility with Gabriel
c        write(1,500) ncoeff
c
         do 505 i = 1, ncoeff
            if(i .le. nfcns) write(1,510) h(i)
            if(i .gt. nfcns) then
               if(neg .eq. 0) then
                  write(1,510) h(ncoeff-i+1)
               else
                  write(1,510) -h(ncoeff-i+1)
               endif
            endif
  505    continue
      endif
  510 format(1pe14.6)
      write (*,*)
      go to 520
c
c   Catch Errors
c
  515 print *, '*** FILTER TOO LONG OR SHORT FOR PROGRAM ***'
c
c   Find out whether to terminate program
c
  520 call flush(1)
      close(1)
      write (*,'(/,a\)') ' Another design? (y/n): '
      read (*,'(a1)') answer
      if (answer .eq. 'y' .or. answer .eq. 'Y') go to 100
      stop 'End of program'
      end
c
      Function Eff(freq,fx,lband,jtype,idist)
      dimension fx(10)
      if(jtype.eq.2) go to 1
      eff=fx(lband)
      if(idist.eq.0) return
      x=3.141593*freq
      if(x.ne.0) eff=eff*x/sin(x)
      return
    1 eff=fx(lband)*freq
      return
      end
c
      Function Wate(freq,fx,wtx,lband,jtype)
      dimension fx(10),wtx(10)
      if(jtype.eq.2) go to 1
      wate=wtx(lband)
      return
    1 if(fx(lband).lt.0.0001) go to 2
      wate=wtx(lband)/freq
      return
    2 wate=wtx(lband)
      return
      end
c
      Subroutine Remez(edge,nbands)
      double precision pi2,dnum,dden,dtemp,a,p,q
      double precision d,ad,dev,x,y
      common des,wt,alpha,iext,nfcns,ngrid,pi2,ad,dev,x,y,grid
      dimension edge(20)
      dimension iext(100),ad(100),alpha(100),x(100),y(100)
      dimension des(1600),grid(1600),wt(1600)
      dimension a(100),p(100),q(100)
      itrmax=25
      devl=-1.0
      nz=nfcns+1
      nzz=nfcns+2
      niter=0
  100 continue
      iext(nzz)=ngrid+1
      niter=niter+1
      if(niter.gt.itrmax) go to 400
      do 110 j=1,nz
      dtemp=grid(iext(j))
      dtemp=dcos(dtemp*pi2)
  110 x(j)=dtemp
      jet=(nfcns-1)/15+1
      do 120 j=1,nz
      kid=j
  120 ad(j)=d(kid,nz,jet)
      dnum=0.0
      dden=0.0
      k=1
      do 130 j=1,nz
      l=iext(j)
      dtemp=ad(j)*des(l)
      dnum=dnum+dtemp
      dtemp=k*ad(j)/wt(l)
      dden=dden+dtemp
  130 k=-k
      dev=dnum/dden
c
c	DEBUG
c
c     write(6,131) dev
c 131 format(1x,12hdeviation = ,f12.9)
c
      nu=1
      if(dev.gt.0.0) nu=-1
      dev=-nu*dev
      k=nu
      do 140 j=1,nz
      l=iext(j)
      dtemp=k*dev/wt(l)
      y(j)=des(l)+dtemp
  140 k=-k
      if(dev.ge.devl) go to 150
      call Ouch
      go to 400
  150 devl=dev
      jchnge=0
      k1=iext(1)
      knz=iext(nz)
      klow=0
      nut=-nu
      j=1
  200 if(j.eq.nzz) ynz=comp
      if(j.ge.nzz) go to 300
      kup=iext(j+1)
      l=iext(j)+1
      nut=-nut
      if(j.eq.2) y1=comp
      comp=dev
      if(l.ge.kup) go to 220
      err=gee(l,nz)
      err=(err-des(l))*wt(l)
      dtemp=nut*err-comp
      if(dtemp.le.0.0) go to 220
      comp=nut*err
  210 l=l+1
      if(l.ge.kup) go to 215
      err=gee(l,nz)
      err=(err-des(l))*wt(l)
      dtemp=nut*err-comp
      if(dtemp.le.0.0) go to 215
      comp=nut*err
      go to 210
  215 iext(j)=l-1
      j=j+1
      klow=l-1
      jchnge=jchnge+1
      go to 200
  220 l=l-1
  225 l=l-1
      if(l.le.klow) go to 250
      err=gee(l,nz)
      err=(err-des(l))*wt(l)
      dtemp=nut*err-comp
      if(dtemp.gt.0.0) go to 230
      if(jchnge.le.0) go to 225
      go to 260
  230 comp=nut*err
  235 l=l-1
      if(l.le.klow) go to 240
      err=gee(l,nz)
      err=(err-des(l))*wt(l)
      dtemp=nut*err-comp
      if(dtemp.le.0.0) go to 240
      comp=nut*err
      go to 235
  240 klow=iext(j)
      iext(j)=l+1
      j=j+1
      jchnge=jchnge+1
      go to 200
  250 l=iext(j)+1
      if(jchnge.ge.0) go to 215
  255 l=l+1
      if(l.ge.kup) go to 260
      err=gee(l,nz)
      err=(err-des(l))*wt(l)
      dtemp=nut*err-comp
      if(dtemp.le.0.0) go to 255
      comp=nut*err
      go to 210
  260 klow=iext(j)
      j=j+1
      go to 200
  300 if(j.gt.nzz) go to 320
      if(k1.gt.iext(1)) k1=iext(1)
      if(knz.lt.iext(nz)) knz=iext(nz)
      nut1=nut
      nut=-nu
      l=0
      kup=k1
      comp=ynz*(1.00001)
      luck=1
  310 l=l+1
      if(l.ge.kup) go to 315
      err=gee(l,nz)
      err=(err-des(l))*wt(l)
      dtemp=nut*err-comp
      if(dtemp.le.0.0) go to 310
      comp=nut*err
      j=nzz
      go to 210
  315 luck=6
      go to 325
  320 if(luck.gt.9) go to 350
      if(comp.gt.y1) y1=comp
      k1=iext(nzz)
  325 l=ngrid+1
      klow=knz
      nut=-nut1
      comp=y1*(1.00001)
  330 l=l-1
      if(l.le.klow) go to 340
      err=gee(l,nz)
      err=(err-des(l))*wt(l)
      dtemp=nut*err-comp
      if(dtemp.le.0.0) go to 330
      j=nzz
      comp=nut*err
      luck=luck+10
      go to 235
  340 if(luck.eq.6) go to 370
      do 345 j=1,nfcns
  345 iext(nzz-j)=iext(nz-j)
      iext(1)=k1
      go to 100
  350 kn=iext(nzz)
      do 360 j=1,nfcns
  360 iext(j)=iext(j+1)
      iext(nz)=kn
      go to 100
  370 if(jchnge.gt.0) go to 100
  400 continue
      nm1=nfcns-1
      fsh=1.0e-06
      gtemp=grid(1)
      x(nzz)=-2.0
      cn=2.*nfcns-1.
      delf=1.0/cn
      l=1
      kkk=0
      if(edge(1).eq.0.0.and.edge(2*nbands).eq.0.5) kkk=1
      if(nfcns.le.3) kkk=1
      if(kkk.eq.1) go to 405
      dtemp=dcos(pi2*grid(1))
      dnum=dcos(pi2*grid(ngrid))
      aa=2.0/(dtemp-dnum)
      bb=-(dtemp+dnum)/(dtemp-dnum)
  405 continue
      do 430 j=1,nfcns
      ft=(j-1)*delf
      xt=dcos(pi2*ft)
      if(kkk.eq.1) go to 410
      xt=(xt-bb)/aa
      ft=acos(xt)/pi2
  410 xe=x(l)
      if(xt.gt.xe) go to 420
      if((xe-xt).lt.fsh) go to 415
      l=l+1
      go to 410
  415 a(j)=y(l)
      go to 425
  420 if((xt-xe).lt.fsh) go to 415
      grid(1)=ft
      a(j)=gee(1,nz)
  425 continue
      if(l.gt.1) l=l-1
  430 continue
      grid(1)=gtemp
      dden=pi2/cn
      do 510 j=1,nfcns
      dtemp=0.0
      dnum=(j-1)*dden
      if(nm1.lt.1) go to 505
      do 500 k=1,nm1
  500 dtemp=dtemp+a(k+1)*dcos(dnum*k)
  505 dtemp=2.0*dtemp+a(1)
  510 alpha(j)=dtemp
      do 550 j=2,nfcns
  550 alpha(j)=2.*alpha(j)/cn
      alpha(1)=alpha(1)/cn
      if(kkk.eq.1) go to 545
      p(1)=2.0*alpha(nfcns)*bb+alpha(nm1)
      p(2)=2.0*aa*alpha(nfcns)
      q(1)=alpha(nfcns-2)-alpha(nfcns)
      do 540 j=2,nm1
      if(j.lt.nm1) go to 515
      aa=0.5*aa
      bb=0.5*bb
  515 continue
      p(j+1)=0.0
      do 520 k=1,j
      a(k)=p(k)
  520 p(k)=2.0*bb*a(k)
      p(2)=p(2)+a(1)*2.0*aa
      jm1=j-1
      do 525 k=1,jm1
  525 p(k)=p(k)+q(k)+aa*a(k+1)
      jp1=j+1
      do 530 k=3,jp1
  530 p(k)=p(k)+aa*a(k-1)
      if(j.eq.nm1) go to 540
      do 535 k=1,j
  535 q(k)=-a(k)
      q(1)=q(1)+alpha(nfcns-1-j)
  540 continue
      do 543 j=1,nfcns
  543 alpha(j)=p(j)
  545 continue
      if(nfcns.gt.3) return
      alpha(nfcns+1)=0.0
      alpha(nfcns+2)=0.0
      return
      end
c
      Double Precision Function D(k,n,m)
      double precision ad,dev,x,y,q,pi2
      common des,wt,alpha,iext,nfcns,ngrid,pi2,ad,dev,x,y,grid
      dimension iext(100),ad(100),alpha(100),x(100),y(100)
      dimension des(1600),grid(1600),wt(1600)
      d=1.0
      q=x(k)
      do 3 l=1,m
      do 2 j=l,n,m
      if(j-k)1,2,1
    1 d=2.0*d*(q-x(j))
    2 continue
    3 continue
      d=1.0/d
      return
      end
c
      Function Gee(k,n)
      double precision p,c,d,xf,pi2,ad,dev,x,y
      common des,wt,alpha,iext,nfcns,ngrid,pi2,ad,dev,x,y,grid
      dimension iext(100),ad(100),alpha(100),x(100),y(100)
      dimension des(1600),grid(1600),wt(1600)
      p=0.0
      xf=grid(k)
      xf=dcos(pi2*xf)
      d=0.0
      do 1 j=1,n
      c=xf-x(j)
      c=ad(j)/c
      d=d+c
    1 p=p+c*y(j)
      gee=p/d
      return
      end
c
      Subroutine Ouch
c     write (*,'(/a)') ' ********** FAILURE TO CONVERGE **********'
c     write (*,*) 'Probable cause is machine rounding error'
c     write (*,*) 'The impulse response may still be correct'
      return
      end
