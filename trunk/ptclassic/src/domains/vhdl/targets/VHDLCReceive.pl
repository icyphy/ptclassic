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
    default {0}
  }
  defstate {
    name {rtype}
    type {string}
    default {"type"}
  }
  setup {
    if (strcmp(output.resolvedType(), "INT") == 0) 
      rtype = "integer";
    else if (strcmp(output.resolvedType(), "FLOAT") == 0) 
      rtype = "real";
    else
      Error::abortRun(*this, output.resolvedType(), ": type not supported");

    numXfer = output.numXfer();     
//  VHDLCSynchComm::setup();      
  }

// Called only once, after the scheduler is done
  begin {
    // Call method to wire up a C2V VHDL entity
    targ()->registerC2V(int(pairNumber), numXfer, output.resolvedType());
  }

  go {
    // Add code to synch at beginning of main.
    StringList preSynch;
    preSynch << "C2V" << rtype << int(pairNumber) << "_go" << " <= '0';\n";
    preSynch << "wait on " << "C2V" << rtype << int(pairNumber) << "_done" << "'transaction;\n";
    preSynch << "$ref(output)" << " := " << "C2V" << rtype << int(pairNumber) << "_data;\n";
  
//  addCode(preSynch, "preSynch");
    addCode(preSynch);
  }

}
