defstar {
	name { DummySink }
	domain { CG56 }
	desc { Dummy sink with selectable size }
	version { $Id$ }
	author { J. T. Buck }
	copyright { 1992 The Regents of the University of California }
	location { CG56 demo library }
	explanation {
This star just copies data from the input to the a register -- of course,
everything but the last value is overwritten.  It is here to help test
memory allocation, code generation, and the addr2 macro.
	}

	execTime {
		return 0;
	}
	state {
		name {n}
		type {int}
		default {2}
		descriptor { Number of samples to "read"}
	}
	state {
		name {i}
		type {int}
		default { 0 }
		descriptor { internal }
		attributes { A_NONSETTABLE|A_NONCONSTANT }
	}
	input {
		name {input}
		type {FIX}
	}
	start {
		input.setSDFParams(int(n),int(n)-1);
	}
	codeblock (src) {
	move	x:$addr2(input,i),a	; dummy read, offset $val(i)
	}
	go {
		for (i = 0; int(i) < int(n); i = int(i) + 1) {
			gencode(src);
		}
	}
}

