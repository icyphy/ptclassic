defstar {
	name { DummySink }
	domain { CG56 }
	desc { Dummy sink with selectable size }
	version { $Id$ }
	author { J. T. Buck }
	copyright { 1992 The Regents of the University of California }
	location { CG56 demo library }
	execTime {
		return 0;
	}
	state {
		name {n}
		type {int}
		default {2}
		descriptor { Number of samples to "read"}
	}
	input {
		name {input}
		type {FIX}
	}
	start {
		input.setSDFParams(int(n),int(n)-1);
	}
	codeblock (src) {
; read $val(n) samples starting at $ref(input)
	}
	go {
		gencode(src);
	}
}

