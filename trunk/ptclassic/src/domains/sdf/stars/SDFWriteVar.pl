defstar {
  name { WriteVar }
  domain { SDF }
  derivedfrom {SDFSharedMem}
  version { $Id$ }
  author { Stefan De Troch (IMEC) }
  copyright{ 
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
}
  desc {}
  input
  {
    name { in }
    type { float }
  }
  state
  {
    name { name }
    type { string }
    default { "0" }
    desc { Name of the register }
  }
  go
  {
    float val;

    val = float(in%0);
    SDFSharedMem::WriteVar(name,val);
  }
}
