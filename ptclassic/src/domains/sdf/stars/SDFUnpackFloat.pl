defstar {
	name {UnpackFloat}
	domain {SDF}
	desc { Take a Particle vector as input and output the user
		 specified field of this vector (casted to an 'int'). This star
                 has one input and one output. }
	version { @(#)SDFUnpackFloat.pl	1.1   03/21/97 }
	author { R. P. Gunturi }
	copyright {
Copyright (c) 1990- The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { SDF main library }
	defstate {
	  name { fieldSelect }
	  type { StringArray }
	  default { "1" }
	  desc { The parameter that is desired }
	}
	input {
	  name { in }
	  type { Message }
	}
	output {
		name { out }
		type { float }
	}
	ccinclude { "Message.h", "ParticleVector.h", "dataType.h" }

	go {
	 
	  Envelope pkt;
	  (in%0).getMessage(pkt);
	  
	  if (!pkt.typeCheck("ParticleVector")) {
	    Error::abortRun(*this, pkt.typeError("ParticleVector"));
	    return;
	  }
	  const ParticleVector * pv = (const ParticleVector
				       *) (pkt.myData());

	  Particle* p = pv->retrieve(fieldSelect[0]);
	  
	  if (p == NULL) {
	    Error::abortRun(*this, "Field specified is out of bounds");
	    return;
	  }
	  if (p->type() != FLOAT) {
	    Error::abortRun(*this, "Data types do not match");
	    return;
	  }

	  float value = float(*p);
	  out%0 << value;
	}
}




