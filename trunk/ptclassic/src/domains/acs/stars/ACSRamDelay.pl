defcorona {
	name { RamDelay }
	domain { ACS }
	desc {
	    Generates a pipelined delay element using FPGA RAM
	}
	version {@(#)ACSRamDelay.pl	1.0	2 March 1999}
	author { K. Smith }
	copyright {
	}
	location { ACS main library }
	input {
		name {input}
		type {FLOAT}
	}
	output {
		name {output}
		type {FLOAT}
	    }
	defstate {
		name { delays }
		type { int }
		default { 1 }
		desc { Pipeline delays. }
	}
}  
