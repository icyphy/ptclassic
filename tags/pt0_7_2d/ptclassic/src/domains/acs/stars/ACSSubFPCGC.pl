defcore {
    name { Sub }
    domain { ACS }
    coreCategory { FPCGC }
    corona { Sub } 
    desc { Output the "pos" input minus the "neg" input. }
    version { @(#)ACSSubFPCGC.pl	1.2 09/08/99 }
    author { James Lundblad }
    copyright {
Copyright (c) 1998 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
    }
    location { ACS main library }
    constructor {
	noInternalState();
    }
    go { 
      addCode(init); 
      addCode("\t$ref(output) -= $ref(neg);\n");
    }
    codeblock (init) {
      $ref(output) = $ref(pos);
    }
    exectime {
      return 2;
    }
}
    
