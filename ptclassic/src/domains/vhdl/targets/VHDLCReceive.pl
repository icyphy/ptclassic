defstar {
  name { CReceive }
  domain { VHDL }
  desc { CGC to VHDL Receive star }
  version { $Id$ }
  author { Michael C. Williamson, Jose L. Pino }
//  derivedFrom {CSynchComm}
  copyright {
Copyright (c) 1994,1993 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
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
  output {
    name {output}
    type {ANYTYPE}
  }
  defstate {
    name {pairNumber}
    type {int}
    default {11}
  }
setup {
  numXfer = output.numXfer();     
//  VHDLCSynchComm::setup();      
}

go {
  // Add code to synch at beginning of main.
  StringList preSynch;
  preSynch << "C2V" << int(pairNumber) << "_go" << " <= '0';\n";
  preSynch << "wait on " << "C2V" << int(pairNumber) << "_done" << "'transaction;\n";
  preSynch << "$ref(output)" << " := " << "C2V" << int(pairNumber) << "_data;\n";
  
  addCode(preSynch, "preSynch");
  // Call method to wire up a C2V VHDL entity
  targ()->registerC2V(int(pairNumber),numXfer);
}

}
