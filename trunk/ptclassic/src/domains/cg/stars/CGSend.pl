defstar {
	name { Send }
	domain { CG }
	derivedFrom { CGVarTime }
	desc {
Generates conditional code, depending on the value of
the Boolean state "control".
	}
	version { $Id$ }
	author { Soonhoi Ha }
	copyright { 1991 The Regents of the University of California }
	location { CG demo library }
	explanation {
This star exists only for demoing the generic CG domain.
It outputs lines of comments, instead of code.
	}
	private {
		int orgProc;
		int destProc;
	}
	input {
		name {input}
		type {FLOAT}
	}
	output {
		name {output}
		type {FLOAT}
	}
	method {
		name { registerProcs }
		access { public }
		arglist { "(int from, int to)" }
		type { "void" }
		code {
			orgProc = from;
			destProc = to;
		}
	}
	ccinclude { "StringList.h" }

	codeblock (block) {
// CGSend Block
	}
	go {
		gencode(block);
		StringList out;
		out += "// ";
		out += "From processor ";
		out += orgProc;
		out += "   to processor ";
		out += destProc;
		addCode(out);
	}
}

