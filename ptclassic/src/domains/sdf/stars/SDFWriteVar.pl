defstar {
  name { WriteVar }
  domain { SDF }
  derivedfrom { SDFSharedMem }
  version { @(#)SDFWriteVar.pl	1.7 11/28/95 }
  author { Stefan De Troch (IMEC) }
  copyright{ 
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
}
  location { SDF main library }
  desc {
Write the value of the input to a double-precision floating-point variable
in shared memory.  Use the ReadVar star to read values from the shared memory.

WARNING: This star may produce unpredictable results, since the results
will depend on the precedences in the block diagram in which it appears
as well as the scheduler (target) used.
  }
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
  protected
  {
    char* lastname;
  }
  constructor {
    lastname = 0;
  }
  destructor
  {
    if (lastname) {
      SDFSharedMem::RemoveVar(lastname);
    }
    delete [] lastname;
  }
  setup {
    const char* namestring = name;
    if (lastname) {
      SDFSharedMem::RemoveVar(lastname);
      delete [] lastname;
    }
    lastname = savestring(namestring);
  }
  go
  {
    double val = double(in%0);

    // Write val to register 'name' and create 'name' if it doesn't exist
    SDFSharedMem::WriteVar(name, val);
  }
}
