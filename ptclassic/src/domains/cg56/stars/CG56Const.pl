defstar {
	name { FixDC }
	domain { CG56 }
	desc { Constant source }
	version { $Id$ }
	author { E. A. Lee,  J. Pino, J. Buck }
	copyright { 1992 The Regents of the University of California }
	location { CG56 demo library }
	explanation {
There are no runtime instructions associated with this star.  The
output buffer is initialized with the specified DC value.
	}
	output {
		name {output}
		type {FIX}
	}
	state {
		name {value}
		type {FIX}
		default {0.001}
		descriptor { Constant value to go to the output. }
	}
	codeblock (org) {
; initialization code for star $fullname() - class CG56DC	
	org	$ref(output)
	}
	codeblock (dc) {
	dc	$val(value)
	}
	codeblock (orgp) {
	org	p:
	}
	initCode {
		gencode(org);
		for (int i=0 ; i<output.bufSize() ; i++) gencode(dc);
		gencode(orgp);
	}
	execTime {
		return 0;
	}
}
