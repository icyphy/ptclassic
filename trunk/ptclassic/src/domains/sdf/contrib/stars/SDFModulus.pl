defstar {
	name {Modulus}
	domain {SDF}
	desc { Integer Modulus. }
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
		name { Modulus }
		type { int }
		default { 2 }
		desc { Modulus }
	}
	go {
		output%0 << ( int(input%0) % int(Modulus) );
	}
}
