defstar {
  name { WriteVar }
  domain { SDF }
  derivedfrom { SDFSharedMem }
  version { $Id$ }
  author { Stefan De Troch (IMEC) }
  copyright{ 
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
}
  location { SDF main library }
  desc {
Write the value of the input to a floating-point variable in shared memory.
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
