defstar {
	name { Sleep }
	domain { CGC }
	desc {
Suspend execution for an interval (in milliseconds).
The input is passed to the output when the process resumes.
	}
	version { $Id$ }
	author { E. A. Lee }
	copyright {
Copyright (c) 1990, 1991, 1992 The Regents of the University of California.
All rights reserved.
See the file ~ptolemy/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { CGC main library }
	input {
		name { input }
		type { float }
	}
	output {
		name { output }
		type { float }
	}
	defstate {
		name { interval }
		type { int }
		default { "10" }
		desc { Time in milliseconds to sleep. }
	}
	constructor {
		noInternalState();
	}
	initCode {
		addInclude("<sys/types.h>");
		addInclude("<sys/time.h>");
	}
	go {
		addCode(std);
	}
	codeblock (std) {
	    {
	    static struct timeval delay;
	    delay.tv_sec = $val(interval)/1000;
	    delay.tv_usec = ($val(interval)%1000)*1000;
	    (void) select(0, (SELECT_MASK *) 0, (SELECT_MASK *) 0,
        	    (SELECT_MASK *) 0, &delay);
	    }
	    $ref(output) = $ref(input);
	}
}
