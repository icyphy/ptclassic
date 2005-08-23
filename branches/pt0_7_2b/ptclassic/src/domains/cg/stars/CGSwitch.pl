defstar {
	name { Switch }
	domain { CG }
	derivedFrom { CGVarTime }
	desc {
This star requires a BDF scheduler!

Switches input events to one of two outputs, depending on
the value of the control input.  If control is true, the
value is written to trueOutput; otherwise it is written to
falseOutput.
	}
	version { @(#)CGSwitch.pl	1.4 1/1/96 }
	author { J. Buck }
	copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { CG demo library }
	input {
		name {input}
		type {ANYTYPE}
	}
	input {
		name {control}
		type {int}
	}
	output {
		name {trueOutput}
		type {=input}
	}
	output {
		name {falseOutput}
		type {=input}
	}
	constructor {
		trueOutput.setRelation(DF_TRUE, &control);
		falseOutput.setRelation(DF_FALSE, &control);
	}
	codeblock (foo) {
// Switch star
	}
	go {
		addCode(foo);
	}
}

