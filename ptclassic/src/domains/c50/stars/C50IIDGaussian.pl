defstar {
	name { IIDGaussian }
	domain { C50 }
	desc {Gaussian Noise Source}
	version { $Id$ }
	author { A. Baensch }
	copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { C50 signal sources library }
        explanation {
.PP
.Id "Gaussian noise"
.Id "noise, Gaussian"
This star generates a sequence of random output samples with a Gaussian
distribution (mean = 0, standard deviation = 0.1).
.PP
.Ir "central limit theorem"
According to the central limit theorem, the sum of N random variables
approaches a Gaussian distribution as N approaches infinity.
This star generates an output number by summing \fInoUniforms\fR uniform
random variables.
.PP
The parameters \fIseed\fR and \fImultiplier\fR control the generation of the
random number. 
	}

        output {
		name { output }
		type { fix }
	}
        state  {
                name { noUniforms }
		type { int }
		desc { uniform random variables. }
		default { 16 }
      	}
	 state  {
                name { noUniformsSpace }
		type { int }
		desc { memory space for uniform variables. }
		default { 0 }
		attributes { A_NONCONSTANT|A_NONSETTABLE }
	}

        state {
                name { seed }
	        type { int }
	        default { 0 }
	        desc { internal }
	        attributes { A_UMEM|A_NONCONSTANT|A_NONSETTABLE|A_NOINIT }
        }
        state {
                name { ravs }
	        type { fixarray }
	        default { "0" }
	        desc { internal }
	        attributes { A_UMEM|A_NONCONSTANT|A_NONSETTABLE|A_NOINIT }
        }

        setup {
                ravs.resize(noUniforms);
        }		
        initCode {
		noUniformsSpace=int(noUniforms)*16;
                addCode(block);
        }		
        go {
                addCode(std);
        }		

        codeblock(block) {
        .ds     $addr(seed)			;initial seed
        .word	07e6dh
        .ds     $addr(ravs)			;memory space for random var.
        .space  $val(noUniformsSpace)		;space in bits
        .text
        }
                       
        codeblock(std) {
	mar	*,AR1
	lar	AR0,#$addr(ravs)		;Address ravs		=> AR0
        lar     AR1,#$addr(seed)		;Address seed		=> AR1
	lar	AR7,#$addr(output)		;Address output		=> AR7
        splk	#$val(noUniforms)-1,BRCR	;number of loops => BRCR
	rptb	$label(lpUni)			;loop to label(lpUni)
         lacc	*,1				;
	 xor	*				;
	 sacl	INDX,2				;generate random
	 xor	INDX				;
	 and	#8000h				;variables
	 add 	*,16				;
	 sach	*,1,AR1				;
	 sach	*,1				;new seed
$label(lpUni)	 				;
        mar	*,AR0				;Gaussian mean=0 sigma=0.1
	lar	AR0,#$addr(ravs)		;Adress from first value => AR0
        zap					;clear P-Reg and Accu
        rpt	#$val(noUniforms)-1		;
         add	*+,15       			;Accu = sum [ravs(i)]
        mar	*,AR7				;
	sach	*,1				;store Accu into output
        }

	execTime { 
                 return 10+29*int(noUniforms);
	}
}


