defstar {
	name { BlackHole }
	domain { AnyAsm }
	version { $Id$}
	author { J. Buck }
	desc {
Black hole star for assembly-code domains.  No code is generated at runtime.
	}
	input {
		name {input}
		type {anytype}
		attributes {P_CIRC}
	}
	start {
		int n = input.far()->numXfer();
		input.setSDFParams(n,n-1);
	}
	exectime {
		return 0;
	}
}
