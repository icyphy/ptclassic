defstar {
	name { Fork }
	domain { AnyAsm }
	desc {
Fork star for assembly-code domains.  No code is generated at runtime.
	}
	version {$Id$}
	author { J. Buck }
	copyright { 1992 The Regents of the University of California }
	location { included in all assembly domains }
	input {
		name {input}
		type {anytype}
	}
	outmulti {
		name {output}
		type {=input}
	}
	exectime {
		return 0;
	}
	start {
		forkInit(input,output);
	}
}
