defstar {
	name {IntDiv}
	domain {SDF}
	desc { Integer Divide. }
	author { N. Becker }
	version { $Id$ }
        copyright {
Copyright (c) 1990-1994 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
        }
	location { SDF user contribution library }
	input {
		name {input}
		type {int}
	}
	output {
		name {output}
		type {int}
	}
	defstate {
		name { Divisor }
		type { int }
		default { 2 }
		desc { Divisor }
	}
	go {
		output%0 << ( int(input%0) / int(Divisor) );
	}
}
