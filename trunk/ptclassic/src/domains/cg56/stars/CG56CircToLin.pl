defstar {
	name { CircToLin }
	domain { CG56 }
	desc {
Copy data from a circular buffer to a linear buffer.  If N is zero (default),
the number transferred per execution equals the number read per execution
by the star that reads from the output. 
	}
	version { $Id$ }
	author { J. Buck }
	copyright { 1992 The Regents of the University of California }
	location { CG56 demo library }
	explanation {
This star is normally inserted automatically, as needed.  Data movement
is repeated inline so may not be efficient for large N.
	}
	input {
		name { input }
		type { anytype }
		attributes { P_CIRC}
	}
	output {
		name { output }
		type { anytype }
	}
	protected {
		int n;
	}
	defstate {
		name { N }
		type { int }
		desc { number to transfer per execution }
		default { 0 }
	}
	start {
		n = int(N);
		if (n == 0)
			n = output.far()->numberTokens;
		input.setSDFParams(n,n-1);
		output.setSDFParams(n,n-1);
	}
	defstate {
		name { i }
		type { int }
		desc { internal }
		default { 0 }
		attributes { A_NONCONSTANT|A_NONSETTABLE }
	}
	defstate {
		name { ptr }
		type { int }
		desc { storage for pointer register (internal) }
		default { 0 }
		attributes { A_YMEM|A_NOINIT|A_NONSETTABLE }
	}
	initCode { gencode(init);}
	go {
		gencode(setupC);
		for (int j = 0; j < n; j++) {
			i = j;
			gencode(one);
		}
		gencode(restore);
	}
	
	exectime {
		return 4+2*n;
	}


	codeblock(init) {
	move	#>$addr(input),x0
	move	x0,$ref(ptr)
	}

	codeblock(setupC) {
	move	$ref(ptr),r0
	move	#$size(input)-1,m0
	nop			; can't read from circbuf right away
	}
	codeblock(one) {
	move	x:(r0)+,x0
	move	x0,$ref2(output,i)
	}
	codeblock(restore) {
	move	r0,$ref(ptr)
	move	m7,m0
	}
}
