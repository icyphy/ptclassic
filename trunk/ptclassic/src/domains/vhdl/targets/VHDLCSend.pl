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
    default {21}
  }
setup {
  numXfer = input.numXfer();
//  VHDLCSynchComm::setup();
}
	
go {
  // Add code to synch at end of main.
  StringList postSynch;
  postSynch << "V2C" << int(pairNumber) << "_data" << " <= " << "$ref(input)" << ";\n";
  postSynch << "V2C" << int(pairNumber) << "_go" << " <= " << "'0';\n";
  postSynch << "wait on " << "V2C" << int(pairNumber) << "_done" << "'transaction;\n";

  addCode(postSynch, "postSynch");
  // Call method to wire up a V2C VHDL entity
  targ()->registerV2C(int(pairNumber),numXfer);
}

}
