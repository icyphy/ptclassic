defstar {
	name { IIDGaussian }
	domain { CG56 }
	desc {Gaussian Noise Source}
	version { $Id$ }
	author { Chih-Tsung Huang, ported from Gabriel }
	copyright { 1992 The Regents of the University of California }
	location { CG56 demo library }
        explanation {
.PP
This star generates a sequence of random output samples with a Gaussian
distribution (mean = 0, standard deviation = 0.1).
.PP
According to the central limit theorem, the sum of N random variables
approaches a Gaussian distribution as N approaches infinity.
This star generates an output number by summing \fIno_uniforms\fR uniform
random variables.
.PP
The parameters \fIseed\fR and \fImultiplier\fR control the generation of the
random number. 
.SH BUGS
This needs to be filled in.  For now, there is no seed and multiplier
parameter; the default seed parameter from Gabriel is always used.
We'd really need to use a 48-bit integer to get the same functionality.
This can be done with g++ (type "long long"), but it isn't portable.
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

        state {
                name { accum }
	        type { int }
	        default { 0 }
	        desc { internal }
	        attributes {
		    A_SYMMETRIC|A_RAM|A_NONCONSTANT|A_NONSETTABLE|A_NOINIT
	        }
        }
        state {
                name { ravs }
	        type { fixarray }
	        default { "0" }
	        desc { internal }
	        attributes { A_XMEM|A_NONCONSTANT|A_NONSETTABLE|A_NOINIT }
        }

        start {
                ravs.resize(noUniforms);
        }		
        initCode {
                gencode(block);
        }		
        go {
                gencode(std);
        }		

        codeblock(block) {
; initial seed for $fullname()
        org     x:$addr(accum)
        dc      1246684
        org     p:

        org     y:$addr(accum)
        dc      8123721
        org     p:

; intermediate storage for random variables -- $fullname()
        org     $ref(ravs)
        bsc     $val(noUniforms),0
        org     p:
        }
                       
        codeblock(std) {
        move    #$addr(ravs),r0
        do      #$val(noUniforms),$label(cont)
        move    #>10916575,y1
        move    l:<$addr(accum),x
        mpy     x0,y1,a    #>12648789,y0
        mac     +x1,y0,a   y1,b1
        asr     a          y0,b0
        mpy     x0,y0,a    a0,x1
        addr    b,a
        add     x1,a       #>363237,x0
        move    a1,y0
        mpy     x0,y0,a    a10,l:<$addr(accum)
        move    a,x:(r0)+
$label(cont)

; generate Gaussian, mean=0, sigma=0.1
        move    #$addr(ravs),r0
        clr a
        move  x:(r0)+,x0
        rep   #$val(noUniforms)-1
        add   x0,a        x:(r0)+,x0
        add   x0,a
        move  a,$ref(output)
        }

	execTime { 
                 return 10+15*int(noUniforms);
	}
}


