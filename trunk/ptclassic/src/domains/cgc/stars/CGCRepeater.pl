defstar {
	name { Repeater }
	domain { CGC }
	desc {
This star uses the "control" input value to specify how many
times to repeat the "input" value on "output".
The generated code is just copying input to output since this star
will be placed inside a loop structure
	}
	version { $Id$ }
	author { Soonhoi Ha }
	copyright {
Copyright (c) 1990-1995 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { CGC main library }
	input {
		name { input }
		type { ANYTYPE }
	}
	input {
		name { control }
		type { int }
	}
	output {
		name { output }
		type { =input }
		num { 0 }
	}
	method {
		name { readTypeName }
		access { public }
		type { "const char *" }
		code { return "UpSample"; }
	}
	codeblock (block) {
	$ref(output) = $ref(input);
	}
	go {
		addCode(block);
	}
}


