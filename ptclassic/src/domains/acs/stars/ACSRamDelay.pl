defcorona {
	name { RamDelay }
	domain { ACS }
	desc {
	    Generates a pipelined delay element using FPGA RAM
	}
	version {$Id$}
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
