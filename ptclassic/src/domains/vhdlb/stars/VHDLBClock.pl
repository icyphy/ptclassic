defstar {
	name { Clock }
	domain {VHDLB}
	desc {
Output a signal which alternates between 0 and 1 with the given period.
	}
        version { $Id$ }
	author { M. C. Williamson }
	copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { VHDLB main library }
	output {
		name { output }
		type { int }
	}
        defstate {
                name { period }
	        type { int }
                default { 1 }
                desc { The clock period, in nsec }
        }
	go {
	}
}
