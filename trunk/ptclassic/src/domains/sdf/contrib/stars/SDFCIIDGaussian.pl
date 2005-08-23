defstar {
	name { CIIDGaussian }
	domain { SDF } 
	version { @(#)SDFCIIDGaussian.pl	1.6 3/7/96 }
	location { SDF user contribution library }
	desc { Complex IID Gaussian generator. }
	author { N. Becker }
        copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
        }
	output {
		name { output }
		type { complex }
	}
	defstate {
	  name{ variance }
	  type{ float }
	  default{ "1.0" }
	  desc{ Variance of distribution. }
	}
	hinclude { <Normal.h> }
	header {
#include <ACG.h>
extern ACG* gen;
	}
	protected {
	  Normal *random;
	}
	constructor {
	  random = 0;
	}
	destructor {
	  LOG_DEL; delete random;
	}
	setup {
	  LOG_DEL; delete random;
	  LOG_NEW; random = new Normal(0., double(variance), gen);
	}
        go {
	  Complex randomValue( (*random)(), (*random)() );
	  output%0 << randomValue;
	}
}
