defstar {
	name {Pack}
	domain {SDF}
	desc { This star takes two inputs and creates a Particle
		 vector from its input particles.}
	version { @(#)SDFPack.pl	1.1   03/21/97 }
	author { R. P. Gunturi }
	copyright {
Copyright (c) 1990- The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { SDF main library }
	defstate {
	  name { fieldName }
          type { StringArray }
	  default { "1 2" }
	  desc { classifies inputs by the field name }
	}
	input {
	  name { input1 }
	  type { ANYTYPE }
	}
	input {
	  name {input2}
	  type {ANYTYPE}
	}
	output {
		name {output}
		type {Message}
	}
	ccinclude { "Message.h", "ParticleVector.h" }
	go {

	  const int len = 2;

	  ParticleVector * pv = new ParticleVector(len);

	  if (fieldName.size() != len) {
	    Error::abortRun(*this, "Incorrect field specification");
	    return;
	  }
	  pv->insert(1, fieldName[0], &(input1%0));
	  pv->insert(2, fieldName[1], &(input2%0));

	  Envelope pkt(*pv);
	  output%0 << pkt;

	}
}


