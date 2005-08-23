defstar {
	name { IIDGaussian }
	domain { C50 }
	desc { Gaussian noise source }
	version {@(#)C50IIDGaussian.pl	1.7	05/26/98}
	author { A. Baensch, G. Arslan }
	copyright {
Copyright (c) 1990-1998 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { C50 main library }
	htmldoc {
<p>
<a name="Gaussian noise"></a>
<a name="noise, Gaussian"></a>
This star generates a sequence of random output samples with a Gaussian
distribution (mean = 0, standard deviation = 0.1).
<p>
<a name="central limit theorem"></a>
According to the central limit theorem, the sum of N random variables
approaches a Gaussian distribution as N approaches infinity.
This star generates an output number by summing <i>noUniforms</i> uniform
random variables.
<p>
The parameters <i>seed</i> and <i>multiplier</i> control the generation of the
random number. 
	}

	output {
		name { output }
		type { fix }
	}
	// FIXME: States are incompatible with SDF version
	state  {
		name { noUniforms }
		type { int }
		desc { uniform random variables. }
		default { 16 }
      	}

	state {
		name { seed }
		type { int }
		default { 0.9877315592 }
		desc { internal }
		attributes { A_BMEM|A_NONCONSTANT|A_NONSETTABLE }
	}
	state {
		name { ravs }
		type { fixarray }
		default { "0" }
		desc { internal }
		attributes { A_BMEM|A_NONCONSTANT|A_NONSETTABLE }
	}

	setup {
		ravs.resize(noUniforms);
	}		
	
	go {
		addCode(std);
	}		

	codeblock(std) {
	mar	*,AR1
	lar	AR0,#$addr(ravs)		;Address ravs    => AR0
	lar     AR1,#$addr(seed)		;Address seed    => AR1
	lar	AR7,#$addr(output)		;Address output  => AR7
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
	lar	AR0,#$addr(ravs)		;Address from 1st value => AR0
	zap					;clear P-Reg and Accu
	rpt	#$val(noUniforms)-1		;
	add	*+,15       			;Accu = sum [ravs(i)]
	mar	*,AR7				;
	sach	*,1				;store Accu into output
	}

	execTime { 
		 return 10 + 29*int(noUniforms);
	}
}
