defstar	{
  name { CSynchComm }
  domain { VHDL }
  desc { Base class for VHDL-CGC synchronous communication }
  version { @(#)VHDLCSynchComm.pl	1.7 01/30/97 }
  author { Michael C. Williamson, Jose Luis Pino }
  copyright {
Copyright (c) 1993-1997 The Regents of the University of California.
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
  }
	codeblock (uses) {
library IEEE,PTVHDLSIM;
use IEEE.STD_LOGIC_1164.all;
	}
  initCode {
    addCode(uses, "top_uses", "std_logic");
  }
}
