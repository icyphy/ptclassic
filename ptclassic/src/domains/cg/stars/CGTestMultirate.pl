defstar {
	name { TestMultirate }
	domain { CG }
	derivedFrom { CGVarTime }
	desc {
This star consumes and produces any fixed number of tokens
on any number of inputs and outputs.  The output value is always
zero.  IT IS INTENDED FOR TESTING SCHEDULERS ONLY.
	}
	version { @(#)CGTestMultirate.pl	1.1	4/19/96 }
	author { Praveen Murthy, based on SDFTestMultirate.pl}
	copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { CG demo library }
	inmulti {
		name { input }
		type { float }
	}
	outmulti {
		name { output }
		type { float }
	}
	state {
	  name { consume }
	  type { intarray }
	  default { 1 }
	  descriptor { number of inputs consumed for each port }
	}
	state {
	  name {produce }
	  type {intarray }
	  default { 1 }
	  descriptor { number of tokens produced on each output }
	}
	setup {
	  if ((input.numberPorts() != consume.size()) ||
	      (output.numberPorts() != produce.size())) {
	    Error::abortRun(*this,
              "list of number of tokens consumed or produced does not match"
	      " the actual number of inputs or outputs.");
	    return;
	  }
	  MPHIter nexti(input);
	  DFPortHole *p;
	  int i = 0;
	  while ((p = (DFPortHole*)nexti++) != 0) {
	    p->setSDFParams(consume[i],1);
	    i++;
	  }
	  MPHIter nexto(output);
	  i = 0;
	  while ((p = (DFPortHole*)nexto++) != 0) {
	    p->setSDFParams(produce[i],1);
	    i++;
	  }
	}
	go {
		addCode(code);
	}
	codeblock(code) {
// code from CGTestMultirate
	}
}
