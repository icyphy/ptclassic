defstar {
	name { HostIn  }
	domain { CG56 }
	desc { Data Input }
	version { $Id$ }
	author { Chih-Tsung, ported from Gabriel }
	copyright { 1992 The Regents of the University of California }
	location { CG56 demo library }
	explanation {
DSP56000 -- Input data from host to DSP via host port.
	}
	execTime {
		return 2;
	}
	output {
		name {output}
		type {FIX}
	}
	state {
		name {command}
		type {string}
		desc { }
		default {""}
	}
	codeblock (std) {
$label(l)
	jclr	#m_hrdf,x:m_hsr,$label(l)
	jclr	#1,x:m_pbddr,$label(l)
	movep	x:m_hrx,$ref(output)
	}
	go {
		addRunCmd(command,"\n");
		gencode(std);
	}
}
