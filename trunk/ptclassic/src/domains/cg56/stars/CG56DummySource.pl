defstar {
	name { DummySource }
	domain { CG56 }
	desc { Dummy source with selectable size }
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
		descriptor { Number of samples to "write"}
	}
	output {
		name {output}
		type {FIX}
	}
	start {
		output.setSDFParams(int(n),int(n)-1);
	}
	codeblock (src) {
; write $val(n) samples starting at $ref(output)
	}
	go {
		gencode(src);
	}
}

