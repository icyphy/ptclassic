defstar {
	name { Send }
	domain { CG }
	derivedFrom { CGVarTime }
	desc {
Generates conditional code, depending on the value of
the Boolean state "control".
	}
	version { @(#)CGSend.pl	1.8	01 Oct 1996 }
	author { Soonhoi Ha }
	copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { CG demo library }
	htmldoc {
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
		addCode(block);
		StringList out;
		out += "// ";
		out += "From processor ";
		out += orgProc;
		out += "   to processor ";
		out += destProc;
		addCode(out);
	}
}

