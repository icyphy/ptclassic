defstar {
  name { ReadVar }
  domain { SDF }
  version { $Id$ }
  author { Stefan De Troch (IMEC) }
  copyright{ 
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
}
  derivedfrom { SDFSharedMem }
  desc {
Output the value of a double-precision floating variable from a shared memory.
Use the WriteVar star to write values into the shared memory.

WARNING: This star may produce unpredictable results, since the results
will depend on the precedences in the block diagram in which it appears
as well as the scheduler (target) used.
}
  location { SDF main library }
  output
  {
    name { out }
    type { float }
  }
  state
  {
    name { name }
    type { string }
    default { "0" }
    desc { Name of the register }
  }
  state
  {
    name { initial }
    type { float }
    default { 0 }
    desc { Default value for output }
  }
  go
  {
    double val = 0.0;			// quiet the cfront C++ compiler

    // method ReadVar() sets the value of val (passed by reference)
    if ( !SDFSharedMem::ReadVar(name, &val) ) {
	val = initial;
    }

    out%0 << val;
  }
}