defstar {
	name { DummySrcInt }
	domain { CG }
	desc { Source star that generates no code.  }
	version { $Id$ }
	author { Raza Ahmed and Brian L. Evans }
	copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	explanation {
The star is used to create connected galaxies (universes) without
requiring any resources.  For example, it is used by the CGCostTarget  
to create dummy universes to measure implementation costs of individual
stars.
	}
	location { CG main library }
	output {
		name {output}
		type {INT}
	}
	defstate {
		name {n}
		type {int}
		desc {number of samples to write}
		default {1}
	}
	setup {
		output.setSDFParams(n,int(n)-1);
	}
	go {
	}
}
