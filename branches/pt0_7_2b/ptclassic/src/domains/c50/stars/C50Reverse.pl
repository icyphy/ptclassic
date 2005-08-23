defstar {
	name { Reverse }
	domain { C50 }
	desc {
On each execution, read a block of "N" samples (default 64)
and writes them out backwards.
	}
	version {@(#)C50Reverse.pl	1.7	05/26/98}
	author { A. Baensch, Luis Gutierrez, G. Arslan }
	copyright {
Copyright (c) 1990-1998 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { C50 main library }

	input {
		name {input}
		type {ANYTYPE}
	}
	output {
		name {output}
		type {=input}
	}
	state {
                name {N}
                type {int}
                default {64}
		desc {Number of particles read and written}
	}
	protected {
		// number of inputs
		int numInputs;
	}

	setup {
                input.setSDFParams(int(N),int(N)-1);
                output.setSDFParams(int(N),int(N)-1);
		numInputs = int(N);
	
        }

 	codeblock(std,"") {
        mar 	*,AR0			
	lar	ar0,#($addr(input)+@(numInputs-1))
        rpt     #@(numInputs-1)		;for number of N
	bldd	*-,#$addr(output)	;output(i) = input(N-i)
        }

	codeblock(cmplx,""){
	lacc	#@numInputs
	samm	brcr
	mar	*,ar1
	lacc	#$addr(output)
	samm	bmar
	lar	ar1,#($addr(input)+@(2*(numInputs - 1)))
	rptb	$label(rev)
	rpt	#1
	bldd	*+,bmar
	sbrk	#4
	lamm	bmar
	add	#2
$label(rev)
	samm	bmar
	}

        codeblock(one) {
	lmmr	ar1,#$addr(input)
	nop
	nop
	smmr	ar1,#$addr(output)
	}

	codeblock(oneCx){
	lmmr	ar0,#$addr(input)
	lmmr	ar1,#($addr(input)+1)
	smmr	ar0,#$addr(output)
	smmr	ar1,#($addr(output)+1)
	}

        go {
                if ( numInputs > 1) {
			if (input.resolvedType() == COMPLEX)
				addCode(cmplx());
			else
				addCode(std());
		} else {
			if (input.resolvedType() == COMPLEX)
				addCode(oneCx);
			else
				addCode(one);
		}
        }
        exectime {
                if ( numInputs > 1) {
			if (input.resolvedType() == COMPLEX)
				 return (7*numInputs + 7);
			else
				 return (3 + numInputs);
		} else {
			if (input.resolvedType() == COMPLEX)
				return 4;
			else 
				return 2;
		}
	}
}

