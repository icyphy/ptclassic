defstar {
	name { DummySource }
	domain { CG56 }
	desc { Dummy source with selectable size }
	version { $Id$ }
	author { J. T. Buck }
	copyright { 1992 The Regents of the University of California }
	location { CG56 demo library }
	explanation {
This star just copies whatever is in the a register (and who knows)
to the output, n times.  It is here to help test memory allocation,
code generation, and the ref2 macro.
	}
	execTime {
		return int(n);
	}
	state {
		name {n}
		type {int}
		default {2}
		descriptor { Number of samples to "write"}
	}
	state {
		name {i}
		type {int}
		default { 0 }
		descriptor { internal }
		attributes { A_NONSETTABLE|A_NONCONSTANT }
	}
	output {
		name {output}
		type {FIX}
	}
	start {
		output.setSDFParams(int(n),int(n)-1);
	}
	codeblock (src) {
	move	a,$ref2(output,i)	; dummy write, offset $val(i)
	}
	go {
		for (i = 0; int(i) < int(n); i = int(i) + 1) {
			gencode(src);
		}
	}
}
