defstar {
	name { FIR }
	domain { CG56 }
	desc { A Finite Impulse Response (FIR) filter. }
	version { $Id$ }
	author { Chih-Tsung Huang, J. Pino, ported from Gabriel }
	copyright { 1992 The Regents of the University of California }
	location { CG56 demo library }
	explanation {
Coefficients are in the "taps" state variable.  Default coefficients
give an 8th order, linear phase lowpass filter. To read coefficients
from a file, replace the default coefficients with "<fileName".
Decimation parameters > 1 reduces sample rate.
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
		name {coef}
		type {FIXARRAY}
		default {
	"-.040609 -.001628 .17853 .37665 .37665 .17853 -.001628 -.040609"
		}
		desc { Filter tap values. }
		attributes { A_NONCONSTANT|A_YMEM }
		
	}
	state {
		name {decimation}
		type {int}
		default {1}
		desc {Decimation ratio.}
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
                attributes {
               A_CIRC|A_NONCONSTANT|A_NONSETTABLE|A_XMEM|A_CONSEC|A_NOINIT
	        }
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
                name {coefNum}
	        type {int}
	        default {8}
                desc { internal }
                attributes {A_NONCONSTANT|A_NONSETTABLE}
        }	    
        state {
                name {taps}
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

	start {
              coefNum=coef.size();
              if (int(-(int(decimation)-int(coefNum))/int(interpolation))>0)
                   oldsample.resize(int(-(int(decimation)-int(coefNum))/int(interpolation)));
              else
                   oldsample.resize(0);


              int modtemp=coefNum%interpolation;
              input.setSDFParams(decimation, decimation-1);
	      output.setSDFParams(interpolation, interpolation-1);

              if (decimation>1 && interpolation>1) 
	              Error::abortRun (*this, ": Cannot both interpolate and decimate.");
                
              if (modtemp !=0) {
	             coef.resize(coefNum+interpolation-modtemp);
		     coefNum=coef.size();
	      }
              StringList permuted ="";
              for (int i=interpolation-1; i> -1; i--) {
                    int j=i;
                    while(j<coefNum) {
		            permuted +=coef[j];
			    permuted +=" ";
                            j +=interpolation;
		    }
              }
	      coef.setCurrentValue(permuted);
               
        }

        initCode {
                oldsampleSize=oldsample.size();

		if (oldsampleSize>0) 
                     gencode(block);
        }
        
	go {
                taps= int(coefNum/interpolation);
                dec = taps;
                if (taps > decimation) dec=decimation;
		adjust = decimation - taps;
		if (adjust<0) adjust=0;
		if((taps*interpolation) >1)  gencode(first);
                if(taps>decimation) gencode(old);
	        if(taps>(decimation+1)) gencode(second); 

// Normal case
                if(dec==1 && interpolation==1) {
                     gencode(must);
	             if(taps>2) gencode(greaterTwo);
		     if(taps==2) gencode(equalTwo);
		     if(taps<2) gencode(lessTwo);
		}
// decimation
                else if(dec>1) {
                     gencode(decmust);
	             if(taps > (1+dec)) gencode(decgreater);
		     if(taps == (1+dec)) gencode(decequal);
		     if(taps < (1+dec)) gencode(decless);
	        }   
// interpolation
                else {
                     gencode(interpmust);
	             if(taps>2) {
                          gencode(greater);
		          if(interpolation-1 > 1) gencode(greaterloop);
		          if(interpolation-1 ==1) gencode(gone);
			  if(taps-2==2) {
		              gencode(grep);
			      gencode(grep);
			  }
                          else if(taps-2==1) 
			      gencode(grep);
		          else if(interpolation-2>0) 
		              gencode(repGreater);

                          gencode(gcont);
	             }
		     if(taps==2) {
                          gencode(equal);
		          if(interpolation-2 > 1) gencode(fullloop);
		          if(interpolation-2 ==1) gencode(equalone);
                          gencode(equalcont);
		     }
		     if(taps<2) {
		          gencode(less);
			  if(interpolation-2==2) {
		              gencode(lessrep);
			      gencode(lessrep);
			  }
                          else if(interpolation-2==1) 
			      gencode(lessrep);

		          else if(interpolation-2>0) 
		              gencode(rep);

			  gencode(lesscont);
		    }
               }
	}

	codeblock(block) {
; initialize FIR
; state variables
        org     $ref(oldsample)
        bsc     $val(oldsampleSize),0
        org     p:

; pointer to state variable buffer

        org     $ref(oldsampleStart)
        dc      $addr(oldsample)
        org     p:
        }

        codeblock(first) {
; taps: $val(taps)
; interpolation : $val(interpolation)
; dec: $val(dec)
        move    #<$val(taps)*$val(interpolation)+$addr(coef)-1,r0
        }
        codeblock(old) {
        move    $ref(oldsampleStart),r5
        }
        codeblock(second) {
        move    #<$val(taps)-$val(decimation)-1,m5
        }
        codeblock(must) {
        move    $ref2(input,adjust),x0
        }    
        codeblock(greaterTwo) {
        clr     a         x:(r5)+,x1      y:(r0)-,y1
        rep     #$val(taps)-2
        mac     x1,y1,a   x:(r5)+,x1      y:(r0)-,y1
        mac     x1,y1,a   x0,x:(r5)+      y:(r0)-,y1
        macr    x0,y1,a   r5,$ref(oldsampleStart)
        move            m7,m5
        move    a,$ref(output)
        }    
        codeblock(equalTwo) {
        clr     a         x:(r5)+,x1      y:(r0)-,y1
        mac     x1,y1,a   x0,x:(r5)       y:(r0)-,y1
        macr    x0,y1,a 
        move    a,$ref(output)
        }
        codeblock(lessTwo) {    
        clr     a         $ref(coef),y1
        mpyr    x0,y1,a
        move    a,$ref(output)
        }
        codeblock(decmust) {
        move    #<$val(adjust)+$addr(input),r6
        }
        codeblock(decgreater) {
        clr     a         x:(r5)+,x1      y:(r0)-,y1
        rep     #$val(taps)-$val(dec)-1
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
        rep     #$val(taps)-1
        mac     x1,y1,a   x:(r6)+,x1      y:(r0)-,y1
        macr    x1,y1,a
        move    a,$ref(output)
        }
        codeblock(interpmust) {
        move    $ref2(input,adjust),x0
        move    #<$addr(output),r6
        }
        codeblock(greater) {
        clr   a         x:(r5)+,x1      y:(r0)-,y1
        }
        codeblock(greaterloop) {
        do    #$val(interpolation)-1,$label(looparound)
        rep   #$val(taps)-1
        mac   x1,y1,a   x:(r5)+,x1      y:(r0)-,y1
        macr  x0,y1,a                   y:(r0)-,y1
        clr   a         a,x:(r6)+
$label(looparound)   
        }
        codeblock(gone) {
        rep   #$val(taps)-1
        mac   x1,y1,a   x:(r5)+,x1      y:(r0)-,y1
        macr  x0,y1,a                   y:(r0)-,y1
        clr   a         a,x:(r6)+
        }
        codeblock(grep) {
        mac   x1,y1,a   x:(r5)+,x1      y:(r0)-,y1
        }
        codeblock(repGreater) {
        rep   #$val(taps)-2
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

                if (coefNum>1)  a=4;
		    
                if (coefNum<=decimation) {
              	    if (coefNum=1) 
		            b=3;
	            else
		            b=5+taps;
                }
	        else if (taps==dec+1) {
	            if (dec==1)
		            b=4;
	            else
		            b=5+dec;
	        }
                else {
	            if (dec==1)
		            b=8+taps-dec;
	            else
		            b=10+2*int(dec);	            
               }
       }
}
