defstar	{
  name { CSend }
  domain { VHDL }
  desc { VHDL to CGC sunchronous send star }
  version { $Id$ }
  author { Michael C. Williamson, Jose Luis Pino }
//  derivedFrom { CSynchComm }
  copyright {
Copyright (c) 1994, 1993 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright	for copyright notice,
limitation of liability, and disclaimer	of warranty provisions.
}
  location { VHDL Target Directory }
  explanation { }
  public {
    int numXfer;
  }
  protected {
    friend class VHDLTarget;
  }
  hinclude { "VHDLTarget.h" }
  input {
    name {input}
    type {ANYTYPE}
  }
  defstate {
    name {pairNumber}
    type {int}
    default {0}
  }
  defstate {
    name {rtype}
    type {string}
    default {"type"}
  }
  setup {
    if (strcmp(input.resolvedType(), "INT") == 0) 
      rtype = "integer";
    else if (strcmp(input.resolvedType(), "FLOAT") == 0) 
      rtype = "real";
    else
      Error::abortRun(*this, input.resolvedType(), ": type not supported");

    numXfer = input.numXfer();
//  VHDLCSynchComm::setup();
  }

// Called only once, after the scheduler is done
  begin {
    // Call method to wire up a V2C VHDL entity
    targ()->registerV2C(int(pairNumber), numXfer, input.resolvedType());
  }

  go {
    // Add code to synch at end of main.
    StringList postSynch;
    postSynch << "V2C" << rtype << int(pairNumber) << "_data" << " <= " << "$ref(input)" << ";\n";
    postSynch << "V2C" << rtype << int(pairNumber) << "_go" << " <= " << "'0';\n";
    postSynch << "wait on " << "V2C" << rtype << int(pairNumber) << "_done" << "'transaction;\n";

//  addCode(postSynch, "postSynch");
    addCode(postSynch);
  }

}
