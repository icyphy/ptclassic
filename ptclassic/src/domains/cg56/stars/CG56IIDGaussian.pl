defstar {
	name { IIDGaussian }
	domain { CG56 }
	desc {Gaussian Noise Source}
	version { $Id$ }
	author { Chih-Tsung Huang, Kennard White (ported from Gabriel) }
	copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { CG56 main library }
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
<h3>BUGS:</h3>
<p>
This needs to be filled in.  For now, there is no seed and multiplier
parameter; the default seed parameter from Gabriel is always used.
We'd really need to use a 48-bit integer to get the same functionality.
This can be done with g++ (type "long long"), but it isn't portable.
<p>
The Gabriel version used l:aa addressing for the accum address.
This failed when the accumulator is not :aa addressable (high memory), 
so it now moves the address into a register and uses l:(rn) addressing.
<p>
The loop that calculates the series of uniform variables should really
be pipelined better and should keep the accum value in register instead
of flushing and reloading to/from memory every iteration.
<p>
Incorrect code will probably be generated generated if the number of uniform
variables used is less than 2 or 3.
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

        setup {
                ravs.resize(noUniforms);
        }		
        initCode {
                addCode(block);
        }		
        go {
                addCode(std);
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
	move	#$addr(accum),r1		; put accum ptr for non aa:
        .LOOP	#$val(noUniforms)
        move    #>10916575,y1
        move    l:(r1),x
        mpy     x0,y1,a    #>12648789,y0
        mac     +x1,y0,a   y1,b1
        asr     a          y0,b0
        mpy     x0,y0,a    a0,x1
        addr    b,a
        add     x1,a       #>363237,x0
        move    a1,y0
        mpy     x0,y0,a    a10,l:(r1)
        move    a,x:(r0)+
	.ENDL

; generate Gaussian, mean=0, sigma=0.1
        move	#$addr(ravs),r0
        clr	a
        move	x:(r0)+,x0
        rep	#$val(noUniforms)-1
        add	x0,a        x:(r0)+,x0
        add	x0,a
        move	a,$ref(output)
        }

	execTime { 
                 return 10+15*int(noUniforms);
	}
}


