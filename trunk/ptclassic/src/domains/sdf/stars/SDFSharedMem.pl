defstar {
  name { SharedMem }
  domain {SDF}
  version { $Id$ }
  author { Stefan De Troch (IMEC) }
  copyright{ 
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.

}
  desc { Base class for shared memory }
  header {

    class VarReg {
    public:
      VarReg(): contents(0) {}

      float GetContents() { return contents; }
      void SetContents(float value) { contents= value; }

    private:
      float contents;
    };

  }
  hinclude { "HashTable.h" }
  explanation {}
  code {

    // instantiate the static register list
    HashTable SDFSharedMem::regList;
  }
  protected {
    static HashTable regList;
  }

  method {
    name {ReadVar}
    arglist { "(const char *name, float& value)" }
    type { int }
    code
    {
      VarReg* reg;

      if (regList.hasKey(name)) {
	reg = (VarReg *) regList.lookup(name);

	value = reg->GetContents();
	return 1;
      }
      else
	return 0;
    }
  }
  method {
    name { RemoveVar }
    arglist { "(const char *name)" }
    type { void }
    code
    {
      VarReg* reg;

      if (regList.hasKey(name)) {
        reg = (VarReg *) regList.lookup(name);
	regList.remove(name);
	LOG_DEL; delete reg;
      }
    }
  }
  method
  {
    name{ WriteVar }
    arglist { "(const char *name, float value)" }
    type { void }
    code
    {
      VarReg* reg;

      if (regList.hasKey(name)) {
        reg = (VarReg *) regList.lookup(name);
      }
      else {
	LOG_NEW; reg = new VarReg;
	regList.insert(name,reg);
      }
      reg->SetContents(value);
    }
  }
}
