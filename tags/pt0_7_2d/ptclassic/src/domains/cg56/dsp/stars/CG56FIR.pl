defstar {
	name { FIR }
	domain { CG56 }
	desc {
Finite Impulse Response (FIR) filter.
Coefficients are in the "taps" state variable.  Default coefficients
give an 8th order, linear phase lowpass filter.  To read coefficients
from a file, replace the default coefficients with "<fileName".
Decimation parameter > 1 reduces sample rate.  Interpolation parameter
> 1 increases sample rate.
	}
	version { @(#)CG56FIR.pl	1.37 06 Oct 1996 }
	author { Chih-Tsung Huang, ported from Gabriel }
	copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { CG56 dsp library }
	htmldoc {
The output of the FIR filter is given by:
<pre>
        y(n) = sum from { i = 0 } to { N - 1 } of { c[i]x[N-i] }
</pre>
<p>
where N is the order of the filter given by the number of coefficients in
<i>taps</i>.
c[i], i = 0..N-1 are the tap coefficients; and x[T-i] is the input i samples
before the current input.
<p>
The parameters <i>decimation</i> and <i>interpolation</i> enable this star
to perform downsampling or upsampling.
<p>
The default filter is a linear-phase equiripple lowpass filter with its 3dB
cutoff frequency at about 1/3 of the Nyquist frequency.
	}
	input {
		name {input}
		type {FIX}
	}
	output {
		name {output}
		type {FIX}
	}
	state {
		name {taps}
		type {FIXARRAY}
		default {
	"-.040609 -.001628 .17853 .37665 .37665 .17853 -.001628 -.040609"
		}
		desc { Filter tap values. }
		attributes { A_NONCONSTANT|A_YMEM|A_NOINIT }
	}
	state {
		name {decimation}
		type {int}
		default {1}
		desc {Decimation ratio.}
	}
        state {
		name {decimationPhase}
		type {int}
		default {0}
		desc {Downsampler phase, currently not supported.}
	}
	state {
		name {interpolation}
		type {int}
		default {1}
		desc {Interpolation ratio.}
	}
	state {
		name {oldsample}
		type {fixarray}
		default {0}
		desc {internal}
		// removed A_CONSEQ flag -- kennard
                attributes {A_CIRC|A_NONCONSTANT|A_NONSETTABLE|A_XMEM|A_NOINIT}
	}
        state {
                name {oldsampleStart}
	        type {int}
	        default {0}
                desc { pointer to oldsample }
                attributes {A_NONCONSTANT|A_NONSETTABLE|A_XMEM|A_NOINIT}
        }	    
        state {
                name {oldsampleSize}
	        type {int}
	        default {0}
                desc { size of oldsample }
                attributes {A_NONCONSTANT|A_NONSETTABLE}
        }	    
        state {
                name {tapsNum}
	        type {int}
	        default {8}
                desc { internal }
                attributes {A_NONCONSTANT|A_NONSETTABLE}
        }	    
        state {
                name {tp}
	        type {int}
	        default {0}
                desc { internal }
                attributes {A_NONCONSTANT|A_NONSETTABLE}
        }	    
        state {
                name {adjust}
	        type {int}
	        default {0}
                desc { internal }
                attributes {A_NONCONSTANT|A_NONSETTABLE}
        }	    
        state {
                name {dec}
	        type {int}
	        default {0}
                desc { internal }
                attributes {A_NONCONSTANT|A_NONSETTABLE}
        }	    

        method {
	    name {CheckParameterValues}
	    arglist { "()" }
	    type { void }
	    code {
		StringList msg = "Filter taps ";
		int numTaps = taps.size();
		int errorFlag = FALSE;
		for (int i = 0; i < numTaps; i++) {
		    double tapvalue = double(taps[i]);
		    if ( tapvalue < -1 || tapvalue > 1 ) {
			errorFlag = TRUE;
			msg << i << " ";
		    }
		}
		if (errorFlag) {
		    msg << " are outside the range of [-1,1]";
		    Error::warn(*this, msg);
		}
	    }
	}

	setup {
	      if ( int(decimationPhase) != 0 ) {
		  Error::warn(*this,
			      "A non-zero decimationPhase is not supported ",
			      "in the CG56 version of this star");
	      }

	      int interp = int(interpolation);
 	      int decimt = int(decimation);
              tapsNum = taps.size();
	      int oldSampleNum = int(ceil(double(tapsNum - decimt)/double(interp)));	      if (oldSampleNum>0)
                   oldsample.resize(oldSampleNum);
              else
                   oldsample.resize(0);
              input.setSDFParams(decimt, decimt-1);
	      output.setSDFParams(interp, interp-1);

              if (decimt>1 && interp>1) 
	              Error::abortRun (*this, ": Cannot both interpolate and decimate.");
	      
              int modtemp = tapsNum%interp;
              if (modtemp != 0) {
	             taps.resize(tapsNum+interp-modtemp);
		     tapsNum=taps.size();
	      }
        }

        initCode {
	    int interp = interpolation;
	    int i = 0;
	    StringList tapInit;
	    tapInit = "\torg\t$ref(taps)\n";
	    if (interp == 1)
		for (i = 0; i < taps.size() ; i++)
		    tapInit << "\tdc\t" << double(taps[i]) << '\n';
	    else {
		for (i = interp-1; i >= 0; i--)
		    for (int j=i; j<taps.size(); j+=interp) {
			tapInit << "\tdc\t"<< double(taps[j]) << '\n';
		    }
	    }
	    tapInit << "\torg\tp:\n";
	    addCode(tapInit);
	    oldsampleSize=oldsample.size();
	    if (oldsampleSize>0) 
		addCode(block);
        }
        
	go {
		CheckParameterValues();
                tp = int(tapsNum/interpolation);
                dec = tp;
                if (tp > decimation) dec=decimation;
		adjust = (int)decimation - (int)tp;
		if (adjust<0) adjust=0;
		if((tp*interpolation) >1)  addCode(first);
                if(tp>decimation) addCode(old);
	        if(tp>(decimation+1)) addCode(second); 

// Normal case
                if(dec==1 && interpolation==1) {
                     addCode(must);
	             if(tp>2) addCode(greaterTwo);
		     if(tp==2) addCode(equalTwo);
		     if(tp<2) addCode(lessTwo);
		}
// decimation
                else if(dec>1) {
                     addCode(decmust);
	             if(tp > (1+dec)) addCode(decgreater);
		     if(tp == (1+dec)) addCode(decequal);
		     if(tp < (1+dec)) addCode(decless);
	        }   
// interpolation
                else {
                     addCode(interpmust);
	             if(tp>2) {
                          addCode(greater);
		          if(interpolation-1 > 1) addCode(greaterloop);
		          if(interpolation-1 ==1) addCode(gone);
			  if(tp-2==2) {
		              addCode(grep);
			      addCode(grep);
			  }
                          else if(tp-2==1) 
			      addCode(grep);
		          else /*if(interpolation-2>0) is this needed*/
		              addCode(repGreater);

                          addCode(gcont);
	             }
		     if(tp==2) {
                          addCode(equal);
		          if(interpolation-2 > 1) addCode(fullloop);
		          if(interpolation-2 ==1) addCode(equalone);
                          addCode(equalcont);
		     }
		     if(tp<2) {
		          addCode(less);
			  if(interpolation-2==2) {
		              addCode(lessrep);
			      addCode(lessrep);
			  }
                          else if(interpolation-2==1) 
			      addCode(lessrep);

		          else if(interpolation-2>0) 
		              addCode(rep);

			  addCode(lesscont);
		    }
               }
	}

	codeblock(block) {
; initialize FIR
; state variables
        org     $ref(oldsample)
        bsc     $val(oldsampleSize),0

; pointer to state variable buffer
        org     p:
        move    #>$addr(oldsample),x0
        move    x0,$ref(oldsampleStart)
        org     p:
        }

        codeblock(first) {
; tap size: $val(tp)
; interpolation : $val(interpolation)
; decimation: $val(dec)
        move    #>($val(tp)*$val(interpolation)+$addr(taps)-1),r0
        }
        codeblock(old) {
        move    $ref(oldsampleStart),r5
        }
        codeblock(second) {
        move    #$val(tp)-$val(decimation)-1,m5
        }
        codeblock(must) {
        move    $ref2(input,adjust),x0
        }    
        codeblock(greaterTwo) {
        clr     a         x:(r5)+,x1      y:(r0)-,y1
        rep     #$val(tp)-2
        mac     x1,y1,a   x:(r5)+,x1      y:(r0)-,y1
        mac     x1,y1,a   x0,x:(r5)+      y:(r0)-,y1
        macr    x0,y1,a   r5,$ref(oldsampleStart)
        move            m7,m5
        move    a,$ref(output)
        }    
        codeblock(equalTwo) {
        clr     a         x:(r5),x1       y:(r0)-,y1
        mac     x1,y1,a   x0,x:(r5)       y:(r0)-,y1
        macr    x0,y1,a 
        move    a,$ref(output)
        }
        codeblock(lessTwo) {    
        clr     a         $ref(taps),y1
        mpyr    x0,y1,a
        move    a,$ref(output)
        }
        codeblock(decmust) {
        move    #>($val(adjust)+$addr(input)),r6
        }
        codeblock(decgreater) {
        clr     a         x:(r5)+,x1      y:(r0)-,y1
        rep     #$val(tp)-$val(dec)-1
        mac     x1,y1,a   x:(r5)+,x1      y:(r0)-,y1
        do      #$val(dec),$label(loop)
        mac     x1,y1,a   x:(r6)+,x1      y:(r0)-,y1
        move    x1,x:(r5)+
$label(loop)
        macr    x1,y1,a   r5,$ref(oldsampleStart)
        move    m7,m5
        move    a,$ref(output)
        }
        codeblock(decequal) {
        clr     a               x:(r5),x1       y:(r0)-,y1
        rep     #$val(dec)
        mac     x1,y1,a         x:(r6)+,x1      y:(r0)-,y1
        macr    x1,y1,a         x1,x:(r5)
        move    a,$ref(output)
        }
        codeblock(decless) {        
        nop
        clr     a         x:(r6)+,x1      y:(r0)-,y1
        rep     #$val(tp)-1
        mac     x1,y1,a   x:(r6)+,x1      y:(r0)-,y1
        macr    x1,y1,a
        move    a,$ref(output)
        }
        codeblock(interpmust) {
        move    $ref2(input,adjust),x0
        move    #>$addr(output),r6
        }
        codeblock(greater) {
        clr   a         x:(r5)+,x1      y:(r0)-,y1
        }
        codeblock(greaterloop) {
        do    #$val(interpolation)-1,$label(looparound)
        rep   #$val(tp)-1
        mac   x1,y1,a   x:(r5)+,x1      y:(r0)-,y1
        macr  x0,y1,a                   y:(r0)-,y1
        clr   a         a,x:(r6)+
$label(looparound)   
        }
        codeblock(gone) {
        rep   #$val(tp)-1
        mac   x1,y1,a   x:(r5)+,x1      y:(r0)-,y1
        macr  x0,y1,a                   y:(r0)-,y1
        clr   a         a,x:(r6)+
        }
        codeblock(grep) {
        mac   x1,y1,a   x:(r5)+,x1      y:(r0)-,y1
        }
        codeblock(repGreater) {
        rep   #$val(tp)-2
        mac   x1,y1,a   x:(r5)+,x1      y:(r0)-,y1    
        }
        codeblock(gcont) {
        mac   x1,y1,a   x0,x:(r5)+      y:(r0)-,y1
        macr  x0,y1,a   r5,$ref(oldsampleStart)
        move            m7,m5
        move            a,x:(r6)+
        }
           
        codeblock(equal) {        
        move            x:(r5),x1       y:(r0)-,y1
        mpy   x1,y1,a                   y:(r0)-,y1
        macr  x0,y1,a                   y:(r0)-,y1
        }

        codeblock(fullloop) {       
        do    #$val(interpolation)-2,$label(fullLoop)
        mpy   x1,y1,a   a,x:(r6)+       y:(r0)-,y1
        macr  x0,y1,a                   y:(r0)-,y1
$label(fullLoop)
        }
        codeblock(equalone) {
        mpy   x1,y1,a   a,x:(r6)+       y:(r0)-,y1
        macr  x0,y1,a                   y:(r0)-,y1
        }
        codeblock(equalcont) {
        mpy   x1,y1,a   a,x:(r6)+       y:(r0)-,y1
        macr  x0,y1,a   x0,x:(r5)
        move            a,x:(r6)+
        }

        codeblock(less) {
        move                            y:(r0)-,y1
        mpyr  x0,y1,a                   y:(r0)-,y1
        }
        codeblock(lessrep) {
        mpyr  x0,y1,a   a,x:(r6)+       y:(r0)-,y1
        }
        codeblock(rep) {
        rep   #$val(interpolation)-2
        mpyr  x0,y1,a   a,x:(r6)+       y:(r0)-,y1
        }
        codeblock(lesscont) {
        mpyr  x0,y1,a   a,x:(r6)+
        move            a,x:(r6)+
        }

	execTime {
                int a=2;
                int b=0;

                if (tapsNum>1)  a=4;
		    
                if (tapsNum<=decimation) {
              	    if (tapsNum=1) 
		            b=3;
	            else
		            b=5+tapsNum;
                }
	        else if (tapsNum==decimation+1) {
	            if (decimation==1)
		            b=4;
	            else
		            b=5+decimation;
	        }
                else {
	            if (decimation==1)
		            b=8+tapsNum-decimation;
	            else
		            b=10+tapsNum+decimation;
               }
               return a+b;
      }
}
